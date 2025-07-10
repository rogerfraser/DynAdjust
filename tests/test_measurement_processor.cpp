//============================================================================
// Name         : test_measurement_processor.cpp
// Author       : Dale Roberts <dale.o.roberts@gmail.com>
// Contributors : 
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
// Description  : Unit tests
//============================================================================

#define TESTING_MAIN
#define __BINARY_NAME__ "test_measurement_processor"
#define __BINARY_DESC__ "Unit tests for MeasurementProcessor class"

#include "testing.hpp"

#ifndef __BINARY_NAME__
#define __BINARY_NAME__ "test_measurement_processor"
#endif
#ifndef __BINARY_DESC__
#define __BINARY_DESC__ "Unit tests for MeasurementProcessor class"
#endif

#include "../dynadjust/dynadjust/dnaadjust/measurement_processor.hpp"
#include "../dynadjust/include/config/dnatypes.hpp"
#include "../dynadjust/include/measurement_types/dnameasurement.hpp"

using namespace dynadjust::networkadjust::processors;
using namespace dynadjust::measurements;

namespace {
// Helper function to create test measurement data
measurement_t createTestMeasurement(char type, bool ignore = false, UINT32 clusterID = 0, UINT32 vectorCount2 = 0,
                                    MEASUREMENT_START measStart = xMeas) {
    measurement_t msr = {};
    msr.measType = type;
    msr.ignore = ignore;
    msr.clusterID = clusterID;
    msr.vectorCount2 = vectorCount2;
    msr.measStart = measStart;
    msr.station1 = 0;
    msr.station2 = 1;
    msr.term1 = 100.0;
    return msr;
}

// Helper function to create GPS baseline measurements
vmsr_t createGPSBaselines() {
    vmsr_t measurements;

    // GPS X component
    measurement_t gps_x = createTestMeasurement('X', false, 1);
    measurements.push_back(gps_x);

    // GPS Y component
    measurement_t gps_y = createTestMeasurement('Y', false, 1);
    measurements.push_back(gps_y);

    // GPS Z component (G type)
    measurement_t gps_z = createTestMeasurement('G', false, 1);
    measurements.push_back(gps_z);

    return measurements;
}

// Helper function to create direction set measurements
vmsr_t createDirectionSet() {
    vmsr_t measurements;

    // Create direction set with vectorCount2 = 5 (5 directions)
    measurement_t dir_set = createTestMeasurement('D', false, 0, 5);
    measurements.push_back(dir_set);

    return measurements;
}

// Helper function to create mixed measurement types
vmsr_t createMixedMeasurements() {
    vmsr_t measurements;

    // Distance measurement
    measurement_t distance = createTestMeasurement('S');
    measurements.push_back(distance);

    // Angle measurement
    measurement_t angle = createTestMeasurement('A');
    measurements.push_back(angle);

    // Height difference
    measurement_t height = createTestMeasurement('L');
    measurements.push_back(height);

    // Zenith angle
    measurement_t zenith = createTestMeasurement('V');
    measurements.push_back(zenith);

    return measurements;
}

// Helper function to create measurements with ignored entries
vmsr_t createMeasurementsWithIgnored() {
    vmsr_t measurements;

    // Valid measurement
    measurement_t valid = createTestMeasurement('S', false);
    measurements.push_back(valid);

    // Ignored measurement
    measurement_t ignored = createTestMeasurement('S', true);
    measurements.push_back(ignored);

    // Another valid measurement
    measurement_t valid2 = createTestMeasurement('A', false);
    measurements.push_back(valid2);

    return measurements;
}

// Helper function to create measurements with covariance terms
vmsr_t createMeasurementsWithCovariance() {
    vmsr_t measurements;

    // Valid measurement term
    measurement_t measurement = createTestMeasurement('S', false, 0, 0, xMeas);
    measurements.push_back(measurement);

    // Covariance terms (should be ignored)
    measurement_t cov_x = createTestMeasurement('S', false, 0, 0, xCov);
    measurements.push_back(cov_x);

    measurement_t cov_y = createTestMeasurement('S', false, 0, 0, yCov);
    measurements.push_back(cov_y);

    measurement_t cov_z = createTestMeasurement('S', false, 0, 0, zCov);
    measurements.push_back(cov_z);

    return measurements;
}
} // namespace

