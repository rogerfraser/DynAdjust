//============================================================================
// Name         : dnasegment.cpp
// Author       : Roger Fraser
// Contributors : Dale Roberts <dale.o.roberts@gmail.com>
// Copyright    : Copyright 2017-2025 Geoscience Australia
//
//                Licensed under the Apache License, Version 2.0 (the "License");
//                you may not use this file except in compliance with the License.
//                You may obtain a copy of the License at
//
//                http ://www.apache.org/licenses/LICENSE-2.0
//
//                Unless required by applicable law or agreed to in writing, software
//                distributed under the License is distributed on an "AS IS" BASIS,
//                WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//                See the License for the specific language governing permissions and
//                limitations under the License.
//
// Description  : DynAdjust Network Segmentation library
//============================================================================

#include <dynadjust/dnasegment/dnasegment.hpp>

/// \cond
#include <math.h>

#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
/// \endcond

#include <include/config/dnaconsts.hpp>
#include <include/config/dnaoptions.hpp>
#include <include/config/dnaversion.hpp>
#include <include/exception/dnaexception.hpp>
#include <include/functions/dnaiostreamfuncs.hpp>
#include <include/functions/dnastringfuncs.hpp>
#include <include/functions/dnatemplatedatetimefuncs.hpp>
#include <include/functions/dnatemplatefuncs.hpp>
#include <include/functions/dnatemplatestnmsrfuncs.hpp>
#include <include/functions/dnatimer.hpp>
#include <include/io/aml_file.hpp>
#include <include/io/asl_file.hpp>
#include <include/io/bms_file.hpp>
#include <include/io/bst_file.hpp>
#include <include/io/map_file.hpp>
#include <include/io/seg_file.hpp>

namespace dynadjust {
namespace networksegment {

using namespace dynadjust::measurements;
using namespace dynadjust::exception;
using namespace dynadjust::iostreams;

// PImpl implementation struct
struct dna_segment::Impl {
    project_settings projectSettings_;
    _SEGMENT_STATUS_ segmentStatus_;
    bool isProcessing_;
    UINT32 currentBlock_;
    UINT32 currentNetwork_;  // current contiguous network ID

    std::string network_name_;  // network name
    UINT16 debug_level_;
    std::string output_folder_;

    // functors
    CompareClusterID<measurement_t, UINT32> clusteridCompareFunc_;

    // block vectors
    vstring vinitialStns_;
    vUINT32 v_ContiguousNetList_;  // vector of contiguous network IDs (corresponding to each block)
    vUINT32 vCurrJunctStnList_;
    vUINT32 vCurrInnerStnList_;
    vUINT32 vCurrMeasurementList_;

    vstn_t bstBinaryRecords_;
    vmsr_t bmsBinaryRecords_;
    vASL vAssocStnList_;
    vUINT32 vASLCount_;
    vUINT32 vAssocMsrList_;
    v_aml_pair vAssocFreeMsrList_;
    vUINT32 vfreeStnList_;
    vUINT32 vfreeMsrList_;  // vAssocMsrList_, less non-measurements and duplicate measurement references, sorted by
                            // ClusterID.
    v_string_uint32_pair stnsMap_;

    v_freestn_pair vfreeStnAvailability_;

    vvUINT32 vJSL_;
    vvUINT32 vISL_;
    vvUINT32 vCML_;

    std::ofstream debug_file;
    std::ofstream trace_file;

    double averageBlockSize_;
    UINT32 stationSolutionCount_;
    UINT32 minBlockSize_;
    UINT32 maxBlockSize_;

