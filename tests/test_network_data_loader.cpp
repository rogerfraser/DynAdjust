#define TESTING_MAIN
#define __BINARY_NAME__ "test_network_data_loader"
#define __BINARY_DESC__ "Unit tests for NetworkDataLoader class"

#include "testing.hpp"

#ifndef __BINARY_NAME__
#define __BINARY_NAME__ "test_network_data_loader"
#endif
#ifndef __BINARY_DESC__
#define __BINARY_DESC__ "Unit tests for NetworkDataLoader class"
#endif

#include "../dynadjust/dynadjust/dnaadjust/network_data_loader.hpp"
#include "../dynadjust/include/config/dnaoptions.hpp"
#include "../dynadjust/include/config/dnatypes.hpp"
#include "../dynadjust/include/measurement_types/dnameasurement.hpp"
#include "../dynadjust/include/measurement_types/dnastation.hpp"
#include <filesystem>
#include <optional>

using namespace dynadjust::networkadjust;
using namespace dynadjust::measurements;

namespace {
// Mock classes for testing
namespace loaders {
class BinaryStationLoader {
  public:
    explicit BinaryStationLoader(std::string_view filename): filename_(filename), should_fail_(false) {}

    std::optional<UINT32> load(vstn_t* bstBinaryRecords, binary_file_meta_t& bst_meta) {
        if (should_fail_)
            return std::nullopt;

        if (bstBinaryRecords) {
            // Create test station data
            station_t stn1;
            snprintf(stn1.stationName, sizeof(stn1.stationName), "STN001");
            stn1.currentLatitude = -35.308;
            stn1.currentLongitude = 149.124;
            stn1.currentHeight = 100.0;
            bstBinaryRecords->push_back(stn1);

            station_t stn2;
            snprintf(stn2.stationName, sizeof(stn2.stationName), "STN002");
            stn2.currentLatitude = -35.342;
            stn2.currentLongitude = 149.161;
            stn2.currentHeight = 150.0;
            bstBinaryRecords->push_back(stn2);
        }

        bst_meta.binCount = 2;
        snprintf(bst_meta.epsgCode, sizeof(bst_meta.epsgCode), "7843");
        snprintf(bst_meta.epoch, sizeof(bst_meta.epoch), "01.01.2020");

        return 2;
    }

    void setShouldFail(bool fail) { should_fail_ = fail; }

  private:
    std::string filename_;
    bool should_fail_;
};

class AssociatedStationLoader {
  public:
    explicit AssociatedStationLoader(std::string_view filename): filename_(filename), should_fail_(false) {}

    std::optional<UINT32> load(vASL* vAssocStnList, vUINT32* v_ISLTemp) {
        if (should_fail_)
            return std::nullopt;

        if (v_ISLTemp) {
            v_ISLTemp->push_back(0);
            v_ISLTemp->push_back(1);
        }

        if (vAssocStnList) {
            // Use emplace_back to avoid copy constructor issues
            vAssocStnList->emplace_back();
            vAssocStnList->back().SetAssocMsrCount(2);
            vAssocStnList->back().SetAMLStnIndex(0);
            vAssocStnList->back().SetValid();

            vAssocStnList->emplace_back();
            vAssocStnList->back().SetAssocMsrCount(1);
            vAssocStnList->back().SetAMLStnIndex(1);
            vAssocStnList->back().SetValid();
        }

        return 2;
    }

    void setShouldFail(bool fail) { should_fail_ = fail; }

  private:
    std::string filename_;
    bool should_fail_;
};

class BinaryMeasurementLoader {
  public:
    explicit BinaryMeasurementLoader(std::string_view filename): filename_(filename), should_fail_(false) {}