// Basic constructor tests
TEST_CASE("MeasurementProcessor constructor simultaneous", "[measurement_processor][basic]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);
    REQUIRE(true); // Constructor should not throw
}

TEST_CASE("MeasurementProcessor constructor phased", "[measurement_processor][basic]") {
    MeasurementProcessor processor(AdjustmentMode::Phased);
    REQUIRE(true); // Constructor should not throw
}

// Move semantics tests
TEST_CASE("MeasurementProcessor move constructor", "[measurement_processor][move]") {
    MeasurementProcessor processor1(AdjustmentMode::Simultaneous);
    MeasurementProcessor processor2 = std::move(processor1);
    REQUIRE(true); // Move constructor should work
}

TEST_CASE("MeasurementProcessor move assignment", "[measurement_processor][move]") {
    MeasurementProcessor processor1(AdjustmentMode::Simultaneous);
    MeasurementProcessor processor2(AdjustmentMode::Phased);
    processor2 = std::move(processor1);
    REQUIRE(true); // Move assignment should work
}

// Copy semantics are deleted - compile-time test
TEST_CASE("MeasurementProcessor copy semantics deleted", "[measurement_processor][copy]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    // These should not compile:
    // MeasurementProcessor processor2(processor);     // Copy constructor deleted
    // MeasurementProcessor processor3 = processor;    // Copy constructor deleted
    // processor3 = processor;                         // Assignment operator deleted

    REQUIRE(true);
}

// Empty data tests
TEST_CASE("Process empty measurement list", "[measurement_processor][empty]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t empty_measurements;
    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(empty_measurements, 0, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 0);
    REQUIRE(counts.measurement_count == 0);
    REQUIRE(counts.measurement_variance_count == 0);
    REQUIRE(counts.measurement_params == 0);
    REQUIRE(v_CML.size() == 1);
    REQUIRE(v_CML[0].empty());
}

// Basic measurement processing tests
TEST_CASE("Process single distance measurement", "[measurement_processor][single]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;
    measurements.push_back(createTestMeasurement('S'));

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, 1, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 1);
    REQUIRE(counts.measurement_count == 1);
    REQUIRE(counts.measurement_variance_count == 1);
    REQUIRE(counts.measurement_params == 1);
    REQUIRE(v_CML.size() == 1);
    REQUIRE(v_CML[0].size() == 1);
    REQUIRE(v_CML[0][0] == 0); // Index of the first measurement
}

TEST_CASE("Process single angle measurement", "[measurement_processor][single]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;
    measurements.push_back(createTestMeasurement('A'));

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, 1, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 1);
    REQUIRE(counts.measurement_count == 1);
    REQUIRE(counts.measurement_variance_count == 1);
    REQUIRE(counts.measurement_params == 1);
}

// GPS measurement tests
TEST_CASE("Process GPS baseline measurements", "[measurement_processor][gps]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    auto measurements = createGPSBaselines();
    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 3);
    REQUIRE(counts.measurement_count == 3);
    // GPS measurements have increasing variance counts: 1, 2, 3
    REQUIRE(counts.measurement_variance_count == 6); // 1 + 2 + 3
    REQUIRE(counts.measurement_params == 3);
}

// Direction set tests
TEST_CASE("Process direction set measurements", "[measurement_processor][directions]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    auto measurements = createDirectionSet();
    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 1);
    // Direction set with vectorCount2 = 5 contributes (5-1) = 4 measurements
    REQUIRE(counts.measurement_count == 4);
    REQUIRE(counts.measurement_variance_count == 4);
    REQUIRE(counts.measurement_params == 4);
}

