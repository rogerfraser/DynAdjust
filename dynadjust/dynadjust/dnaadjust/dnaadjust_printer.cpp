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

} // namespace networkadjust
} // namespace dynadjust