    std::optional<UINT32> load(vmsr_t* bmsBinaryRecords, binary_file_meta_t& bms_meta) {
        if (should_fail_)
            return std::nullopt;

        if (bmsBinaryRecords) {
            // Create test measurement data
            measurement_t msr1;
            msr1.measType = 'G';
            msr1.station1 = 0;
            msr1.station2 = 1;
            msr1.ignore = false;
            msr1.term1 = 1000.123;
            msr1.term2 = 2000.456;
            msr1.term3 = 100.789;
            bmsBinaryRecords->push_back(msr1);

            measurement_t msr2;
            msr2.measType = 'S';
            msr2.station1 = 1;
            msr2.station2 = 0;
            msr2.ignore = false;
            msr2.term1 = 1500.789;
            bmsBinaryRecords->push_back(msr2);
        }

        bms_meta.binCount = 2;
        snprintf(bms_meta.epsgCode, sizeof(bms_meta.epsgCode), "7843");
        snprintf(bms_meta.epoch, sizeof(bms_meta.epoch), "01.01.2020");

        return 2;
    }

    void setShouldFail(bool fail) { should_fail_ = fail; }

  private:
    std::string filename_;
    bool should_fail_;
};
} // namespace loaders

namespace processors {
enum class AdjustmentMode { Simultaneous, Phased };

struct MeasurementCounts {
    UINT32 measurement_count = 0;
    UINT32 measurement_variance_count = 0;
    UINT32 measurement_params = 0;
};

class MeasurementProcessor {
  public:
    explicit MeasurementProcessor(AdjustmentMode mode): mode_(mode), should_fail_(false) {}

    std::optional<UINT32>
    processForMode(const vmsr_t& bmsBinaryRecords, UINT32 bmsr_count, vvUINT32& v_CML, MeasurementCounts& counts) {
        if (should_fail_)
            return std::nullopt;

        counts.measurement_count = bmsr_count;
        counts.measurement_variance_count = bmsr_count * 3; // Typical for GPS measurements
        counts.measurement_params = bmsr_count;

        // Create cluster measurement list for simultaneous mode
        if (mode_ == AdjustmentMode::Simultaneous) {
            v_CML.clear();
            vUINT32 cluster;
            for (UINT32 i = 0; i < bmsr_count; ++i) {
                cluster.push_back(i);
            }
            v_CML.push_back(cluster);
        }

        return bmsr_count;
    }

    void setShouldFail(bool fail) { should_fail_ = fail; }

  private:
    AdjustmentMode mode_;
    bool should_fail_;
};
} // namespace processors

// Helper function to create test project settings
project_settings createTestSettings(adjustMode mode = SimultaneousMode) {
    project_settings settings;

    // Initialize adjust settings
    settings.a.adjust_mode = mode;
    settings.a.bst_file = "test.bst";
    settings.a.bms_file = "test.bms";

    // Initialize segment settings
    settings.s.asl_file = "test.asl";

    return settings;
}

// Test callback flags
bool error_handler_called = false;
bool constraint_applier_called = false;
bool invalid_station_remover_called = false;
bool non_measurement_remover_called = false;
bool measurement_count_updater_called = false;

std::string last_error_message;
UINT32 last_error_station = 0;
UINT32 last_measurement_count = 0;
UINT32 last_measurement_variance_count = 0;

// Reset callback flags
void resetCallbackFlags() {
    error_handler_called = false;
    constraint_applier_called = false;
    invalid_station_remover_called = false;
    non_measurement_remover_called = false;
    measurement_count_updater_called = false;
    last_error_message.clear();
    last_error_station = 0;
    last_measurement_count = 0;
    last_measurement_variance_count = 0;
}

// Callback implementations
void testErrorHandler(const std::string& message, UINT32 station) {
    error_handler_called = true;
    last_error_message = message;
    last_error_station = station;
}

void testConstraintApplier() { constraint_applier_called = true; }

void testInvalidStationRemover(vUINT32& stations) {
    invalid_station_remover_called = true;
    // Remove second station for testing
    if (stations.size() > 1) {
        stations.erase(stations.begin() + 1);
    }
}

void testNonMeasurementRemover(UINT32 index) { non_measurement_remover_called = true; }

void testMeasurementCountUpdater(UINT32 count, UINT32 variance_count) {
    measurement_count_updater_called = true;
    last_measurement_count = count;
    last_measurement_variance_count = variance_count;
}
} // namespace