// Mixed measurement types
TEST_CASE("Process mixed measurement types", "[measurement_processor][mixed]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    auto measurements = createMixedMeasurements();
    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 4);
    REQUIRE(counts.measurement_count == 4);
    REQUIRE(counts.measurement_variance_count == 4);
    REQUIRE(counts.measurement_params == 4);
}

// Ignored measurements tests
TEST_CASE("Process measurements with ignored entries", "[measurement_processor][ignored]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    auto measurements = createMeasurementsWithIgnored();
    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    // The loop increments m for all measurements, but only counts non-ignored ones
    REQUIRE(*result == 3);                  // All measurements processed for indexing
    REQUIRE(counts.measurement_count == 2); // Only non-ignored contribute to counts
    REQUIRE(counts.measurement_variance_count == 2);
    REQUIRE(counts.measurement_params == 2);
}

// Covariance terms tests
TEST_CASE("Process measurements with covariance terms", "[measurement_processor][covariance]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    auto measurements = createMeasurementsWithCovariance();
    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    // All measurements are processed for indexing, but only measurement terms count
    REQUIRE(*result == 4);                  // All measurements processed for indexing
    REQUIRE(counts.measurement_count == 1); // Only xMeas contributes to count
    REQUIRE(counts.measurement_variance_count == 1);
    REQUIRE(counts.measurement_params == 1);
}

// Phased mode tests
TEST_CASE("Process in phased mode", "[measurement_processor][phased]") {
    MeasurementProcessor processor(AdjustmentMode::Phased);

    vmsr_t measurements;
    measurements.push_back(createTestMeasurement('S'));

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, 1, v_CML, counts);

    // Phased mode currently returns nullopt
    REQUIRE(result == std::nullopt);
}

// Large dataset tests
TEST_CASE("Process large measurement dataset", "[measurement_processor][large]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;
    const UINT32 num_measurements = 1000;

    // Create 1000 distance measurements
    for (UINT32 i = 0; i < num_measurements; ++i) {
        measurements.push_back(createTestMeasurement('S'));
    }

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, num_measurements, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == num_measurements);
    REQUIRE(counts.measurement_count == num_measurements);
    REQUIRE(counts.measurement_variance_count == num_measurements);
    REQUIRE(counts.measurement_params == num_measurements);
    REQUIRE(v_CML.size() == 1);
    REQUIRE(v_CML[0].size() == num_measurements);
}

// GPS cluster handling tests
TEST_CASE("Process GPS measurements with different clusters", "[measurement_processor][clusters]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;

    // First GPS cluster (cluster ID = 1)
    measurements.push_back(createTestMeasurement('X', false, 1));
    measurements.push_back(createTestMeasurement('Y', false, 1));

    // Second GPS cluster (cluster ID = 2)
    measurements.push_back(createTestMeasurement('X', false, 2));
    measurements.push_back(createTestMeasurement('Y', false, 2));

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 4);
    REQUIRE(counts.measurement_count == 4);
    REQUIRE(v_CML.size() == 1);
    REQUIRE(v_CML[0].size() == 4);
}

// Measurement count vs bmsr_count mismatch tests
TEST_CASE("Handle measurement count mismatch", "[measurement_processor][mismatch]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;
    measurements.push_back(createTestMeasurement('S'));
    measurements.push_back(createTestMeasurement('S', true)); // ignored

    vvUINT32 v_CML;
    MeasurementCounts counts;

    // bmsr_count = 2, but only 1 non-ignored measurement
    auto result = processor.processForMode(measurements, 2, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 2);                  // All measurements processed for indexing
    REQUIRE(counts.measurement_count == 1); // Only non-ignored contributes to count
    REQUIRE(v_CML.size() == 1);
    REQUIRE(v_CML[0].size() == 2); // Resized to actual measurement vector size
}

