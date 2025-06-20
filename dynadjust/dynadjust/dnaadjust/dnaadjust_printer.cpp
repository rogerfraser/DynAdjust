//============================================================================
// Name         : dnaadjust_printer.cpp
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
// Description  : DynAdjust Network Adjustment Printer Module Implementation
//============================================================================

#include "dnaadjust.hpp"
#include "dnaadjust_printer.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <boost/timer/timer.hpp>

namespace dynadjust {
namespace networkadjust {

DynAdjustPrinter::DynAdjustPrinter(dna_adjust& adjust_instance) 
    : adjust_(adjust_instance) {
}

void DynAdjustPrinter::PrintMeasurementWithStations(it_vmsr_t& it_msr, char measurement_type) {
    // Lambda to print station names based on count
    auto print_stations = [&](int station_count) {
        adjust_.adj_file << std::left << std::setw(STATION) 
                        << adjust_.bstBinaryRecords_.at(it_msr->station1).stationName;
        adjust_.adj_file << std::left << std::setw(STATION) 
                        << adjust_.bstBinaryRecords_.at(it_msr->station2).stationName;
        
        if (station_count == 3) {
            adjust_.adj_file << std::left << std::setw(STATION) 
                            << adjust_.bstBinaryRecords_.at(it_msr->station3).stationName;
        } else {
            adjust_.adj_file << std::left << std::setw(STATION) << " ";
        }
    };
    
    // Get station count for this measurement type
    const int station_count = GetStationCount(measurement_type);
    print_stations(station_count);
    
    // Print the measurement based on type
    if (IsAngularType(measurement_type)) {
        // Delegate to existing angular measurement printer
        adjust_.PrintAdjMeasurementsAngular(' ', it_msr, true);
    } else {
        // Delegate to existing linear measurement printer
        adjust_.PrintAdjMeasurementsLinear(' ', it_msr, true);
    }
}

void DynAdjustPrinter::PrintIteration(const UINT32& iteration) {
    std::stringstream iteration_message;

    iteration_message << std::endl << OUTPUTLINE << std::endl <<
        std::setw(PRINT_VAR_PAD) << std::left << "ITERATION" << iteration << std::endl << std::endl;

    adjust_.adj_file << iteration_message.str();

    if (adjust_.projectSettings_.g.verbose)
        adjust_.debug_file << iteration_message.str();		
}

void DynAdjustPrinter::PrintAdjustmentTime(boost::timer::cpu_timer& time, int timer_type) {
    using namespace boost::posix_time;
    
    // calculate and print total time
    milliseconds ms(milliseconds(time.elapsed().wall/1000000));
    time_duration t(ms);
    
    std::stringstream ss;
    if (t > seconds(1))
        ss << seconds(static_cast<long>(t.total_seconds()));
    else
        ss << t;

    if (timer_type == 0) // iteration_time equivalent
        adjust_.adj_file << std::setw(PRINT_VAR_PAD) << std::left << "Elapsed time" << ss.str() << std::endl;
    else
    {
        adjust_.adj_file << std::setw(PRINT_VAR_PAD) << std::left << "Total time" << ss.str() << std::endl << std::endl;
    }
}

constexpr int DynAdjustPrinter::GetStationCount(char measurement_type) const {
    // Use constexpr lookup for station count
    for (const auto& [type, count] : kStationCounts) {
        if (type == measurement_type) {
            return count;
        }
    }
    return 2; // Default to 2 stations
}

constexpr bool DynAdjustPrinter::IsAngularType(char measurement_type) const {
    // Check if measurement type is angular
    for (const auto& type : kAngularTypes) {
        if (type == measurement_type) {
            return true;
        }
    }
    return false;
}

// Template specializations
template<>
void DynAdjustPrinter::PrintAdjustedMeasurements<AngularMeasurement>(
    char cardinal, const it_vmsr_t& it_msr, bool initialise_dbindex) {
    // Print adjusted angular measurements
    adjust_.PrintMeasurementsAngular(cardinal, it_msr->measAdj, it_msr->measCorr, it_msr);
    
    // Print adjusted statistics
    adjust_.PrintAdjMeasurementStatistics(cardinal, it_msr, initialise_dbindex);
}

template<>
void DynAdjustPrinter::PrintAdjustedMeasurements<LinearMeasurement>(
    char cardinal, const it_vmsr_t& it_msr, bool initialise_dbindex) {
    // Print adjusted linear measurements
    adjust_.PrintMeasurementsLinear(cardinal, it_msr->measAdj, it_msr->measCorr, it_msr);
    
    // Print adjusted statistics
    adjust_.PrintAdjMeasurementStatistics(cardinal, it_msr, initialise_dbindex);
}

// Template specializations for comparative measurements
template<>
void DynAdjustPrinter::PrintComparativeMeasurements<AngularMeasurement>(
    char cardinal, const double& computed, const double& correction, const it_vmsr_t& it_msr) {
    // Print computed angular measurements
    adjust_.PrintMeasurementsAngular(cardinal, computed, correction, it_msr, false);
    
    // Print measurement correction
    adjust_.PrintMeasurementCorrection(cardinal, it_msr);
    
    // Print measurement database ids if enabled
    if (adjust_.projectSettings_.o._database_ids) {
        PrintMeasurementDatabaseID(it_msr, true);
    }
    
    adjust_.adj_file << std::endl;
}

template<>
void DynAdjustPrinter::PrintComparativeMeasurements<LinearMeasurement>(
    char cardinal, const double& computed, const double& correction, const it_vmsr_t& it_msr) {
    // Check if adjustment is questionable for linear measurements
    if (!adjust_.isAdjustmentQuestionable_) {
        adjust_.isAdjustmentQuestionable_ = (fabs(correction) > 999.9999);
    }
    
    // Print computed linear measurements
    adjust_.PrintMeasurementsLinear(cardinal, computed, correction, it_msr, false);
    
    // Print measurement correction
    adjust_.PrintMeasurementCorrection(cardinal, it_msr);
    
    // Print measurement database ids if enabled
    if (adjust_.projectSettings_.o._database_ids) {
        PrintMeasurementDatabaseID(it_msr, true);
    }
    
    adjust_.adj_file << std::endl;
}

// Utility function implementations
void DynAdjustPrinter::PrintAdjustmentStatus() {
    // Print adjustment status
    adjust_.adj_file << std::endl << OUTPUTLINE << std::endl;
    adjust_.adj_file << std::setw(PRINT_VAR_PAD) << std::left << "SOLUTION";

    if (adjust_.projectSettings_.a.report_mode) {
        adjust_.adj_file << "Printing results of last adjustment only" << std::endl;
        return;
    }
    
    switch (adjust_.projectSettings_.a.adjust_mode) {
    case Phased_Block_1Mode:
        if (adjust_.adjustStatus_ == ADJUST_SUCCESS)
            adjust_.adj_file << "Estimates solved for Block 1 only" << std::endl;
        else
            adjust_.adj_file << "Failed to solve Block 1 estimates" << std::endl;
        break;
    default:
        if (adjust_.adjustStatus_ == ADJUST_SUCCESS && 
            adjust_.CurrentIteration() <= adjust_.projectSettings_.a.max_iterations &&
            fabs(adjust_.maxCorr_) <= adjust_.projectSettings_.a.iteration_threshold)
            adjust_.adj_file << "Converged" << std::endl;
        else
            adjust_.adj_file << "Failed to converge" << std::endl;
    }
}

void DynAdjustPrinter::PrintMeasurementDatabaseID(const it_vmsr_t& it_msr, bool initialise_dbindex) {
    // Set iterator to the database index
    if (initialise_dbindex) {
        size_t dbindx = std::distance(adjust_.bmsBinaryRecords_.begin(), it_msr);
        adjust_._it_dbid = adjust_.v_msr_db_map_.begin() + dbindx;
    }

    // Print measurement id
    if (adjust_._it_dbid->is_msr_id_set)
        adjust_.adj_file << std::setw(STDDEV) << std::right << adjust_._it_dbid->msr_id;
    else
        adjust_.adj_file << std::setw(STDDEV) << " ";

    // Print cluster id?
    switch (it_msr->measType) {
    case 'D':
    case 'G':
    case 'X':
    case 'Y':
        if (adjust_._it_dbid->is_cls_id_set)
            adjust_.adj_file << std::setw(STDDEV) << std::right << adjust_._it_dbid->cluster_id;
        else
            adjust_.adj_file << std::setw(STDDEV) << " ";
    }
}

void DynAdjustPrinter::PrintAdjMeasurementStatistics(char cardinal, const it_vmsr_t& it_msr, bool initialise_dbindex) {
    UINT16 PRECISION_STAT(2);

    if (adjust_.isAdjustmentQuestionable_ || (fabs(it_msr->NStat) > adjust_.criticalValue_ * 4.0))
        adjust_.adj_file << StringFromTW(removeNegativeZero(it_msr->NStat, 2), STAT, PRECISION_STAT);
    else
        adjust_.adj_file << std::setw(STAT) << std::setprecision(2) << std::fixed << std::right << 
            removeNegativeZero(it_msr->NStat, 2);

    if (adjust_.projectSettings_.o._adj_msr_tstat) {
        if (adjust_.isAdjustmentQuestionable_ || (fabs(it_msr->NStat) > adjust_.criticalValue_ * 4.0))
            adjust_.adj_file << StringFromTW(removeNegativeZero(it_msr->TStat, 2), STAT, PRECISION_STAT);
        else
            adjust_.adj_file << std::setw(STAT) << std::setprecision(2) << std::fixed << std::right << 
                removeNegativeZero(it_msr->TStat, 2);
    }

    adjust_.adj_file << std::setw(REL) << std::setprecision(2) << std::fixed << std::right << it_msr->PelzerRel;

    // Print measurement correction
    adjust_.PrintMeasurementCorrection(cardinal, it_msr);

    // Print asterisk for values which exceed the critical value
    if (fabs(it_msr->NStat) > adjust_.criticalValue_)
        adjust_.adj_file << std::setw(OUTLIER) << std::right << "*";
    else
        adjust_.adj_file << std::setw(OUTLIER) << std::right << " ";

    // Print measurement database ids
    if (adjust_.projectSettings_.o._database_ids)
        PrintMeasurementDatabaseID(it_msr, initialise_dbindex);

    adjust_.adj_file << std::endl;
}

} // namespace networkadjust
} // namespace dynadjust