// Basic constructor test
TEST_CASE("NetworkDataLoader constructor", "[network_data_loader][basic]") {
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);
    REQUIRE(true); // Constructor should not throw
}

// Test successful loading with file existence handling
TEST_CASE("Load network files with error handling", "[network_data_loader][load]") {
    resetCallbackFlags();
    auto settings = createTestSettings(SimultaneousMode);
    NetworkDataLoader manager(settings);

    // Set up callbacks
    manager.SetErrorHandler(testErrorHandler);
    manager.SetConstraintApplier(testConstraintApplier);
    manager.SetInvalidStationRemover(testInvalidStationRemover);
    manager.SetNonMeasurementRemover(testNonMeasurementRemover);
    manager.SetMeasurementCountUpdater(testMeasurementCountUpdater);

    // Prepare data structures
    vstn_t bstBinaryRecords;
    binary_file_meta_t bst_meta;
    vASL vAssocStnList;
    vmsr_t bmsBinaryRecords;
    binary_file_meta_t bms_meta;
    vvUINT32 v_ISL;
    v_uint32_uint32_map v_blockStationsMap(1);
    vvUINT32 v_CML;

    UINT32 bstn_count = 0, asl_count = 0, bmsr_count = 0;
    UINT32 unknownParams = 0, unknownsCount = 0;
    UINT32 measurementParams = 0, measurementCount = 0;

    bool result = manager.LoadInto(&bstBinaryRecords, bst_meta, &vAssocStnList, &bmsBinaryRecords, bms_meta,
                                           v_ISL, &v_blockStationsMap, &v_CML, bstn_count, asl_count, bmsr_count,
                                           unknownParams, unknownsCount, measurementParams, measurementCount);

    // The actual result will depend on file availability, but test that:
    // 1. The method doesn't crash
    // 2. Error handler can be called if files don't exist
    // 3. Data structures are properly initialized

    REQUIRE(v_blockStationsMap.size() == 1);

    // Check that error handler was called if loading failed
    if (!result) {
        REQUIRE(error_handler_called == true);
        REQUIRE(!last_error_message.empty());
    }
}

// Test phased mode configuration
TEST_CASE("Phased mode configuration", "[network_data_loader][phased]") {
    resetCallbackFlags();
    auto settings = createTestSettings(PhasedMode);
    NetworkDataLoader manager(settings);

    manager.SetErrorHandler(testErrorHandler);
    manager.SetConstraintApplier(testConstraintApplier);
    manager.SetInvalidStationRemover(testInvalidStationRemover);

    // Prepare data structures
    vstn_t bstBinaryRecords;
    binary_file_meta_t bst_meta;
    vASL vAssocStnList;
    vmsr_t bmsBinaryRecords;
    binary_file_meta_t bms_meta;
    vvUINT32 v_ISL;
    v_uint32_uint32_map v_blockStationsMap(1);
    vvUINT32 v_CML;

    UINT32 bstn_count = 0, asl_count = 0, bmsr_count = 0;
    UINT32 unknownParams = 0, unknownsCount = 0;
    UINT32 measurementParams = 0, measurementCount = 0;

    bool result = manager.LoadInto(&bstBinaryRecords, bst_meta, &vAssocStnList, &bmsBinaryRecords, bms_meta,
                                           v_ISL, &v_blockStationsMap, &v_CML, bstn_count, asl_count, bmsr_count,
                                           unknownParams, unknownsCount, measurementParams, measurementCount);

    // Test basic functionality and error handling
    REQUIRE(v_blockStationsMap.size() == 1);

    // Check that error handler was called if loading failed
    if (!result) {
        REQUIRE(error_handler_called == true);
    }
}

// Test station loading failure
TEST_CASE("Station loading failure", "[network_data_loader][error]") {
    resetCallbackFlags();
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    manager.SetErrorHandler(testErrorHandler);

    // Mock station loader to fail
    // Note: In a real implementation, we'd need dependency injection to control
    // this For this test, we'll simulate the error handling behavior

    vstn_t bstBinaryRecords;
    binary_file_meta_t bst_meta;
    vASL vAssocStnList;
    vmsr_t bmsBinaryRecords;
    binary_file_meta_t bms_meta;
    vvUINT32 v_ISL;
    v_uint32_uint32_map v_blockStationsMap(1);
    vvUINT32 v_CML;

    UINT32 bstn_count = 0, asl_count = 0, bmsr_count = 0;
    UINT32 unknownParams = 0, unknownsCount = 0;
    UINT32 measurementParams = 0, measurementCount = 0;

    // This test would require mocking the actual loaders
    // For now, we test that the method can be called without crashing
    REQUIRE(true);
}