// Complex direction set tests
TEST_CASE("Process multiple direction sets", "[measurement_processor][multiple_directions]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;

    // First direction set with 3 directions
    measurements.push_back(createTestMeasurement('D', false, 0, 3));

    // Second direction set with 5 directions
    measurements.push_back(createTestMeasurement('D', false, 0, 5));

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 2);
    // First set: (3-1) = 2, Second set: (5-1) = 4, Total = 6
    REQUIRE(counts.measurement_count == 6);
    REQUIRE(counts.measurement_variance_count == 6);
    REQUIRE(counts.measurement_params == 6);
}

// Edge case: Direction set with vectorCount2 = 0
TEST_CASE("Process direction set with zero vector count", "[measurement_processor][edge_direction]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;
    measurements.push_back(createTestMeasurement('D', false, 0, 0)); // vectorCount2 = 0

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, 1, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 1);
    // No contribution to counts since vectorCount2 = 0
    REQUIRE(counts.measurement_count == 0);
    REQUIRE(counts.measurement_variance_count == 0);
    REQUIRE(counts.measurement_params == 0);
}

// Edge case: Direction set with vectorCount2 = 1
TEST_CASE("Process direction set with single vector count", "[measurement_processor][edge_direction_single]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;
    measurements.push_back(createTestMeasurement('D', false, 0, 1)); // vectorCount2 = 1

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, 1, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 1);
    // No contribution since (vectorCount2 - 1) = 0
    REQUIRE(counts.measurement_count == 0);
    REQUIRE(counts.measurement_variance_count == 0);
    REQUIRE(counts.measurement_params == 0);
}

// Input validation tests
TEST_CASE("Process with pre-populated v_CML", "[measurement_processor][pre_populated]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;
    measurements.push_back(createTestMeasurement('S'));

    vvUINT32 v_CML;
    // Pre-populate v_CML with some data
    v_CML.push_back({99, 98, 97});
    v_CML.push_back({96, 95});

    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, 1, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 1);
    // v_CML should be cleared and rebuilt
    REQUIRE(v_CML.size() == 1);
    REQUIRE(v_CML[0].size() == 1);
    REQUIRE(v_CML[0][0] == 0);
}

// Boundary tests
TEST_CASE("Process with maximum UINT32 bmsr_count", "[measurement_processor][boundary]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;
    measurements.push_back(createTestMeasurement('S'));

    vvUINT32 v_CML;
    MeasurementCounts counts;

    // Test with large bmsr_count (measurement vector has only 1 element)
    auto result = processor.processForMode(measurements, 1000, v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 1); // Only 1 measurement processed
    REQUIRE(v_CML.size() == 1);
    REQUIRE(v_CML[0].size() == 1); // Resized to actual count
}

// GPS axis counter test
TEST_CASE("GPS axis counter behavior", "[measurement_processor][gps_axis]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;

    // Create sequence of GPS measurements to test axis counter
    for (int i = 0; i < 6; ++i) {
        measurements.push_back(createTestMeasurement('G'));
    }

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == 6);
    REQUIRE(counts.measurement_count == 6);
    // Variance count should follow pattern: 1, 2, 3, 1, 2, 3 (axis resets after 2)
    REQUIRE(counts.measurement_variance_count == 12); // 1+2+3+1+2+3 = 12
    REQUIRE(counts.measurement_params == 6);
}

// All measurement types test
TEST_CASE("Process all supported measurement types", "[measurement_processor][all_types]") {
    MeasurementProcessor processor(AdjustmentMode::Simultaneous);

    vmsr_t measurements;

    // Add various measurement types
    std::vector<char> types = {'A', 'B', 'C', 'E', 'G', 'H', 'I', 'J', 'K', 'L',
                               'M', 'P', 'Q', 'R', 'S', 'V', 'X', 'Y', 'Z'};

    for (char type : types) {
        measurements.push_back(createTestMeasurement(type));
    }

    vvUINT32 v_CML;
    MeasurementCounts counts;

    auto result = processor.processForMode(measurements, static_cast<UINT32>(measurements.size()), v_CML, counts);

    REQUIRE(result.has_value());
    REQUIRE(*result == types.size());
    REQUIRE(v_CML.size() == 1);
    REQUIRE(v_CML[0].size() == types.size());
}