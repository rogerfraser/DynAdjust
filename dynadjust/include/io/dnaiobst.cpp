//============================================================================
// Name         : dnaiobst.cpp
// Copyright    : Copyright 2025 Geoscience Australia
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
// Description  : DynAdjust binary station file io operations
//============================================================================

#include <include/functions/dnaiostreamfuncs.hpp>
#include <include/io/dnaiobst.hpp>

namespace dynadjust {
namespace iostreams {

dna_io_bst& dna_io_bst::operator=(const dna_io_bst& rhs) {
    if (this == &rhs)
        return *this;

    dna_io_base::operator=(rhs);
    return *this;
}

UINT16 dna_io_bst::create_stn_input_file_meta(vifm_t& vinput_file_meta, input_file_meta_t** input_file_meta) {
    try {
        // Count matching entries
        UINT16 count = 0;
        for (const auto& ifm : vinput_file_meta) {
            if (ifm.datatype == stn_data || ifm.datatype == stn_msr_data)
                count++;
        }

        // Clean up existing memory
        if (*input_file_meta != nullptr)
            delete[] *input_file_meta;

        // Allocate new memory
        *input_file_meta = new input_file_meta_t[count];

        // Copy matching entries
        UINT16 idx = 0;
        for (const auto& ifm : vinput_file_meta) {
            if (ifm.datatype == stn_data || ifm.datatype == stn_msr_data) {
                (*input_file_meta)[idx] = ifm;
                idx++;
            }
        }

        return count;
    } catch (...) {
        std::stringstream ss;
        ss << "create_stn_input_file_meta(): An error was encountered when creating" << std::endl
           << "  the binary station file metadata." << std::endl;
        throw std::runtime_error(ss.str());
    }
}

void dna_io_bst::load_bst_file_meta(const std::string& bst_filename, binary_file_meta_t& bst_meta) {
    std::stringstream ss;
    ss << "load_bst_file(): An error was encountered when opening " << bst_filename << "." << std::endl;

    try {
        // RAII file stream - closes automatically
        std::ifstream bst_file{bst_filename, std::ios::in | std::ios::binary};
        if (!bst_file.is_open()) {
            throw std::runtime_error(ss.str());
        }

        // read the file information
        readFileInfo(bst_file);

        // read the metadata
        readFileMetadata(bst_file, bst_meta);
    } catch (const std::ios_base::failure& f) {
        ss.str("");
        ss << "load_bst_file(): An error was encountered when reading from " << bst_filename << "." << std::endl;
        ss << f.what();
        throw std::runtime_error(ss.str());
    } catch (const std::runtime_error& e) {
        ss << e.what();
        throw std::runtime_error(ss.str());
    } catch (...) { throw std::runtime_error(ss.str()); }
}

UINT32 dna_io_bst::load_bst_file(const std::string& bst_filename, pvstn_t vbinary_stn, binary_file_meta_t& bst_meta) {
    std::stringstream ss;
    ss << "load_bst_file(): An error was encountered when opening " << bst_filename << "." << std::endl;

    try {
        // RAII file stream - closes automatically
        std::ifstream bst_file{bst_filename, std::ios::in | std::ios::binary};
        if (!bst_file.is_open()) {
            throw std::runtime_error(ss.str());
        }

        // read the file information
        readFileInfo(bst_file);

        // read the metadata
        readFileMetadata(bst_file, bst_meta);

        // read the bst data
        vbinary_stn->reserve(bst_meta.binCount);
        for (UINT32 stn = 0; stn < bst_meta.binCount; ++stn) {
            station_t stationRecord;
            bst_file.read(reinterpret_cast<char*>(&stationRecord), sizeof(station_t));
            vbinary_stn->push_back(stationRecord);
        }

        return bst_meta.binCount;
    } catch (const std::ios_base::failure& f) {
        ss.str("");
        ss << "load_bst_file(): An error was encountered when reading from " << bst_filename << "." << std::endl;
        ss << f.what();
        throw std::runtime_error(ss.str());
    } catch (const std::runtime_error& e) {
        ss << e.what();
        throw std::runtime_error(ss.str());
    } catch (...) { throw std::runtime_error(ss.str()); }
}

void dna_io_bst::write_bst_file(const std::string& bst_filename, pvstn_t vbinary_stn, binary_file_meta_t& bst_meta) {
    std::stringstream ss;
    ss << "write_bst_file(): An error was encountered when opening " << bst_filename << "." << std::endl;

    try {
        // RAII file stream - closes automatically
        std::ofstream bst_file{bst_filename, std::ios::out | std::ios::binary};
        if (!bst_file.is_open()) {
            throw std::runtime_error(ss.str());
        }

        // write version
        writeFileInfo(bst_file);

        // write the metadata
        writeFileMetadata(bst_file, bst_meta);

        // write the bst data using range-for
        for (const auto& station : *vbinary_stn) {
            bst_file.write(reinterpret_cast<const char*>(&station), sizeof(station_t));
        }
    } catch (const std::ios_base::failure& f) {
        ss.str("");
        ss << "write_bst_file(): An error was encountered when writing to " << bst_filename << "." << std::endl;
        ss << f.what();
        throw std::runtime_error(ss.str());
    } catch (const std::runtime_error& e) {
        ss << e.what();
        throw std::runtime_error(ss.str());
    } catch (...) {
        ss.str("");
        ss << "write_bst_file(): An error was encountered when writing to " << bst_filename << "." << std::endl;
        throw std::runtime_error(ss.str());
    }
}

bool dna_io_bst::write_bst_file(const std::string& bst_filename, vdnaStnPtr* vStations, pvstring vUnusedStns,
                                binary_file_meta_t& bst_meta, bool flagUnused) {
    std::stringstream ss;
    ss << "write_bst_file(): An error was encountered when opening " << bst_filename << "." << std::endl;

    try {
        // RAII file stream - closes automatically
        std::ofstream bst_file{bst_filename, std::ios::out | std::ios::binary};
        if (!bst_file.is_open()) {
            throw std::runtime_error(ss.str());
        }

        // write version
        writeFileInfo(bst_file);

        // write the metadata
        writeFileMetadata(bst_file, bst_meta);

        // write the bst data
        if (flagUnused) {
            std::sort(vUnusedStns->begin(), vUnusedStns->end());
            for (auto& station : *vStations) {
                if (std::binary_search(vUnusedStns->begin(), vUnusedStns->end(), station->GetName())) {
                    station->SetStationUnused();
                    station->WriteBinaryStn(&bst_file, true);
                } else {
                    station->WriteBinaryStn(&bst_file);
                }
            }
        } else {
            for (auto& station : *vStations) {
                station->WriteBinaryStn(&bst_file);
            }
        }

        return true;
    } catch (const std::ios_base::failure& f) {
        ss.str("");
        ss << "write_bst_file(): An error was encountered when writing to " << bst_filename << "." << std::endl;
        ss << f.what();
        throw std::runtime_error(ss.str());
    } catch (const std::runtime_error& e) {
        ss << e.what();
        throw std::runtime_error(ss.str());
    } catch (...) {
        ss.str("");
        ss << "write_bst_file(): An error was encountered when writing to " << bst_filename << "." << std::endl;
        throw std::runtime_error(ss.str());
    }
}

} // namespace iostreams
} // namespace dynadjust