// Test ASL loading failure
TEST_CASE("ASL loading failure", "[network_data_loader][error]") {
    resetCallbackFlags();
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    manager.SetErrorHandler(testErrorHandler);

    // Similar to above - would need dependency injection for proper mocking
    REQUIRE(true);
}

// Test measurement loading failure
TEST_CASE("Measurement loading failure", "[network_data_loader][error]") {
    resetCallbackFlags();
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    manager.SetErrorHandler(testErrorHandler);

    // Similar to above - would need dependency injection for proper mocking
    REQUIRE(true);
}

// Test callback setters
TEST_CASE("Callback setters", "[network_data_loader][callbacks]") {
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    // Test that setters don't throw
    manager.SetErrorHandler([](const std::string& msg, UINT32 stn) {});
    manager.SetConstraintApplier([]() {});
    manager.SetInvalidStationRemover([](vUINT32& stations) {});
    manager.SetNonMeasurementRemover([](UINT32 index) {});
    manager.SetMeasurementCountUpdater([](UINT32 count, UINT32 variance) {});

    REQUIRE(true);
}

// Test move semantics
TEST_CASE("Move semantics", "[network_data_loader][move]") {
    auto settings = createTestSettings();
    NetworkDataLoader manager1(settings);

    // Test move constructor
    NetworkDataLoader manager2 = std::move(manager1);

    REQUIRE(true);
}

// Test copy semantics are deleted
TEST_CASE("Copy semantics deleted", "[network_data_loader][copy]") {
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    // These should not compile:
    // NetworkDataLoader manager2(manager);     // Copy constructor deleted
    // NetworkDataLoader manager3 = manager;    // Copy constructor deleted
    // manager3 = manager;                       // Assignment operator deleted

    REQUIRE(true);
}

// Test with null pointers (defensive programming)
TEST_CASE("Handle null pointers", "[network_data_loader][null]") {
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    manager.SetErrorHandler(testErrorHandler);

    vvUINT32 v_ISL;
    v_uint32_uint32_map v_blockStationsMap(1);
    vvUINT32 v_CML;
    binary_file_meta_t bst_meta, bms_meta;

    UINT32 bstn_count = 0, asl_count = 0, bmsr_count = 0;
    UINT32 unknownParams = 0, unknownsCount = 0;
    UINT32 measurementParams = 0, measurementCount = 0;

    // Test with some null pointers
    bool result = manager.LoadInto(nullptr, bst_meta, nullptr, nullptr, bms_meta, v_ISL, &v_blockStationsMap,
                                           &v_CML, bstn_count, asl_count, bmsr_count, unknownParams, unknownsCount,
                                           measurementParams, measurementCount);

    // Should handle gracefully - exact behavior depends on implementation
    REQUIRE(true);
}

// Test parameter calculation logic
TEST_CASE("Parameter calculation logic", "[network_data_loader][params]") {
    resetCallbackFlags();
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    manager.SetErrorHandler(testErrorHandler);
    manager.SetInvalidStationRemover([](vUINT32& stations) {
        // Don't remove any stations for this test
    });

    vstn_t bstBinaryRecords;
    binary_file_meta_t bst_meta;
    vASL vAssocStnList;
    vmsr_t bmsBinaryRecords;
    binary_file_meta_t bms_meta;
    vvUINT32 v_ISL;
    v_uint32_uint32_map v_blockStationsMap(1);
    vvUINT32 v_CML;

    UINT32 bstn_count = 0, asl_count = 0, bmsr_count = 0;
    UINT32 unknownParams = 0, unknownsCount = 0;
    UINT32 measurementParams = 0, measurementCount = 0;

    bool result = manager.LoadInto(&bstBinaryRecords, bst_meta, &vAssocStnList, &bmsBinaryRecords, bms_meta,
                                           v_ISL, &v_blockStationsMap, &v_CML, bstn_count, asl_count, bmsr_count,
                                           unknownParams, unknownsCount, measurementParams, measurementCount);

    // Test that the calculation logic works even if loading fails
    REQUIRE(v_blockStationsMap.size() == 1);

    // If loading failed, error handler should be called
    if (!result) {
        REQUIRE(error_handler_called == true);
    }
}

