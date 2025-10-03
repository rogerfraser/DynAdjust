// precompile.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// See https://github.com/boostorg/process/issues/161
#define _WIN32_WINNT 0x0501

#include <include/config/dnaconsts.hpp>
#include <include/config/dnaversion.hpp>

// Platform-specific type inclusion
#ifdef _WIN32
    #include <include/config/dnatypes.hpp>  // Full types on Windows for better PCH
#else
    #include <include/config/dnatypes-fwd.hpp>  // Forward declarations on Unix
#endif

#include <include/config/dnatypes-gui.hpp>
#include <include/config/dnaexports.hpp>
#include <include/config/dnaoptions-interface.hpp>
#include <include/config/dnaprojectfile.hpp>

#include <include/exception/dnaexception.hpp>

#include <include/functions/dnastringfuncs.hpp>
#include <include/functions/dnaprocessfuncs.hpp>
#include <include/functions/dnafilepathfuncs.hpp>

#include <dynadjust/dnaplot/dnaplot.hpp>


// NOTE: Module-specific headers should NOT be in PCH
