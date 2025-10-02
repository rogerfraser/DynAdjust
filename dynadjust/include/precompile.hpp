//============================================================================
// Name         : precompile.hpp
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
// Description  : Precompiled header for DynAdjust - includes commonly used headers
//============================================================================

#ifndef DYNADJUST_PRECOMPILE_H_
#define DYNADJUST_PRECOMPILE_H_

// Standard library headers (most frequently used based on analysis)
// These are included in order of frequency of use across the codebase
#include <string>        // Used in 57 files
#include <sstream>       // Used in 41 files
#include <vector>        // Used in 39 files
#include <iostream>      // Used in 31 files
#include <iomanip>       // Used in 29 files
#include <memory>        // Used in 26 files
#include <algorithm>     // Used in 23 files
#include <fstream>       // Used in 22 files
#include <filesystem>    // Used in 20 files
#include <stdexcept>     // Used in 13 files
#include <cstdint>       // Used in 14 files

// Additional commonly used STL headers
#include <functional>
#include <optional>
#include <map>
#include <queue>

// Math headers (used in 17 files)
#include <math.h>
#include <cmath>

// C headers
#include <stdio.h>
#include <cstring>

// Boost headers (commonly used throughout the codebase)
// Including these in PCH significantly speeds up compilation
#ifdef USE_BOOST
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#endif

// Common project headers (use forward declarations where possible)
#include <include/config/dnatypes-fwd.hpp>  // Forward declarations only

// These lightweight headers are commonly used and rarely change
#include <include/config/dnaconsts.hpp>      // Constants
#include <include/config/dnaversion.hpp>     // Version info
#include <include/config/dnaexports.hpp>     // Export macros

// Commonly used function headers (lightweight, rarely change)
#include <include/functions/dnastrutils.hpp>
#include <include/functions/dnastrmanipfuncs.hpp>
#include <include/functions/dnaiostreamfuncs.hpp>
#include <include/exception/dnaexception.hpp>

// Forward declarations for measurement types
#include <include/measurement_types/dnameasurement-fwd.hpp>
#include <include/measurement_types/dnastation-fwd.hpp>

#endif // DYNADJUST_PRECOMPILE_H_