// precompile.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Use the common precompiled header foundation
#include <include/pch-common.h>

// Boost headers that might be needed by this module
#include <boost/program_options.hpp>

// Additional stable headers for this module
#include <include/config/dnaoptions.hpp>
#include <include/functions/dnastrmanipfuncs.hpp>
#include <include/parameters/dnaepsg.hpp>
#include <include/parameters/dnaellipsoid.hpp>
#include <include/parameters/dnaprojection.hpp>
#include <include/parameters/dnadatum.hpp>
#include <include/parameters/dnatransformationparameters.hpp>
#include <include/math/dnamatrix_contiguous.hpp>

// NOTE: Module-specific headers (dnareftran.hpp, dnareftranwrapper.hpp)
// should NOT be in precompiled headers as they change frequently
