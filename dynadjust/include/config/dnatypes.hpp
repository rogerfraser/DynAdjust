//============================================================================
// Name         : dnatypes.hpp
// Author       : Roger Fraser
// Contributors : Dale Roberts <dale.o.roberts@gmail.com>
//              : Dale Roberts
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
// Description  : DynAdjust data types include file
//============================================================================

#pragma once
#ifndef DNATYPES_H_
#define DNATYPES_H_

#if defined(_MSC_VER)
	#if defined(LIST_INCLUDES_ON_BUILD)
		#pragma message("  " __FILE__)
	#endif
#endif

// Include the split headers for better compilation performance
// These headers are organized by functionality to reduce dependencies

// 1. Basic types and constants (fundamental types, enums, constants)
#include <include/config/dnatypes-basic.hpp>

// 2. STL container typedefs (vectors, pairs, maps, etc.)
#include <include/config/dnatypes-containers.hpp>

// 3. Custom structures (application-specific structs and classes)
#include <include/config/dnatypes-structs.hpp>

// Note: For backward compatibility, all types are available through this header.
// However, for better compilation times, consider including only the specific
// headers you need:
//   - dnatypes-basic.hpp for basic types and enums
//   - dnatypes-containers.hpp for STL container typedefs
//   - dnatypes-structs.hpp for custom structures

#endif // DNATYPES_H_