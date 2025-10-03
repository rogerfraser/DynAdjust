// precompile.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// See https://github.com/boostorg/process/issues/161
#define _WIN32_WINNT 0x0501

// Use the common precompiled header foundation
#include <include/pch-common.h>

// Boost headers needed by this module
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>

// Additional stable headers for this module
#include <include/config/dnaconsts-interface.hpp>
#include <include/config/dnaprojectfile.hpp>

// Platform-specific includes
#ifdef _WIN32
    // On Windows, PCH is more effective, include more headers
    #include <include/config/dnatypes.hpp>  // Full types on Windows

    // IO headers - consider moving to io-fwd.hpp later
    #include <include/io/bst_file.hpp>
    #include <include/io/bms_file.hpp>
    #include <include/io/map_file.hpp>
    #include <include/io/aml_file.hpp>
    #include <include/io/asl_file.hpp>
    #include <include/io/seg_file.hpp>
    #include <include/io/adj_file.hpp>

    // Parameters - stable headers
    #include <include/parameters/dnaepsg.hpp>
    #include <include/parameters/dnadatum.hpp>
#else
    // On macOS/Linux, use forward declarations for better incremental builds
    #include <include/measurement_types/dnameasurement-fwd.hpp>
    #include <include/measurement_types/dnastation-fwd.hpp>
#endif

// NOTE: Module-specific headers (dnaadjust.hpp, dnaadjustwrapper.hpp, etc.)
// should NEVER be in PCH as they change frequently