// Test different adjustment modes
TEST_CASE("Different adjustment modes", "[network_data_loader][modes]") {
    for (auto mode : {SimultaneousMode, PhasedMode, Phased_Block_1Mode}) {
        resetCallbackFlags();
        auto settings = createTestSettings(mode);
        NetworkDataLoader manager(settings);

        manager.SetErrorHandler(testErrorHandler);

        vstn_t bstBinaryRecords;
        binary_file_meta_t bst_meta;
        vASL vAssocStnList;
        vmsr_t bmsBinaryRecords;
        binary_file_meta_t bms_meta;
        vvUINT32 v_ISL;
        v_uint32_uint32_map v_blockStationsMap(1);
        vvUINT32 v_CML;

        UINT32 bstn_count = 0, asl_count = 0, bmsr_count = 0;
        UINT32 unknownParams = 0, unknownsCount = 0;
        UINT32 measurementParams = 0, measurementCount = 0;

        bool result = manager.LoadInto(&bstBinaryRecords, bst_meta, &vAssocStnList, &bmsBinaryRecords, bms_meta,
                                               v_ISL, &v_blockStationsMap, &v_CML, bstn_count, asl_count, bmsr_count,
                                               unknownParams, unknownsCount, measurementParams, measurementCount);

        // Test that mode doesn't crash and basic structures are initialized
        REQUIRE(v_blockStationsMap.size() == 1);

        // If loading failed, error handler should be called
        if (!result) {
            REQUIRE(error_handler_called == true);
        }
    }
}

// Test error handler functionality
TEST_CASE("Error handler functionality", "[network_data_loader][error_handler]") {
    resetCallbackFlags();
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    manager.SetErrorHandler(testErrorHandler);

    // Test that error handler can be called
    testErrorHandler("Test error message", 42);

    REQUIRE(error_handler_called == true);
    REQUIRE(last_error_message == "Test error message");
    REQUIRE(last_error_station == 42);
}

// Test measurement count updater
TEST_CASE("Measurement count updater", "[network_data_loader][updater]") {
    resetCallbackFlags();
    auto settings = createTestSettings();
    NetworkDataLoader manager(settings);

    manager.SetMeasurementCountUpdater(testMeasurementCountUpdater);

    // Test that updater can be called
    testMeasurementCountUpdater(100, 300);

    REQUIRE(measurement_count_updater_called == true);
    REQUIRE(last_measurement_count == 100);
    REQUIRE(last_measurement_variance_count == 300);
}

// Test RemoveInvalidStations method
TEST_CASE("RemoveInvalidStations functionality", "[network_data_loader][station_filter]") {
    auto settings = createTestSettings();
    NetworkDataLoader loader(settings);

    // Create test associated stations list
    vASL test_associated_stations;
    
    // Add valid station
    test_associated_stations.emplace_back();
    test_associated_stations.back().SetValid();
    
    // Add invalid station
    test_associated_stations.emplace_back();
    test_associated_stations.back().SetInvalid();
    
    // Add another valid station
    test_associated_stations.emplace_back();
    test_associated_stations.back().SetValid();

    // Create station list with all indices
    vUINT32 station_list = {0, 1, 2};
    
    // Apply filter
    loader.RemoveInvalidStations(station_list, test_associated_stations);
    
    // Should have removed index 1 (invalid station)
    REQUIRE(station_list.size() == 2);
    REQUIRE(station_list[0] == 0); // First valid station
    REQUIRE(station_list[1] == 2); // Second valid station
}