    Impl()
        : isProcessing_(false),
          currentBlock_(0),
          currentNetwork_(0),
          debug_level_(0),
          averageBlockSize_(0.0),
          stationSolutionCount_(0),
          minBlockSize_(0),
          maxBlockSize_(0) {}
};

dna_segment::dna_segment(): pImpl(std::make_unique<Impl>()) { pImpl->network_name_ = ""; }

dna_segment::~dna_segment() {}

double dna_segment::GetProgress() const {
    return ((pImpl->bstBinaryRecords_.size() - pImpl->vfreeStnList_.size()) * 100. / pImpl->bstBinaryRecords_.size());
}

UINT32 dna_segment::currentBlock() const { return pImpl->currentBlock_; }
size_t dna_segment::blockCount() const { return pImpl->vISL_.size(); }
double dna_segment::averageblockSize() const { return pImpl->averageBlockSize_; }
UINT32 dna_segment::stationSolutionCount() const { return pImpl->stationSolutionCount_; }
UINT32 dna_segment::maxBlockSize() const { return pImpl->maxBlockSize_; }
UINT32 dna_segment::minBlockSize() const { return pImpl->minBlockSize_; }
_SEGMENT_STATUS_ dna_segment::GetStatus() const { return pImpl->segmentStatus_; }

void dna_segment::LoadNetFile() {
    // Get stations listed in net file
    std::ifstream net_file;
    try {
        // Load net file.  Throws runtime_error on failure.
        file_opener(net_file, pImpl->projectSettings_.s.net_file, std::ios::in, ascii, true);
    } catch (const std::runtime_error& e) { SignalExceptionSerialise(e.what(), 0, NULL); }

    std::string station;
    char line[PROGRAM_OPTIONS_LINE_LENGTH];

    // read header lines
    net_file.ignore(PRINT_LINE_LENGTH, '\n');             // -------------------------------------------
    net_file.getline(line, PROGRAM_OPTIONS_LINE_LENGTH);  //            DYNADJUST BLOCK 1 STATIONS FILE

    pImpl->vinitialStns_.clear();

    try {
        while (!net_file.eof()) {
            net_file.getline(line, PROGRAM_OPTIONS_LINE_LENGTH);  // Stations
            station = line;
            if ((station = trimstr(station)).empty()) continue;
            // add this station to the list
            pImpl->vinitialStns_.push_back(station);
        }
    } catch (const std::ios_base::failure& f) {
        if (!net_file.eof()) {
            std::stringstream ss;
            ss << "ParseStartingStations(): An error was encountered when reading "
               << pImpl->projectSettings_.s.net_file << "." << std::endl
               << f.what();
            SignalExceptionSerialise(ss.str(), 0, "i", &net_file);
        }
    }

    net_file.close();
}

void dna_segment::ParseStartingStations() {
    // Get stations listed in net file (if it exists)
    // Rememeber, calling app should make sure that
    // pImpl->projectSettings_.s.net_file contains a valid path if
    // the user wants to use a net file.
    if (std::filesystem::exists(pImpl->projectSettings_.s.net_file.c_str())) LoadNetFile();

    // OK, now get the additionalstations on the command line
    if (pImpl->projectSettings_.s.seg_starting_stns.empty()) return;

    vstring cmd_line_stations;

    try {
        // Extract stations from comma delimited string
        SplitDelimitedString<std::string>(pImpl->projectSettings_.s.seg_starting_stns,  // the comma delimited string
                                          std::string(","),                             // the delimiter
                                          &cmd_line_stations);                          // the respective values
    } catch (...) { return; }

    if (!cmd_line_stations.empty())
        pImpl->vinitialStns_.insert(pImpl->vinitialStns_.end(), cmd_line_stations.begin(), cmd_line_stations.end());

    if (!pImpl->vinitialStns_.empty()) strip_duplicates(pImpl->vinitialStns_);
}

void dna_segment::PrepareSegmentation(project_settings* p) {
    pImpl->projectSettings_ = *p;

    ParseStartingStations();

    pImpl->network_name_ = p->g.network_name;

    LoadBinaryFiles(p->s.bst_file, p->s.bms_file);
    LoadAssociationFiles(p->s.asl_file, p->s.aml_file);
    LoadStationMap(p->s.map_file);
    BuildFreeStationAvailabilityList();

    SortbyMeasurementCount(&pImpl->vfreeStnList_);
}

void dna_segment::InitialiseSegmentation() {
    pImpl->segmentStatus_ = SEGMENT_SUCCESS;
    pImpl->isProcessing_ = false;
    pImpl->currentBlock_ = 0;

    pImpl->network_name_ = "network_name";  // network name

    pImpl->vinitialStns_.clear();
    pImpl->vCurrJunctStnList_.clear();
    pImpl->vCurrInnerStnList_.clear();
    pImpl->vCurrMeasurementList_.clear();

    pImpl->bstBinaryRecords_.clear();
    pImpl->bmsBinaryRecords_.clear();
    pImpl->vAssocStnList_.clear();
    pImpl->vAssocMsrList_.clear();
    pImpl->vfreeStnList_.clear();
    pImpl->vfreeMsrList_.clear();
    pImpl->stnsMap_.clear();

    pImpl->vJSL_.clear();
    pImpl->vISL_.clear();
    pImpl->vCML_.clear();
}

_SEGMENT_STATUS_ dna_segment::SegmentNetwork(project_settings* p) {
    pImpl->isProcessing_ = true;

    pImpl->output_folder_ = p->g.output_folder;

    if ((pImpl->debug_level_ = p->g.verbose) > 2) {
        std::string s(p->s.seg_file + ".trace");
        try {
            // Create trace file.  Throws runtime_error on failure.
            file_opener(pImpl->trace_file, s);
            print_file_header(pImpl->trace_file, "DYNADJUST SEGMENTATION TRACE FILE");
            pImpl->trace_file << OUTPUTLINE << std::endl << std::endl;
        } catch (const std::runtime_error& e) { SignalExceptionSerialise(e.what(), 0, NULL); }
    }

    if ((pImpl->debug_level_ = p->g.verbose) > 1) {
        std::string s(p->s.seg_file + ".debug");

        try {
            // Create debug file.  Throws runtime_error on failure.
            file_opener(pImpl->debug_file, s);
        } catch (const std::ios_base::failure& f) { SignalExceptionSerialise(f.what(), 0, NULL); }
    }

    pImpl->segmentStatus_ = SEGMENT_SUCCESS;
    std::ostringstream ss;

    if (pImpl->debug_level_ > 2) WriteFreeStnListSortedbyASLMsrCount();

    pImpl->currentBlock_ = 1;

    if (pImpl->debug_level_ > 2) pImpl->trace_file << "Block " << pImpl->currentBlock_ << "..." << std::endl;

    cpu_timer time;

    pImpl->v_ContiguousNetList_.clear();
    pImpl->v_ContiguousNetList_.push_back(pImpl->currentNetwork_ = 0);

    if (pImpl->bstBinaryRecords_.empty())
        SignalExceptionSerialise("SegmentNetwork(): the binary stations file has not been loaded into memory yet.", 0,
                                 NULL);
    if (pImpl->bmsBinaryRecords_.empty())
        SignalExceptionSerialise("SegmentNetwork(): the binary measurements file has not been loaded into memory yet.",
                                 0, NULL);
    if (pImpl->vAssocStnList_.empty())
        SignalExceptionSerialise(
            "SegmentNetwork(): the Associated Station List (ASL) has not been loaded into memory yet.", 0, NULL);
    if (pImpl->vAssocFreeMsrList_.empty())
        SignalExceptionSerialise(
            "SegmentNetwork(): the Associated Measurements List (AML) has not been loaded into memory yet.", 0, NULL);
    if (pImpl->stnsMap_.empty())
        SignalExceptionSerialise("SegmentNetwork(): the station map has not been loaded into memory yet.", 0, NULL);

    // The inner stations for the first block are created from segmentCriteria._initialStns
    // Junction stations are retrieved from measurements connected to the inner stations
    BuildFirstBlock();

    while (!pImpl->vfreeStnList_.empty()) {
        pImpl->isProcessing_ = true;

        pImpl->currentBlock_++;

        if (pImpl->debug_level_ > 2) pImpl->trace_file << "Block " << pImpl->currentBlock_ << "..." << std::endl;

        pImpl->v_ContiguousNetList_.push_back(pImpl->currentNetwork_);

        // The inner stations for the next block are the junction stations from the previous block
        // The junction stations are retrieved from measurements connected to the inner stations
        // BuildNextBlock applies the min and max station constraints using segmentCriteria
        BuildNextBlock();

        if (pImpl->vfreeStnList_.empty()) break;
    }

    boost::posix_time::milliseconds elapsed_time(boost::posix_time::milliseconds(0));
    if (pImpl->debug_level_ > 1)
        elapsed_time = boost::posix_time::milliseconds(
            std::chrono::duration_cast<std::chrono::milliseconds>(time.elapsed().wall).count());

    pImpl->isProcessing_ = false;

    CalculateAverageBlockSize();

    char valid_stations(0);
    char valid_measurements_not_included(0);
    if (pImpl->vfreeStnList_.size()) {
        valid_stations = 1;
        ss.str("");
        ss << std::endl << "- Warning: The following stations were not used:" << std::endl;
        ss << "  ";
        it_vUINT32_const _it_freestn(pImpl->vfreeStnList_.begin());
        for (; _it_freestn != pImpl->vfreeStnList_.end(); ++_it_freestn)
            ss << pImpl->bstBinaryRecords_.at(*_it_freestn).stationName << " ";
        ss << std::endl;
        ss << "- Possible reasons why these stations were not used include:" << std::endl;
        ss << "  - No measurements to these stations were found," << std::endl;
        ss << "  - The network is made up of non-contiguous blocks," << std::endl;
        ss << "  - There is a bug in this program." << std::endl;

        if (pImpl->debug_level_ > 1) pImpl->debug_file << ss.str();
    }

    if (pImpl->debug_level_ > 1) {
        pImpl->debug_file << formatedElapsedTime<std::string>(&elapsed_time, "+ Network segmentation took ")
                          << " The network is now ready for adjustment." << std::endl;
        pImpl->debug_file.close();
    }

    pImpl->isProcessing_ = false;

    if (valid_measurements_not_included || valid_stations) return (pImpl->segmentStatus_ = SEGMENT_BLOCK_ERROR);

    return (pImpl->segmentStatus_ = SEGMENT_SUCCESS);
}

void dna_segment::CalculateAverageBlockSize() {
    vUINT32 blockSizes;
    blockSizes.resize(pImpl->vISL_.size());

    it_vUINT32 _it_count;
    it_vvUINT32 _it_isl, _it_jsl;
    for (_it_count = blockSizes.begin(), _it_isl = pImpl->vISL_.begin(), _it_jsl = pImpl->vJSL_.begin();
         _it_isl != pImpl->vISL_.end(); ++_it_count, ++_it_isl, ++_it_jsl)
        *_it_count = static_cast<UINT32>(_it_isl->size() + _it_jsl->size());

    pImpl->averageBlockSize_ =
        average<double, UINT32, it_vUINT32>(blockSizes.begin(), blockSizes.end(), pImpl->stationSolutionCount_);

    pImpl->maxBlockSize_ = *(max_element(blockSizes.begin(), blockSizes.end()));
    pImpl->minBlockSize_ = *(min_element(blockSizes.begin(), blockSizes.end()));
}

std::string dna_segment::DefaultStartingStation() {
    if (pImpl->vfreeStnList_.empty()) return "";
    if (pImpl->bstBinaryRecords_.empty()) return "";
    return pImpl->bstBinaryRecords_.at(pImpl->vfreeStnList_.at(0)).stationName;
}

std::vector<std::string> dna_segment::StartingStations() { return pImpl->vinitialStns_; }

// throws NetSegmentException on failure
void dna_segment::BuildFirstBlock() {
    pImpl->vCurrJunctStnList_.clear();
    pImpl->vCurrInnerStnList_.clear();
    pImpl->vCurrMeasurementList_.clear();

    if (pImpl->vinitialStns_.empty())
        // no station specified? then pick the first one on the free list
        // Remember, pImpl->vfreeStnList_ is not sorted alphabetically, but according
        // to the number of measurements connected to each station
        pImpl->vinitialStns_.push_back(pImpl->bstBinaryRecords_.at(pImpl->vfreeStnList_.at(0)).stationName);
    else
        RemoveDuplicateStations(&pImpl->vinitialStns_);  // remove duplicates

    // First pass to validate stations
    VerifyStationsandBuildBlock(true);

    // Second pass to build the block
    VerifyStationsandBuildBlock();

    FinaliseBlock();
}

void dna_segment::VerifyStationsandBuildBlock(bool validationOnly) {
#ifdef _MSDEBUG
    UINT32 stn_index;
#endif

    it_vUINT32 _it_freeisl;
    _it_vstr_const _it_name(pImpl->vinitialStns_.begin());
    it_pair_string_vUINT32 it_stnmap_range;
    v_string_uint32_pair::iterator _it_stnmap(pImpl->stnsMap_.begin());

    // For each station name specified as the initial stations
    for (_it_name = pImpl->vinitialStns_.begin(); _it_name != pImpl->vinitialStns_.end(); ++_it_name) {
        // Retrieve the ASL index
        it_stnmap_range =
            equal_range(pImpl->stnsMap_.begin(), pImpl->stnsMap_.end(), *_it_name, StationNameIDCompareName());

        if (it_stnmap_range.first == it_stnmap_range.second) {
            // If this point is reached, _it_stnmap->second is not a known network station
            std::stringstream ss;
            ss << "VerifyStationsandBuildBlock(): " << *_it_name << " is not in the list of network stations.";
            SignalExceptionSerialise(ss.str(), 0, NULL);
        }

        _it_stnmap = it_stnmap_range.first;

#ifdef _MSDEBUG
        stn_index = _it_stnmap->second;
#endif
        // add this station to inner station list if it is on the free stations list only.
        // pImpl->vfreeStnList_ is sorted according to the number of measurements connected to each station
        // hence, a binary_search (or lower_bound or similar) cannot be used here

        // Check if this station is available before trying to find it.  Why?
        // find_if over a large vector is not fast like binary_search
        if (pImpl->vfreeStnAvailability_.at(_it_stnmap->second).isfree()) {
            // Ok, find it and remove it
            // if ((_it_freeisl = find_if(pImpl->vfreeStnList_.begin(), pImpl->vfreeStnList_.end(),
            //	bind1st(std::equal_to<UINT32>(), _it_stnmap->second))) != pImpl->vfreeStnList_.end())
            if ((_it_freeisl = find_if(pImpl->vfreeStnList_.begin(), pImpl->vfreeStnList_.end(),
                                       [&_it_stnmap](const UINT32& freeStn) {
                                           return freeStn == _it_stnmap->second;
                                       })) != pImpl->vfreeStnList_.end()) {
                if (!validationOnly) MoveFreeStnToInnerList(_it_freeisl, _it_stnmap->second);
            }
        } else {
            if (validationOnly) {
                // If this point is reached, _it_stnmap->second is a known network station but is
                // invalid or has no measurements connected to it.
                std::stringstream ss;
                ss << "VerifyStationsandBuildBlock(): " << pImpl->bstBinaryRecords_.at(_it_stnmap->second).stationName
                   << " does not have any measurements connected to it.";
                SignalExceptionSerialise(ss.str(), 0, NULL);
            }
        }

        if (!validationOnly)
            // Retrieve JSL (all stations connected to this station) and
            // add AML indices to CML for all measurements connected to this station
            GetInnerMeasurements(_it_stnmap->second);
    }
}

UINT32 dna_segment::SelectInner() {
    // Sort the list of junction stations by the number of measurements
    // connected to them, and begin with the station that has the lowest
    // number of measurements
    SortbyMeasurementCount(&pImpl->vCurrJunctStnList_);

    it_vUINT32 it_currjsl(pImpl->vCurrJunctStnList_.begin());
    UINT32 stn_index = *it_currjsl;

    if (pImpl->debug_level_ > 2)
        pImpl->trace_file << " + Inner '" << pImpl->bstBinaryRecords_.at(stn_index).stationName << "'" << std::endl;

#ifdef _MSDEBUG
    std::string station_name = pImpl->bstBinaryRecords_.at(stn_index).stationName;
#endif

    // add this junction station to inner station list
    MoveStation(pImpl->vCurrJunctStnList_, it_currjsl, pImpl->vCurrInnerStnList_, stn_index);

    return stn_index;
}

// Get the next free station and push it to the junction list
// This method should only be reached when the junction list is empty,
// but measurements still remain and stations exist in the free list.
void dna_segment::SelectJunction() {
    SortbyMeasurementCount(&pImpl->vfreeStnList_);

    it_vUINT32 it_newjsl(pImpl->vfreeStnList_.begin());

    MoveFreeStnToJunctionList(it_newjsl, *it_newjsl);
}

it_vUINT32 dna_segment::MoveStation(vUINT32& fromList, it_vUINT32 it_from, vUINT32& toList, const UINT32& stn_index) {
    // Move a station from one list to another list
    toList.push_back(stn_index);
    return fromList.erase(it_from);
}

void dna_segment::MoveFreeStn(it_vUINT32 it_freeisl, vUINT32& toList, const UINT32& stn_index,
                              const std::string& type) {
    if (pImpl->debug_level_ > 2)
        pImpl->trace_file << " + New " << type << " station (" << stn_index << ") '"
                          << pImpl->bstBinaryRecords_.at(stn_index).stationName << "'" << std::endl;

#ifdef _MSDEBUG
    std::string station_name = pImpl->bstBinaryRecords_.at(stn_index).stationName;
#endif

    // Mark this station as unavailable
    pImpl->vfreeStnAvailability_.at(stn_index).consume();

    // Move a station from the free station list to the specified list
    // stnList may be either inner or junction list
    MoveStation(pImpl->vfreeStnList_, it_freeisl, toList, stn_index);
}

void dna_segment::MoveFreeStnToJunctionList(it_vUINT32 it_freeisl, const UINT32& stn_index) {
    // Move a station from the free station list to the junction list
    MoveFreeStn(it_freeisl, pImpl->vCurrJunctStnList_, stn_index, "junction");
}

void dna_segment::MoveFreeStnToInnerList(it_vUINT32 it_freeisl, const UINT32& stn_index) {
    // Move a station from the free station list to the junction list
    MoveFreeStn(it_freeisl, pImpl->vCurrInnerStnList_, stn_index, "inner");
}

// throws NetSegmentException on failure
void dna_segment::BuildNextBlock() {
    // pImpl->vCurrJunctStnList_ carries the previous block's junction stations. Thus,
    // don't clear it as these stations will become the inners for the next block
    pImpl->vCurrInnerStnList_.clear();
    pImpl->vCurrMeasurementList_.clear();

#ifdef _MSDEBUG
    std::string station_name;
#endif

    // Select a new junction station if there are none on the JSL
    // that have free measurements left
    if (pImpl->vCurrJunctStnList_.empty() && !pImpl->vfreeStnList_.empty()) {
        if (!pImpl->projectSettings_.s.force_contiguous_blocks)
            pImpl->v_ContiguousNetList_.back() = ++pImpl->currentNetwork_;

        if (pImpl->debug_level_ > 1) {
            pImpl->debug_file << "+ Non-contiguous block found... creating a new block using "
                              << pImpl->bstBinaryRecords_.at(pImpl->vfreeStnList_.at(0)).stationName << std::endl;
            if (pImpl->debug_level_ > 2)
                pImpl->trace_file << " + Non-contiguous block found... creating a new block using "
                                  << pImpl->bstBinaryRecords_.at(pImpl->vfreeStnList_.at(0)).stationName << std::endl;
        }

        // Select a new junction from the free station list
        SelectJunction();
    }

    if (pImpl->vCurrJunctStnList_.empty()) {
        coutSummary();
        SignalExceptionSerialise(
            "BuildNextBlock(): An invalid junction list has been created.  This is most likely a bug.", 0, NULL);
    }

    UINT32 stn_index;
    UINT32 currentTotalSize;
    bool block_threshold_reached = false;

    // Until the threshold is reached...
    //  - Go through the list of junction stations (copied from the previous block) and
    //    make them inner stations if there are new stations connected to them
    //  - If the list is exhausted, add new junctions
    while (!block_threshold_reached) {
        // Attempt to add non-contiguous blocks to this block if the
        // station limit hasn't been reached
        if (pImpl->vfreeStnList_.empty() /*|| pImpl->vfreeMsrList_.empty()*/) break;

        // Is the junction list empty?  force_contiguous_blocks determines what to
        // do in this instance.
        if (pImpl->projectSettings_.s.force_contiguous_blocks) {
            // Add non-contiguous blocks to this block if the
            // station limit hasn't been reached
            if (pImpl->vCurrJunctStnList_.empty()) SelectJunction();
        } else if (pImpl->vCurrJunctStnList_.empty())
            break;

        // Get next station from current junction list
        stn_index = SelectInner();

        // Retrieve JSL (all stations connected to this station) and
        // add AML indices to CML for all measurements connected to this station
        GetInnerMeasurements(stn_index);

        currentTotalSize = static_cast<UINT32>(pImpl->vCurrInnerStnList_.size() + pImpl->vCurrJunctStnList_.size());

        // Has the station count threshold been exceeded?
        // if (currentTotalSize >= seg_max_total_stations_)
        if (currentTotalSize >= pImpl->projectSettings_.s.max_total_stations) {
            if (pImpl->vCurrInnerStnList_.size() < pImpl->projectSettings_.s.min_inner_stations) continue;
            block_threshold_reached = true;
            break;
        }
    }

    if (pImpl->debug_level_ > 2) {
        if (block_threshold_reached)
            pImpl->trace_file << " + Block size threshold exceeded... finishing block." << std::endl;
        else
            pImpl->trace_file << " + Block " << pImpl->currentBlock_ << " complete." << std::endl;
    }

    FinaliseBlock();
}

void dna_segment::FinaliseBlock() {
    FindCommonMeasurements();
    MoveJunctiontoISL();

    // Sort lists
    std::sort(pImpl->vCurrInnerStnList_.begin(), pImpl->vCurrInnerStnList_.end());
    std::sort(pImpl->vCurrJunctStnList_.begin(), pImpl->vCurrJunctStnList_.end());
    strip_duplicates(pImpl->vCurrMeasurementList_);  // remove duplicates and sort

    pImpl->vISL_.push_back(pImpl->vCurrInnerStnList_);
    pImpl->vJSL_.push_back(pImpl->vCurrJunctStnList_);
    pImpl->vCML_.push_back(pImpl->vCurrMeasurementList_);

    if (pImpl->debug_level_ > 1) {
        coutCurrentBlockSummary(pImpl->debug_file);

        if (pImpl->debug_level_ > 2) {
            UINT32 currentISLSize, currentJSLSize, currentTotalSize, currentMsrSize;
            currentISLSize = static_cast<UINT32>(pImpl->vCurrInnerStnList_.size());
            currentJSLSize = static_cast<UINT32>(pImpl->vCurrJunctStnList_.size());
            currentTotalSize = currentISLSize + currentJSLSize;
            currentMsrSize = static_cast<UINT32>(pImpl->vCurrMeasurementList_.size());

            pImpl->trace_file << " + Done." << std::endl;
            pImpl->trace_file << " + BLOCK " << pImpl->currentBlock_ << " SUMMARY:" << std::endl;
            pImpl->trace_file << "   - " << currentISLSize << " inner stations, " << currentJSLSize
                              << " junction stations, ";
            pImpl->trace_file << currentTotalSize << " total stations, " << currentMsrSize << " measurements."
                              << std::endl;

            pImpl->trace_file << "   - Inners (" << currentISLSize << "):" << std::endl << "     ";
            it_vUINT32_const _it_freeisl(pImpl->vfreeStnList_.end());
            for (_it_freeisl = pImpl->vCurrInnerStnList_.begin(); _it_freeisl != pImpl->vCurrInnerStnList_.end();
                 ++_it_freeisl)
                pImpl->trace_file << "'" << pImpl->bstBinaryRecords_.at(*_it_freeisl).stationName << "' ";
            pImpl->trace_file << std::endl;

            pImpl->trace_file << "   - Junctions (" << currentJSLSize << "):" << std::endl << "     ";
            for (_it_freeisl = pImpl->vCurrJunctStnList_.begin(); _it_freeisl != pImpl->vCurrJunctStnList_.end();
                 ++_it_freeisl)
                pImpl->trace_file << "'" << pImpl->bstBinaryRecords_.at(*_it_freeisl).stationName << "' ";

            pImpl->trace_file << std::endl << " + ------------------------------------------" << std::endl;
        }
    }
}

// bool dna_segment::IncrementNextAvailableAMLIndex(UINT32& amlIndex, const UINT32& lastamlIndex)
//{
//	// Already at the last measurement?
//	if (amlIndex > lastamlIndex)
//		return false;
//
//	while (!pImpl->vAssocFreeMsrList_.at(amlIndex).available)
//	{
//		// Already at the last measurement?
//		if (amlIndex == lastamlIndex)
//			return false;
//
//		// Get the next measurement record tied
//		// to this measurement
//		++amlIndex;
//	}
//	return true;
// }
//
//
// bool dna_segment::IncrementNextAvailableAMLIndex(it_aml_pair& _it_aml, const it_aml_pair& _it_lastaml)
//{
//	// Already at the last measurement?
//	if (_it_aml > _it_lastaml)
//		return false;
//
//	while (!_it_aml->available)
//	{
//		// Already at the last measurement?
//		if (_it_aml == _it_lastaml)
//			return false;
//
//		// Get the next measurement record tied
//		// to this measurement
//		++_it_aml;
//	}
//	return true;
// }

// Name:				GetInnerMeasurements
// Purpose:				Gets all measurements tied to the subject station (innerStation) and stations tied to
//                      those measurements
// Called by:			BuildFirstBlock(), BuildNextBlock()
// Calls:				AddtoCurrentMsrList(), AddtoJunctionStnList()
void dna_segment::GetInnerMeasurements(const UINT32& innerStation) {
    if (pImpl->debug_level_ > 2) pImpl->trace_file << " + GetInnerMeasurements()" << std::endl;

#ifdef _MSDEBUG
    std::string from, to, to2;
#endif

    measurement_t measRecord;
    vUINT32 msrStations;

    // Get the original measurement count
    UINT32 m, msrCount(pImpl->vASLCount_.at(innerStation));
    // Get the aml index, initialised to the last measurement for this station
    UINT32 amlIndex(pImpl->vAssocStnList_.at(innerStation)
                        .GetAMLStnIndex()  // beginning of the measurements associated with this station
                    + msrCount - 1);       // the number of associated measurements

    for (m = 0; m < msrCount; ++m, --amlIndex) {
        // is this measurement ignored or already used?
        if (!pImpl->vAssocFreeMsrList_.at(amlIndex).available) continue;

        measRecord = pImpl->bmsBinaryRecords_.at(pImpl->vAssocFreeMsrList_.at(amlIndex).bmsr_index);

        if (measRecord.ignore) continue;

#ifdef _MSDEBUG
        from = pImpl->bstBinaryRecords_.at(measRecord.station1).stationName;
        if (MsrTally::Stations(measRecord.measType) >= TWO_STATION) {
            to = pImpl->bstBinaryRecords_.at(measRecord.station2).stationName;
            if (MsrTally::Stations(measRecord.measType) == THREE_STATION)
                to2 = pImpl->bstBinaryRecords_.at(measRecord.station3).stationName;
        }
#endif

        // When a non-measurement element is found (i.e. Y or Z or covariance component),
        // continue to next element.  Hence, only pointers to the starting element of each
        // measurement are kept. See declaration of measRecord.measStart for more info.
        switch (measRecord.measType) {
        case 'G':
        case 'X':
        case 'Y':
            if (measRecord.measStart != xMeas) continue;
        }

        // Get all the stations associated with this measurement
        GetMsrStations(pImpl->bmsBinaryRecords_, pImpl->vAssocFreeMsrList_.at(amlIndex).bmsr_index, msrStations);

        // Add this measurement to the current measurement list
        AddtoCurrentMsrList(amlIndex, msrStations);

        // if this station is not on JSL, move it from the freestnlist to JSL
        AddtoJunctionStnList(msrStations);
    }
}

// If this measurement is "available", then:
//	- consume this measurement and all other occurences of it on the AML
//	- add this measurement to the current list of measurements for this block.
bool dna_segment::AddtoCurrentMsrList(const UINT32& amlIndex, const vUINT32& msrStations) {
    if (!pImpl->vAssocFreeMsrList_.at(amlIndex).available) return false;

    UINT32 m, msrCount, bmsrindex(pImpl->vAssocFreeMsrList_.at(amlIndex).bmsr_index);

#ifdef _MSDEBUG
    measurement_t msr(pImpl->bmsBinaryRecords_.at(bmsrindex));
    char measType(msr.measType);
    std::string station;
    UINT32 stn_index;
#endif

    UINT32 firstIndex(GetFirstMsrIndex<UINT32>(pImpl->bmsBinaryRecords_, bmsrindex));

    // Add the index of the first binary measurement record to the measurement list
    pImpl->vCurrMeasurementList_.push_back(firstIndex);

    if (pImpl->debug_level_ > 2) {
        pImpl->trace_file << "   - Measurement '" << pImpl->bmsBinaryRecords_.at(bmsrindex).measType << "' ";
        if (pImpl->bmsBinaryRecords_.at(bmsrindex).ignore) pImpl->trace_file << "(ignored) ";
        pImpl->trace_file << "associated with station(s): ";
    }

    it_aml_pair _it_aml;
    it_vUINT32_const _it_stn, _it_msr;

    // 1) consume each occurrence of this measurement (and all other
    //	  measurements within a cluster) on the AML, then
    // 2) Decrement the measurement count for all stations associated
    //	  with this measurement.
    for (_it_stn = msrStations.begin(); _it_stn != msrStations.end(); ++_it_stn) {
        // Consume the occurrences of this measurement (and other measurements in a cluster)
        // in the aml for the stations in msrStations

#ifdef _MSDEBUG
        stn_index = *_it_stn;
        station = pImpl->bstBinaryRecords_.at(*_it_stn).stationName;
#endif

        if (pImpl->debug_level_ > 2) pImpl->trace_file << pImpl->bstBinaryRecords_.at(*_it_stn).stationName << " ";

        // set _it_aml to point to the first aml entry for station *_it_stn
        _it_aml = pImpl->vAssocFreeMsrList_.begin() + pImpl->vAssocStnList_.at(*_it_stn).GetAMLStnIndex();
        msrCount = pImpl->vASLCount_.at(*_it_stn);

        // consume measurement and decrement measurement count
        for (m = 0; m < msrCount; ++m, ++_it_aml) {
            // is this measurement ignored or already used?
            if (!_it_aml->available) continue;

            if (_it_aml->bmsr_index == bmsrindex) {
                // Consume the measurement
                _it_aml->consume();

                // Decrement the measurement count.
                pImpl->vAssocStnList_.at(*_it_stn).DecrementMsrCount();
                break;
            }
        }
    }

    if (pImpl->debug_level_ > 2) pImpl->trace_file << std::endl;

    return true;
}

// Name:				FindCommonMeasurements
// Purpose:				Gets all measurements tied only to junction stations.
//						Measurements between two inners, and between an inner
//                      and a junction are captured by GetInnerMeasurements()
// Called by:			BuildFirstBlock(), BuildNextBlock()
void dna_segment::FindCommonMeasurements() {
    // Create a temporary vector
    vUINT32 vCurrBlockStnList(pImpl->vCurrJunctStnList_), msrStations;

    std::sort(vCurrBlockStnList.begin(), vCurrBlockStnList.end());

    it_vUINT32 _it_stn, _it_mstn;
    measurement_t measRecord;
    bool inList;

    if (pImpl->debug_level_ > 2) pImpl->trace_file << " + FindCommonMeasurements()" << std::endl;

#ifdef _MSDEBUG
    std::string stn;
#endif

    UINT32 m, msrCount, amlIndex;

    for (_it_stn = vCurrBlockStnList.begin(); _it_stn != vCurrBlockStnList.end(); ++_it_stn) {
        // Get the original measurement count
        msrCount = pImpl->vASLCount_.at(*_it_stn);
        amlIndex = pImpl->vAssocStnList_.at(*_it_stn).GetAMLStnIndex();

#ifdef _MSDEBUG
        stn = pImpl->bstBinaryRecords_.at(*_it_stn).stationName;
#endif

        for (m = 0; m < msrCount; ++m, ++amlIndex) {
            // is this measurement ignored or already used?
            if (!pImpl->vAssocFreeMsrList_.at(amlIndex).available) continue;

            measRecord = pImpl->bmsBinaryRecords_.at(pImpl->vAssocFreeMsrList_.at(amlIndex).bmsr_index);

            if (measRecord.ignore) continue;

            // Get all the stations associated with this measurement
            GetMsrStations(pImpl->bmsBinaryRecords_, pImpl->vAssocFreeMsrList_.at(amlIndex).bmsr_index, msrStations);

            // Since a single station measurement is not associated with other stations,
            // add the measurement to the list
            if (msrStations.size() == 1) {
                // Add this measurement to the current measurement list
                AddtoCurrentMsrList(amlIndex, msrStations);
                continue;
            }

            // Assume this measurement is common to the stations on vCurrBlockStnList.
            // inList will be set to false if a measurement station is not found on
            // the list
            inList = true;

            // Two or more station measurements
            for (_it_mstn = msrStations.begin(); _it_mstn != msrStations.end(); ++_it_mstn) {
                // If one station in msrStations (which are all stations
                // associated with the current measurement) is not in the junction list,
                // then this cannot be a "common measurement".
                if (!binary_search(vCurrBlockStnList.begin(), vCurrBlockStnList.end(), *_it_mstn)) {
                    // The station at _it_mstn is not in vCurrBlockStnList, so this is not a
                    // "common measurement" - break out of loop.
                    inList = false;
                    break;
                }
            }

            // If all stations in msrStations are found within vCurrBlockStnList, then
            // add this measurement to the current measurement list
            if (inList) AddtoCurrentMsrList(amlIndex, msrStations);
        }
    }
}

UINT32 dna_segment::GetAvailableMsrCount(const UINT32& stn_index) {
    return pImpl->vAssocStnList_.at(stn_index).GetAvailMsrCount();
}

// Name:				MoveJunctiontoISL
// Purpose:				Moves stations on the inner list to the junction list.  The
//						condition for moving a station is that there are no more
//						available measurements connected to the junction station.
// Called by:			BuildFirstBlock(), BuildNextBlock()
// Calls:
void dna_segment::MoveJunctiontoISL() {
    // Move stations on the current junction station list to the inner station list if
    // there are no more measurements tied to this station
    if (pImpl->debug_level_ > 2) pImpl->trace_file << " + MoveJunctiontoISL()" << std::endl;

#ifdef _MSDEBUG
    std::string station_name;
#endif

    it_vUINT32 _it_currjsl(pImpl->vCurrJunctStnList_.begin());
    UINT32 stn_index;

    while (!pImpl->vCurrJunctStnList_.empty()) {
        stn_index = *_it_currjsl;

#ifdef _MSDEBUG
        station_name = pImpl->bstBinaryRecords_.at(stn_index).stationName;
#endif

        // Are there no more available measurements connected to this station?
        if (GetAvailableMsrCount(stn_index) == 0) {
            if (pImpl->debug_level_ > 2)
                pImpl->trace_file << "   - Junction station '"
                                  << pImpl->bstBinaryRecords_.at(static_cast<UINT32>(stn_index)).stationName
                                  << "' was moved to inner list (no more available measurements)." << std::endl;

            // Move this station from the junctions list to inners list
            _it_currjsl = MoveStation(pImpl->vCurrJunctStnList_, _it_currjsl, pImpl->vCurrInnerStnList_, stn_index);

            // Have all JSLs been removed?
            if (pImpl->vCurrJunctStnList_.empty())
                // end the while loop
                break;
            else if (_it_currjsl == pImpl->vCurrJunctStnList_.end())
                // reset the iterator to the previous junction station
                --_it_currjsl;

            continue;
        }

        // move to the next junction, unless the iterator is
        // pointing to the last
        if (++_it_currjsl == pImpl->vCurrJunctStnList_.end()) break;
    }
}

void dna_segment::AddtoJunctionStnList(const vUINT32& msrStations) {
    it_vUINT32_const _it_stn;
    it_vUINT32 _it_freeisl;
    for (_it_stn = msrStations.begin(); _it_stn != msrStations.end(); ++_it_stn) {
        // If First station is not already in the junction list...
        // if (find_if(pImpl->vCurrJunctStnList_.begin(), pImpl->vCurrJunctStnList_.end(),
        //	bind1st(equal_to<UINT32>(), *_it_stn)) == pImpl->vCurrJunctStnList_.end())
        if (find_if(pImpl->vCurrJunctStnList_.begin(), pImpl->vCurrJunctStnList_.end(),
                    [&_it_stn](const UINT32& junctionStn) { return junctionStn == *_it_stn; }) ==
            pImpl->vCurrJunctStnList_.end()) {
            // If the first station is available, get an iterator to it and move it
            if (pImpl->vfreeStnAvailability_.at(*_it_stn).isfree()) {
                // Move it to the list of junctions
                // if ((_it_freeisl = find_if(pImpl->vfreeStnList_.begin(), pImpl->vfreeStnList_.end(),
                //	bind1st(equal_to<UINT32>(), *_it_stn))) != pImpl->vfreeStnList_.end())
                if ((_it_freeisl = find_if(pImpl->vfreeStnList_.begin(), pImpl->vfreeStnList_.end(),
                                           [&_it_stn](const UINT32& freeStn) { return freeStn == *_it_stn; })) !=
                    pImpl->vfreeStnList_.end()) {
                    MoveFreeStnToJunctionList(_it_freeisl, *_it_stn);
                }
            }
        }
    }
}

// Name:				SignalExceptionSerialise
// Purpose:				Closes all files (if file pointers are passed in) and throws NetSegmentException
// Called by:			Any
// Calls:				NetSegmentException()
void dna_segment::SignalExceptionSerialise(const std::string& msg, const int& i, const char* streamType, ...) {
    pImpl->isProcessing_ = false;
    pImpl->segmentStatus_ = SEGMENT_EXCEPTION_RAISED;

    if (pImpl->debug_level_ > 1)
        if (pImpl->debug_file.is_open()) pImpl->debug_file.close();

    if (pImpl->debug_level_ > 2)
        if (pImpl->trace_file.is_open()) pImpl->trace_file.close();

    if (streamType == NULL) throw NetSegmentException(msg, i);

    std::ofstream* ofsDynaML;
    std::ifstream* ifsbinaryFile;

    va_list argptr;
    va_start(argptr, streamType);

    while (*streamType != '\0') {
        // ifstream
        if (*streamType == 'i') {
            ifsbinaryFile = va_arg(argptr, std::ifstream*);
            ifsbinaryFile->close();
        }
        // ofstream
        if (*streamType == 'o') {
            ofsDynaML = va_arg(argptr, std::ofstream*);
            ofsDynaML->close();
        }
        streamType++;
    }

    va_end(argptr);

    throw NetSegmentException(msg, i);
}

void dna_segment::LoadAssociationFiles(const std::string& aslfileName, const std::string& amlfileName) {
    UINT32 stn, stnCount(0);

    try {
        AslFile asl(aslfileName);
        auto result = asl.Load();
        pImpl->vAssocStnList_ = std::move(result.stations);
        pImpl->vfreeStnList_ = std::move(result.free_stations);
        stnCount = static_cast<UINT32>(result.count);
    } catch (const std::runtime_error& e) { SignalExceptionSerialise(e.what(), 0, NULL); }

    // Dertmine original measurement count for each station
    pImpl->vASLCount_.clear();
    pImpl->vASLCount_.resize(stnCount);

    _it_vasl _it_asl;
    for (stn = 0, _it_asl = pImpl->vAssocStnList_.begin(); _it_asl != pImpl->vAssocStnList_.end(); ++_it_asl, ++stn) {
        pImpl->vASLCount_.at(stn) = _it_asl->GetAssocMsrCount();  // original measurement count
    }

    // remove stations from the free list that are invalid
    RemoveInvalidFreeStations();

    // Load associated measurements list.  Throws runtime_error on failure.
    try {
        AmlFile aml;
        aml.load_aml_file(amlfileName, &pImpl->vAssocFreeMsrList_, &pImpl->bmsBinaryRecords_);
    } catch (const std::runtime_error& e) { SignalExceptionSerialise(e.what(), 0, NULL); }

    // set available msr count
    SetAvailableMsrCount();
}

void dna_segment::LoadStationMap(const std::string& stnmap_file) {
    try {
        dynadjust::iostreams::MapFile map;
        map.LoadFile(stnmap_file, &pImpl->stnsMap_);
    } catch (const std::runtime_error& e) { SignalExceptionSerialise(e.what(), 0, NULL); }
}

void dna_segment::LoadBinaryFiles(const std::string& bstrfileName, const std::string& bmsrfileName) {
    binary_file_meta_t bst_meta_, bms_meta_;
    try {
        // Load binary stations data.  Throws runtime_error on failure.
        BstFile bst;
        bst.LoadFile(bstrfileName, &pImpl->bstBinaryRecords_, bst_meta_);

        // Load binary measurements data.  Throws runtime_error on failure.
        BmsFile bms;
        bms.LoadFile(bmsrfileName, &pImpl->bmsBinaryRecords_, bms_meta_);
    } catch (const std::runtime_error& e) { SignalExceptionSerialise(e.what(), 0, NULL); }

    // Update the Cluster comparison functor with the AML binary vector
    pImpl->clusteridCompareFunc_.SetAMLPointer(const_cast<pvmsr_t>(&pImpl->bmsBinaryRecords_));

    // count the number of measurements not ignored
    MsrTally msrTally;
    UINT32 m = msrTally.CreateTally(pImpl->bmsBinaryRecords_, true);

    // throw an exception if no measurements are available
    if (m == 0) {
        std::stringstream ss;
        ss << "No measurements were found." << std::endl
           << "  If measurements were successfully loaded on import, ensure that\n  all measurements have not been "
              "ignored."
           << std::endl;
        SignalExceptionSerialise(ss.str(), 0, NULL);
    }
}

void dna_segment::IdentifyInnerMsrsandAssociatedStns(const UINT32& innerStation, vUINT32& totalStations) {
    it_vmsr_t measRecord;
    vUINT32 msrStations;

    // Get the original measurement count
    UINT32 m, msrCount(pImpl->vASLCount_.at(innerStation));
    // Get the aml index, initialised to the last measurement for this station
    UINT32 amlIndex(pImpl->vAssocStnList_.at(innerStation)
                        .GetAMLStnIndex()  // beginning of the measurements associated with this station
                    + msrCount - 1);       // the number of associated measurements

    for (m = 0; m < msrCount; ++m, --amlIndex) {
        // is this measurement ignored or already used?
        if (!pImpl->vAssocFreeMsrList_.at(amlIndex).available) continue;

        measRecord = pImpl->bmsBinaryRecords_.begin() + pImpl->vAssocFreeMsrList_.at(amlIndex).bmsr_index;

        if (measRecord->ignore) continue;

        // When a non-measurement element is found (i.e. Y or Z or covariance component),
        // continue to next element.  Hence, only pointers to the starting element of each
        // measurement are kept. See declaration of measRecord.measStart for more info.
        switch (measRecord->measType) {
        case 'G':
        case 'X':
        case 'Y':
            if (measRecord->measStart != xMeas) continue;
        }

        // Get all the stations associated with this measurement
        GetMsrStations(pImpl->bmsBinaryRecords_, pImpl->vAssocFreeMsrList_.at(amlIndex).bmsr_index, msrStations);

        totalStations.insert(totalStations.end(), msrStations.begin(), msrStations.end());
    }

    strip_duplicates(totalStations);
}

void dna_segment::IdentifyLowestStationAssociation(pvUINT32 vStnList, vUINT32& totalStations, const int currentLevel,
                                                   const int maxLevel, pvUINT32 vStnCount) {
    it_vUINT32 it_stn;
    vUINT32 msrStations, subStations;

    // if (currentLevel == 0)
    //{
    //	TRACE("Stations to examine:\n");
    //	for_each(vStnList->begin(), vStnList->end(),
    //		[this] (UINT32& stn) {
    //			TRACE("%d ", stn);
    //	});
    //	TRACE("\n");
    // }

    for (it_stn = vStnList->begin(); it_stn != vStnList->end(); ++it_stn) {
        if (currentLevel == 0) vStnCount->push_back(0);

        // TRACE("Level %d  Station %d: ", currentLevel, *it_stn);
        IdentifyInnerMsrsandAssociatedStns(*it_stn, msrStations);

        // for_each(msrStations.begin(), msrStations.end(),
        //	[this] (UINT32& stn) {
        //		TRACE("%d ", stn);
        // });
        // TRACE("\n  ");

        if (currentLevel < maxLevel)
            IdentifyLowestStationAssociation(&msrStations, subStations, currentLevel + 1, maxLevel, vStnCount);

        subStations.insert(subStations.end(), msrStations.begin(), msrStations.end());

        if (currentLevel == 0) {
            // get total stations associated with this station
            strip_duplicates(subStations);
            vStnCount->back() = static_cast<UINT32>(subStations.size());
        }
    }

    if (currentLevel == 0) return;

    totalStations.insert(totalStations.end(), subStations.begin(), subStations.end());
    strip_duplicates(totalStations);

    // TRACE("Summary (%d stations): ", totalStations.size());
    // for_each(totalStations.begin(), totalStations.end(),
    //	[this] (UINT32& stn) {
    //		TRACE("%d ", stn);
    // });
    // TRACE("\n\n");
}

void dna_segment::SortbyMeasurementCount(pvUINT32 vStnList) {
    if (vStnList->size() < 2) return;
    // sort vStnList by number of measurements to each station (held by vAssocStnList
    CompareMeasCount<CAStationList, UINT32> msrcountCompareFunc(&pImpl->vAssocStnList_);
    std::sort(vStnList->begin(), vStnList->end(), msrcountCompareFunc);

    // Search lower level
    if (pImpl->projectSettings_.s.seg_search_level == 0) return;

    vUINT32 msrStations;
    vUINT32 stnList, stnCount;
    stnList.insert(stnList.end(), vStnList->begin(),
                   (vStnList->begin() + minVal(vUINT32::size_type(5), vStnList->size())));
    IdentifyLowestStationAssociation(&stnList, msrStations, 0, pImpl->projectSettings_.s.seg_search_level, &stnCount);

    UINT32 lowestStnCount(*(min_element(stnCount.begin(), stnCount.end())));

    if (stnCount.front() == lowestStnCount) return;

    it_vUINT32 it_stn(vStnList->begin()), it_count;
    for (it_count = stnCount.begin(); it_count != stnCount.end(); ++it_stn, ++it_count) {
        if (lowestStnCount == *it_count) break;
    }

    if (vStnList->size() < 2) return;

    lowestStnCount = *it_stn;
    vStnList->erase(it_stn);
    vStnList->insert(vStnList->begin(), lowestStnCount);
}

void dna_segment::SetAvailableMsrCount() {
    _it_vasl _it_asl;

    UINT32 stn_index, msrCount, amlIndex;

    for (_it_asl = pImpl->vAssocStnList_.begin(); _it_asl != pImpl->vAssocStnList_.end(); ++_it_asl) {
        stn_index = static_cast<UINT32>(std::distance(pImpl->vAssocStnList_.begin(), _it_asl));

        _it_asl->SetAvailMsrCount(_it_asl->GetAssocMsrCount());

        if (_it_asl->GetAssocMsrCount() == 0) continue;

        msrCount = pImpl->vASLCount_.at(stn_index);  // original measurement count
        amlIndex = pImpl->vAssocStnList_.at(stn_index).GetAMLStnIndex();

        // if a measurement is ignored, it will not be available
        for (UINT32 m(0); m < msrCount; ++m, ++amlIndex) {
            if (!pImpl->vAssocFreeMsrList_.at(amlIndex).available) _it_asl->DecrementAvailMsrCount();
        }
    }
}

void dna_segment::RemoveInvalidFreeStations() {
    CompareValidity<CAStationList, UINT32> aslValidityCompareFunc(&pImpl->vAssocStnList_, FALSE);
    std::sort(pImpl->vfreeStnList_.begin(), pImpl->vfreeStnList_.end(), aslValidityCompareFunc);
    erase_if(pImpl->vfreeStnList_, aslValidityCompareFunc);
}

void dna_segment::BuildFreeStationAvailabilityList() {
    SegFile seg;
    seg.BuildFreeStnAvailability(pImpl->vAssocStnList_, pImpl->vfreeStnAvailability_);
}

void dna_segment::RemoveDuplicateStations(pvstring vStations) {
    if (vStations->size() < 2) return;

    // A prior sort on name is essential, since the criteria for removing duplicates
    // is based upon two successive station entries in an ordered vector being equal
    strip_duplicates(vStations);
}

// void dna_segment::RemoveNonMeasurements()
//{
//	if (pImpl->vfreeMsrList_.size() < 2)
//		return;
//	CompareNonMeasStart<measurement_t, UINT32> measstartCompareFunc(&pImpl->bmsBinaryRecords_, xMeas);
//	std::sort(pImpl->vfreeMsrList_.begin(), pImpl->vfreeMsrList_.end(), measstartCompareFunc);
//	erase_if(pImpl->vfreeMsrList_, measstartCompareFunc);
//
// }

// void dna_segment::RemoveIgnoredMeasurements()
//{
//	if (pImpl->vfreeMsrList_.size() < 2)
//		return;
//	CompareIgnoreedMeas<measurement_t, UINT32> ignoremeasCompareFunc(&pImpl->bmsBinaryRecords_);
//	std::sort(pImpl->vfreeMsrList_.begin(), pImpl->vfreeMsrList_.end(), ignoremeasCompareFunc);
//	erase_if(pImpl->vfreeMsrList_, ignoremeasCompareFunc);
//
// }

void dna_segment::WriteFreeStnListSortedbyASLMsrCount() {
    if (!std::filesystem::exists(pImpl->output_folder_)) {
        std::stringstream ss("WriteFreeStnListSortedbyASLMsrCount(): Path does not exist... \n\n    ");
        ss << pImpl->output_folder_ << ".";
        SignalExceptionSerialise(ss.str(), 0, NULL);
    }

    std::string file(pImpl->output_folder_ + "/free_stn_sorted_by_msr_count.lst");
    std::ofstream freestnlist(file.c_str());
    UINT32 x(0);
    std::string s;
    UINT32 u, msrCount, m, amlindex;
    for (; x < pImpl->vfreeStnList_.size(); ++x) {
        u = pImpl->vfreeStnList_.at(x);
        s = pImpl->bstBinaryRecords_.at(pImpl->vfreeStnList_.at(x)).stationName;
        s.insert(0, "'");
        s += "'";
        msrCount = pImpl->vAssocStnList_.at(pImpl->vfreeStnList_.at(x)).GetAssocMsrCount();
        freestnlist << std::left << std::setw(10) << u << std::left << std::setw(14) << s << std::left << std::setw(5)
                    << msrCount;

        for (m = 0; m < msrCount; m++) {
            // get the measurement record (holds other stations tied to this measurement)
            amlindex = pImpl->vAssocStnList_.at(pImpl->vfreeStnList_.at(x)).GetAMLStnIndex() + m;  // get the AML index
            freestnlist << std::left << std::setw(HEADER_20) << amlindex;
        }
        freestnlist << std::endl;
    }
    freestnlist.close();
}

void dna_segment::coutCurrentBlockSummary(std::ostream& os) {
    try {
        SegFile seg;
        seg.WriteSegBlock(os, pImpl->vCurrInnerStnList_, pImpl->vCurrJunctStnList_, pImpl->vCurrMeasurementList_,
                          pImpl->currentBlock_, &pImpl->bstBinaryRecords_, &pImpl->bmsBinaryRecords_, true);
    } catch (const std::runtime_error& e) { SignalExceptionSerialise(e.what(), 0, NULL); }

    if (!pImpl->vfreeStnList_.empty())
        os << "+ Free stations remaining:     " << std::setw(10) << std::right << pImpl->vfreeStnList_.size()
           << std::endl;
    if (!pImpl->vfreeMsrList_.empty())
        os << "+ Free measurements remaining: " << std::setw(10) << std::right << pImpl->vfreeMsrList_.size()
           << std::endl;
}

void dna_segment::coutSummary() const {
    UINT32 stns(0), msrs(0);
    vvUINT32::const_iterator _it_isl, _it_jsl, _it_cml;

    char BLOCK = 10;
    char INNER = 12;
    char JUNCT = 15;
    char TOTAL = 12;
    char MEASR = 14;

    for (_it_cml = pImpl->vCML_.begin(); _it_cml != pImpl->vCML_.end(); ++_it_cml)
        msrs += static_cast<UINT32>(_it_cml->size());
    for (_it_isl = pImpl->vISL_.begin(); _it_isl != pImpl->vISL_.end(); ++_it_isl)
        stns += static_cast<UINT32>(_it_isl->size());

    std::cout << "+ Segmentation summary:" << std::endl << std::endl;
    std::cout << std::setw(BLOCK) << std::left << "  Block" << std::setw(JUNCT) << std::left << "Junction stns"
              << std::setw(INNER) << std::left << "Inner stns" << std::setw(MEASR) << std::left << "Measurements"
              << std::setw(TOTAL) << std::left << "Total stns" << std::endl;
    std::cout << "  ";
    for (char dash = BLOCK + NETID + INNER + JUNCT + TOTAL + MEASR; dash > 2; dash--) std::cout << "-";
    std::cout << std::endl;
    UINT32 b = 1;
    _it_jsl = pImpl->vJSL_.begin();
    _it_cml = pImpl->vCML_.begin();
    for (_it_isl = pImpl->vISL_.begin(); _it_isl != pImpl->vISL_.end(); ++_it_isl) {
        // block
        std::cout << "  " << std::setw(BLOCK - 2) << std::left << b;

        // junction stns
        if (_it_jsl != pImpl->vJSL_.end())
            std::cout << std::setw(JUNCT) << std::left << _it_jsl->size();
        else
            std::cout << std::setw(JUNCT) << " ";

        // inner stns
        std::cout << std::setw(INNER) << std::left << _it_isl->size();

        // total measurements
        if (_it_cml != pImpl->vCML_.end())
            std::cout << std::setw(MEASR) << std::left << _it_cml->size();
        else
            std::cout << std::setw(MEASR) << " ";

        // total stns
        if (_it_jsl != pImpl->vJSL_.end())
            std::cout << std::setw(TOTAL) << std::left << (_it_isl->size() + _it_jsl->size());
        else
            std::cout << std::setw(TOTAL) << std::left << _it_isl->size();

        std::cout << std::endl;

        ++_it_jsl;
        ++_it_cml;
        b++;
    }

    std::cout << std::endl;

    std::cout << "+ Stations used:       " << std::setw(10) << std::right << stns << std::endl;
    std::cout << "+ Measurements used:   " << std::setw(10) << std::right << msrs << std::endl;
    if (!pImpl->vfreeStnList_.empty())
        std::cout << "+ Unused stations:     " << std::setw(10) << std::right << pImpl->vfreeStnList_.size()
                  << std::endl;
    // if (!pImpl->vfreeMsrList_.empty())
    //	std::cout << "+ Unused measurements: " << std::setw(10) << std::right << pImpl->vfreeMsrList_.size() << std::endl;
}

void dna_segment::WriteSegmentedNetwork(const std::string& segfileName) {
    if (pImpl->bstBinaryRecords_.empty())
        SignalExceptionSerialise(
            "WriteSegmentedNetwork(): the binary stations file has not been loaded into memory yet.", 0, NULL);
    if (pImpl->bmsBinaryRecords_.empty())
        SignalExceptionSerialise(
            "WriteSegmentedNetwork(): the binary measurements file has not been loaded into memory yet.", 0, NULL);
    if (pImpl->vJSL_.empty())
        SignalExceptionSerialise(
            "WriteSegmentedNetwork(): the junction stations list is empty, most likely because the network has not "
            "been segmented yet.",
            0, NULL);
    if (pImpl->vISL_.empty())
        SignalExceptionSerialise(
            "WriteSegmentedNetwork(): the inner stations list is empty, most likely because the network has not been "
            "segmented yet.",
            0, NULL);
    if (pImpl->vCML_.empty())
        SignalExceptionSerialise(
            "WriteSegmentedNetwork(): the block measurementslist is empty, most likely because the network has not "
            "been segmented yet.",
            0, NULL);

    try {
        SegFile seg;
        seg.WriteSegFile(segfileName, pImpl->projectSettings_.s.bst_file, pImpl->projectSettings_.s.bms_file,
                         pImpl->projectSettings_.s.min_inner_stations, pImpl->projectSettings_.s.max_total_stations,
                         pImpl->projectSettings_.s.seg_starting_stns, pImpl->vinitialStns_,
                         pImpl->projectSettings_.s.command_line_arguments, pImpl->vISL_, pImpl->vJSL_, pImpl->vCML_,
                         pImpl->v_ContiguousNetList_, &pImpl->bstBinaryRecords_, &pImpl->bmsBinaryRecords_);
    } catch (const std::runtime_error& e) { SignalExceptionSerialise(e.what(), 0, NULL); } catch (...) {
        SignalExceptionSerialise("WriteSegmentedNetwork(): failed to print segmentation file.", 0, NULL);
    }
}

void dna_segment::VerifyStationConnections() {
    UINT32 block, blockCount(static_cast<UINT32>(pImpl->vISL_.size()));

    for (block = 0; block < blockCount; ++block) VerifyStationConnections_Block(block);
}

void dna_segment::VerifyStationConnections_Block(const UINT32& block) {
    vUINT32::const_iterator _it_isl, _it_jsl, _it_cml;
    bool stationAssociated;
    vUINT32 unusedStns, msrStations, allmsrStations;

#ifdef _MSDEBUG
    std::string isl_stn_name;
#endif

    // get all stations connected with measurements
    for (_it_cml = pImpl->vCML_.at(block).begin(); _it_cml != pImpl->vCML_.at(block).end(); ++_it_cml) {
        // Get all the stations associated with this measurement
        GetMsrStations(pImpl->bmsBinaryRecords_, *_it_cml, msrStations);
        // Append to master station list
        allmsrStations.insert(allmsrStations.end(), msrStations.begin(), msrStations.end());
    }

    strip_duplicates(allmsrStations);

    // Check all isl stations are associated with a measurement
    // No need to check jsl stations as a junction may appear in a block
    // as a result of being carried forward without any measurements
    for (_it_isl = pImpl->vISL_.at(block).begin(); _it_isl != pImpl->vISL_.at(block).end(); ++_it_isl) {
        stationAssociated = false;
        if (binary_search(allmsrStations.begin(), allmsrStations.end(), *_it_isl)) stationAssociated = true;

        if (!stationAssociated) {
            unusedStns.push_back(*_it_isl);

#ifdef _MSDEBUG
            isl_stn_name = pImpl->bstBinaryRecords_.at(*_it_isl).stationName;
#endif
        }
    }

    if (!unusedStns.empty()) {
        std::stringstream ss;
        ss << "The following station(s) are not associated with any" << std::endl
           << "  measurements in block " << block + 1 << ":" << std::endl;
        for_each(unusedStns.begin(), unusedStns.end(), [this, &ss](UINT32& stn) {
            ss << "  " << pImpl->bstBinaryRecords_.at(stn).stationName << " (" << stn << ")" << std::endl;
        });
        SignalExceptionSerialise(ss.str(), 0, NULL);
    }
}

}  // namespace networksegment
}  // namespace dynadjust
