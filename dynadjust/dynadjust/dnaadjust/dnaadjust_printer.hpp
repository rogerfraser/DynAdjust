//============================================================================
// Name         : dnaadjust_printer.hpp
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
// Description  : DynAdjust Network Adjustment Printer Module
//============================================================================

#ifndef DNAADJUST_PRINTER_H_
#define DNAADJUST_PRINTER_H_

#if defined(_MSC_VER)
#if defined(LIST_INCLUDES_ON_BUILD)
#pragma message("  " __FILE__)
#endif
#endif

#include <array>
#include <iostream>
#include <type_traits>

#include <include/config/dnaconsts-iostream.hpp>
#include <include/config/dnaconsts.hpp>
#include <include/config/dnaoptions-interface.hpp>
#include <include/config/dnatypes.hpp>
#include <include/measurement_types/dnameasurement.hpp>
#include <include/measurement_types/dnastation.hpp>

using namespace dynadjust::datum_parameters;
using namespace dynadjust::measurements;

namespace dynadjust {
namespace networkadjust {

// Forward declarations
class dna_adjust;

// Print context structure containing all necessary references for printing
struct PrintContext {
    std::ostream& adj_file;
    std::ostream& debug_file;
    const project_settings& settings;
    const vstn_t& binary_stations;
    const vmsr_t& binary_measurements;

    PrintContext(std::ostream& adj, std::ostream& debug, const project_settings& proj_settings,
                 const vstn_t& bst_records, const vmsr_t& bms_records)
        : adj_file(adj), debug_file(debug), settings(proj_settings), binary_stations(bst_records),
          binary_measurements(bms_records) {}
};

// Tag types for measurement classification
struct AngularMeasurement {};
struct LinearMeasurement {};

// Type traits for measurement classification
template <typename T> struct IsAngularMeasurement : std::false_type {};

template <> struct IsAngularMeasurement<AngularMeasurement> : std::true_type {};

template <typename T> constexpr bool kIsAngularMeasurementV = IsAngularMeasurement<T>::value;

// Printer class for DynAdjust output formatting
class DynAdjustPrinter {
  public:
    // Constructor taking dna_adjust reference for direct access to members
    explicit DynAdjustPrinter(dna_adjust& adjust_instance);

    // Template-based measurement printing
    template <typename MeasurementType>
    void PrintAdjustedMeasurements(char cardinal, const it_vmsr_t& it_msr, bool initialise_dbindex = true);

    // Template-based comparative measurement printing
    template <typename MeasurementType>
    void PrintComparativeMeasurements(char cardinal, const double& computed, const double& correction, const it_vmsr_t& it_msr);

    // Consolidated station formatter for measurement types A, BKVZ, CELMS, HR, IJPQ
    void PrintMeasurementWithStations(it_vmsr_t& it_msr, char measurement_type);

    // Utility functions
    void PrintIteration(const UINT32& iteration);
    void PrintAdjustmentTime(boost::timer::cpu_timer& time, int timer_type);
    void PrintAdjustmentStatus();
    void PrintMeasurementDatabaseID(const it_vmsr_t& it_msr, bool initialise_dbindex = false);
    void PrintAdjMeasurementStatistics(char cardinal, const it_vmsr_t& it_msr, bool initialise_dbindex);

  private:
    dna_adjust& adjust_;

    // Helper functions
    constexpr int GetStationCount(char measurement_type) const;
    constexpr bool IsAngularType(char measurement_type) const;

    // Station count lookup table
    static constexpr std::array kStationCounts{
        std::pair{'A', 3}, std::pair{'B', 2}, std::pair{'K', 2}, std::pair{'V', 2},
        std::pair{'Z', 2}, std::pair{'C', 2}, std::pair{'E', 2}, std::pair{'L', 2},
        std::pair{'M', 2}, std::pair{'S', 2}, std::pair{'H', 2}, std::pair{'R', 2},
        std::pair{'I', 2}, std::pair{'J', 2}, std::pair{'P', 2}, std::pair{'Q', 2}};

    // Angular measurement types
    static constexpr std::array kAngularTypes{'A', 'B', 'K', 'V', 'Z'};
};

// Template implementation - will be specialized for Angular/Linear types
template <typename MeasurementType>
void DynAdjustPrinter::PrintAdjustedMeasurements(char cardinal, const it_vmsr_t& it_msr, bool initialise_dbindex) {
    // Default implementation delegates to existing print functions
    // This will be replaced by explicit specializations
    static_assert(sizeof(MeasurementType) == 0, "Must use specialization");
}

template <typename MeasurementType>
void DynAdjustPrinter::PrintComparativeMeasurements(char cardinal, const double& computed, const double& correction, const it_vmsr_t& it_msr) {
    // Default implementation - will be replaced by explicit specializations
    static_assert(sizeof(MeasurementType) == 0, "Must use specialization");
}

// Template specializations - defined in .cpp file
template <>
void DynAdjustPrinter::PrintAdjustedMeasurements<AngularMeasurement>(char cardinal, const it_vmsr_t& it_msr,
                                                                     bool initialise_dbindex);

template <>
void DynAdjustPrinter::PrintAdjustedMeasurements<LinearMeasurement>(char cardinal, const it_vmsr_t& it_msr,
                                                                    bool initialise_dbindex);

template <>
void DynAdjustPrinter::PrintComparativeMeasurements<AngularMeasurement>(char cardinal, const double& computed, 
                                                                        const double& correction, const it_vmsr_t& it_msr);

template <>
void DynAdjustPrinter::PrintComparativeMeasurements<LinearMeasurement>(char cardinal, const double& computed, 
                                                                       const double& correction, const it_vmsr_t& it_msr);

} // namespace networkadjust
} // namespace dynadjust

#endif // DNAADJUST_PRINTER_H_
