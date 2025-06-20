# <dnaadjust/...> build rules
project (dnaadjust)

#add_definitions(-DBUILD_DNAADJUST_DLL -DBUILD_EXPORTDNATYPES -DMKL_ILP64 -fopenmp)
add_definitions(-DBUILD_DNAADJUST_DLL -DBUILD_EXPORTDNATYPES)

include_directories (${PROJECT_SOURCE_DIR})

add_library (${PROJECT_NAME} SHARED
             ${CMAKE_SOURCE_DIR}/include/io/dnaioadj.cpp
             ${CMAKE_SOURCE_DIR}/include/io/dnaioaml.cpp
             ${CMAKE_SOURCE_DIR}/include/io/asl_file_loader.cpp
             ${CMAKE_SOURCE_DIR}/include/io/dnaiobase.cpp
             ${CMAKE_SOURCE_DIR}/include/io/bms_file_loader.cpp
             ${CMAKE_SOURCE_DIR}/include/io/bst_file_loader.cpp
             ${CMAKE_SOURCE_DIR}/include/io/map_file_loader.cpp
             ${CMAKE_SOURCE_DIR}/include/io/dnaioseg.cpp
             ${CMAKE_SOURCE_DIR}/include/io/dnaiotbu.cpp
             ${CMAKE_SOURCE_DIR}/include/io/dnaiosnxwrite.cpp
             ${CMAKE_SOURCE_DIR}/include/parameters/dnadatum.cpp
             ${CMAKE_SOURCE_DIR}/include/parameters/dnaellipsoid.cpp
             ${CMAKE_SOURCE_DIR}/include/parameters/dnaprojection.cpp
             ${CMAKE_SOURCE_DIR}/include/functions/dnastringfuncs.cpp
             ${CMAKE_SOURCE_DIR}/include/math/dnamatrix_contiguous.cpp
             ${CMAKE_SOURCE_DIR}/include/measurement_types/dnagpspoint.cpp
             ${CMAKE_SOURCE_DIR}/include/measurement_types/dnameasurement.cpp
             ${CMAKE_SOURCE_DIR}/include/measurement_types/dnastation.cpp
             ${CMAKE_SOURCE_DIR}/include/measurement_types/dnamsrtally.cpp
             ${CMAKE_SOURCE_DIR}/include/memory/dnafile_mapping.cpp
             network_data_loader.cpp
                          measurement_processor.cpp
             dnaadjust-stage.cpp
             dnaadjust.cpp
             ${CMAKE_SOURCE_DIR}/dynadjust.rc)

target_link_libraries (${PROJECT_NAME} ${DNA_LIBRARIES})

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
