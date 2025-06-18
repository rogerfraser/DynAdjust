cmake_minimum_required(VERSION 3.16)
project(DynAdjustTests LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Include directories
include_directories(../dynadjust)
include_directories(../dynadjust/include)

# Find required packages
find_package(Boost REQUIRED COMPONENTS filesystem system)

# Platform-specific libraries
if(APPLE)
    find_library(ACCELERATE_FRAMEWORK Accelerate)
    set(PLATFORM_LIBS ${ACCELERATE_FRAMEWORK})
elseif(WIN32)
    # Windows-specific libraries can be added here
    set(PLATFORM_LIBS "")
else()
    # Linux-specific libraries (e.g., BLAS/LAPACK) can be added here
    set(PLATFORM_LIBS "")
endif()

# Common source files for I/O tests
set(IO_COMMON_SOURCES
    ../dynadjust/include/io/dnaiobase.cpp
    ../dynadjust/include/measurement_types/dnastation.cpp
)

# Test 1: Matrix test
add_executable(test_matrix
    test_matrix.cpp
    ../dynadjust/include/math/dnamatrix_contiguous.cpp
)

target_link_libraries(test_matrix
    ${PLATFORM_LIBS}
)

target_compile_definitions(test_matrix PRIVATE
    __BINARY_NAME__="test_matrix"
    __BINARY_DESC__="Unit tests for matrix operations"
)

# Test 2: DNA I/O BST test
add_executable(test_dna_io_bst
    test_dna_io_bst.cpp
    ../dynadjust/include/io/dnaiobst.cpp
    ${IO_COMMON_SOURCES}
)

target_link_libraries(test_dna_io_bst
    ${PLATFORM_LIBS}
    Boost::filesystem
    Boost::system
)

target_compile_definitions(test_dna_io_bst PRIVATE
    __BINARY_NAME__="test_dna_io_bst"
    __BINARY_DESC__="Unit tests for dna_io_bst class"
)

# Test 3: DNA I/O ASL test
add_executable(test_dna_io_asl
    test_dna_io_asl.cpp
    ../dynadjust/include/io/dnaioasl.cpp
    ${IO_COMMON_SOURCES}
)

target_link_libraries(test_dna_io_asl
    ${PLATFORM_LIBS}
    Boost::filesystem
    Boost::system
)

target_compile_definitions(test_dna_io_asl PRIVATE
    __BINARY_NAME__="test_dna_io_asl"
    __BINARY_DESC__="Unit tests for dna_io_asl class"
)

# Test 4: DNA I/O AML test
add_executable(test_dna_io_aml
    test_dna_io_aml.cpp
    ../dynadjust/include/io/dnaioaml.cpp
    ../dynadjust/include/io/dnaiobms.cpp
    ../dynadjust/include/measurement_types/dnamsrtally.cpp
    ${IO_COMMON_SOURCES}
)

target_link_libraries(test_dna_io_aml
    ${PLATFORM_LIBS}
    Boost::filesystem
    Boost::system
)

target_compile_definitions(test_dna_io_aml PRIVATE
    __BINARY_NAME__="test_dna_io_aml"
    __BINARY_DESC__="Unit tests for dna_io_aml class"
)

# Test 5: DNA I/O BMS test
add_executable(test_dna_io_bms
    test_dna_io_bms.cpp
    ../dynadjust/include/io/dnaiobms.cpp
    ${IO_COMMON_SOURCES}
)

target_link_libraries(test_dna_io_bms
    ${PLATFORM_LIBS}
    Boost::filesystem
    Boost::system
)

target_compile_definitions(test_dna_io_bms PRIVATE
    __BINARY_NAME__="test_dna_io_bms"
    __BINARY_DESC__="Unit tests for dna_io_bms class"
)

# Test 6: Network Data Manager test
add_executable(test_network_data_manager
    test_network_data_manager.cpp
    ../dynadjust/dynadjust/dnaadjust/network_data_manager.cpp
    ../dynadjust/dynadjust/dnaadjust/associated_station_loader.cpp
    ../dynadjust/dynadjust/dnaadjust/binary_measurement_loader.cpp
    ../dynadjust/dynadjust/dnaadjust/binary_station_loader.cpp
    ../dynadjust/dynadjust/dnaadjust/measurement_processor.cpp
    ../dynadjust/include/io/dnaioasl.cpp
    ../dynadjust/include/io/dnaiobst.cpp
    ../dynadjust/include/io/dnaiobms.cpp
    ${IO_COMMON_SOURCES}
)

target_link_libraries(test_network_data_manager
    ${PLATFORM_LIBS}
    Boost::filesystem
    Boost::system
)

target_compile_definitions(test_network_data_manager PRIVATE
    __BINARY_NAME__="test_network_data_manager"
    __BINARY_DESC__="Unit tests for NetworkDataManager class"
)

# Test 7: Associated Station Loader test
add_executable(test_associated_station_loader
    test_associated_station_loader.cpp
    ../dynadjust/dynadjust/dnaadjust/associated_station_loader.cpp
    ../dynadjust/include/io/dnaioasl.cpp
    ${IO_COMMON_SOURCES}
)

target_link_libraries(test_associated_station_loader
    ${PLATFORM_LIBS}
    Boost::filesystem
    Boost::system
)

target_compile_definitions(test_associated_station_loader PRIVATE
    __BINARY_NAME__="test_associated_station_loader"
    __BINARY_DESC__="Unit tests for AssociatedStationLoader class"
)

# Test 8: Measurement Processor test
add_executable(test_measurement_processor
    test_measurement_processor.cpp
    ../dynadjust/dynadjust/dnaadjust/measurement_processor.cpp
    ${IO_COMMON_SOURCES}
)

target_link_libraries(test_measurement_processor
    ${PLATFORM_LIBS}
    Boost::filesystem
    Boost::system
)

target_compile_definitions(test_measurement_processor PRIVATE
    __BINARY_NAME__="test_measurement_processor"
    __BINARY_DESC__="Unit tests for MeasurementProcessor class"
)

# Enable testing
enable_testing()

# Add tests
add_test(NAME MatrixTest COMMAND test_matrix)
add_test(NAME DNAIOBSTTest COMMAND test_dna_io_bst)
add_test(NAME DNAIOASLTest COMMAND test_dna_io_asl)
add_test(NAME DNAIOAMLTest COMMAND test_dna_io_aml)
add_test(NAME DNAIOBMSTest COMMAND test_dna_io_bms)
add_test(NAME NetworkDataManagerTest COMMAND test_network_data_manager)
add_test(NAME AssociatedStationLoaderTest COMMAND test_associated_station_loader)
add_test(NAME MeasurementProcessorTest COMMAND test_measurement_processor)

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_matrix test_dna_io_bst test_dna_io_asl test_dna_io_aml test_dna_io_bms test_network_data_manager test_associated_station_loader test_measurement_processor
    COMMENT "Running all tests"
)

# Custom target equivalent to 'make all'
add_custom_target(tests_all
    DEPENDS test_matrix test_dna_io_bst test_dna_io_asl test_dna_io_aml test_dna_io_bms test_network_data_manager test_associated_station_loader test_measurement_processor
    COMMENT "Building all tests"
)