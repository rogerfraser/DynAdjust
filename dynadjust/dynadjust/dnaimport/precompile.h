// precompile.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//


#pragma once

// Core configuration headers - always needed
#include <include/config/dnaconsts.hpp>
#include <include/config/dnaversion.hpp>
#include <include/config/dnaexports.hpp>
#include <include/config/dnatypes-fwd.hpp>  // Use forward declarations
#include <include/config/dnaversion-stream.hpp>
#include <include/exception/dnaexception.hpp>

// Platform-specific includes
#ifdef _WIN32
    // On Windows, PCH is more effective, include more headers
    #include <include/config/dnatypes.hpp>  // Full types on Windows

    // IO headers
    #include <include/io/dnaiodnatypes.hpp>
    #include <include/io/bst_file.hpp>
    #include <include/io/bms_file.hpp>
    #include <include/io/aml_file.hpp>
    #include <include/io/asl_file.hpp>
    #include <include/io/map_file.hpp>
    #include <include/io/seg_file.hpp>
    #include <include/io/dnaiosnx.hpp>
    #include <include/io/dnaioscalar.hpp>

    // Measurement types
    #include <include/measurement_types/dnameasurement_types.hpp>

    // Template function headers - beneficial on Windows
    #include <include/functions/dnatemplatefuncs.hpp>
    #include <include/functions/dnatemplatestnmsrfuncs.hpp>

    // Parameters
    #include <include/parameters/dnaepsg.hpp>
    #include <include/parameters/dnadatum.hpp>
#else
    // On macOS/Linux, minimize PCH content for better incremental builds
    // Use forward declarations where possible
    #include <include/measurement_types/dnameasurement-fwd.hpp>
    #include <include/measurement_types/dnastation-fwd.hpp>
    #include <include/measurement_types/dnageometries-fwd.hpp>

    // Only essential IO type definitions
    #include <include/io/dnaiodnatypes.hpp>
#endif

// Common lightweight function headers
#include <include/functions/dnaiostreamfuncs.hpp>
#include <include/functions/dnastringfuncs.hpp>
#include <include/functions/dnafilepathfuncs.hpp>

// Module-specific headers should not be in PCH
// Move these to individual source files that need them
// #include <dynadjust/dnaimport/dnaparser_pskel.hxx>
// #include <dynadjust/dnaimport/dnaparser_pimpl.hxx>
// #include <dynadjust/dnaimport/dnainterop.hpp>
