// precompile.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

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
#include <include/config/dnaconsts-interface.hpp>
#include <include/config/dnaprojectfile.hpp>

#include <include/exception/dnaexception.hpp>

#include <include/io/asl_file.hpp>
#include <include/io/aml_file.hpp>
#include <include/io/bst_file.hpp>
#include <include/io/bms_file.hpp>
#include <include/io/map_file.hpp>
#include <include/io/seg_file.hpp>

#include <include/functions/dnatemplatefuncs.hpp>
#include <include/functions/dnatemplatestnmsrfuncs.hpp>
#include <include/functions/dnaiostreamfuncs.hpp>
#include <include/functions/dnastringfuncs.hpp>
#include <include/functions/dnafilepathfuncs.hpp>
#include <include/functions/dnatemplatedatetimefuncs.hpp>

#include <dynadjust/dnasegment/dnasegment.hpp>


// NOTE: Module-specific headers should NOT be in PCH
