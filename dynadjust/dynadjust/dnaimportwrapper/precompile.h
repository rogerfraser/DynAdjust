// precompile.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Use the common precompiled header foundation
#include <include/pch-common.h>

// Boost headers needed by this module
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>

// Additional module-specific stable headers (minimal)
#include <include/config/dnaoptions-helper.hpp>
#include <include/config/dnaconsts-interface.hpp>
#include <include/config/dnaprojectfile.hpp>

// NOTE: Module-specific headers like dnainterop.hpp and dnaimportwrapper.hpp
// should NOT be in precompiled headers as they change frequently and create
// unnecessary dependencies. Include them directly in source files that need them.
