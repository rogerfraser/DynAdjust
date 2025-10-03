//============================================================================
// Name         : dnatypes-fwd.hpp
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
// Description  : Forward declarations for DynAdjust data types
//============================================================================

#pragma once
#ifndef DNATYPES_FWD_H_
#define DNATYPES_FWD_H_

#include <cstdint>
#include <vector>
#include <string>
#include <utility>  // For std::pair used in typedefs below

// Basic type definitions that don't require full headers
typedef unsigned int	UINT32, *PUINT32;
typedef const PUINT32	PUINT32_const;
typedef unsigned short	UINT16, *PUINT16;
typedef uint32_t        index_t;

// Forward declare STL-based types without full definitions
typedef std::vector<char> vchar;
typedef std::vector<double> vdouble;
typedef std::vector<bool> vbool;
typedef std::vector<std::string> vstring, *pvstring;
typedef std::vector<UINT32> vUINT32, *pvUINT32;
typedef std::vector<vUINT32> vvUINT32, *pvvUINT32;

// Additional typedefs needed by dnaoptions.hpp
typedef std::pair<std::string, std::string> string_string_pair;
typedef std::vector<string_string_pair> v_string_string_pair, *pv_string_string_pair;
typedef std::pair<double, double> doubledouble_pair;
typedef std::pair<string_string_pair, doubledouble_pair> stringstring_doubledouble_pair;

// Forward declarations for structs
// Note: These template structs are defined in dnatypes-structs.hpp
// They cannot be forward declared as simple structs since they are templates
// struct stn_appearance_t;  // Template in dnatypes-structs.hpp
// struct stn_block_map_t;   // Template in dnatypes-structs.hpp
// struct freestnpair_t;     // Template in dnatypes-structs.hpp
// struct amlpair_t;         // Template in dnatypes-structs.hpp
// struct sequential_adj_t;  // Template in dnatypes-structs.hpp

// Constants that are frequently used
#ifndef INT_MIN
#define INT_MIN -2147483648
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef DNATYPES_H_
const char LOWER_TRIANGLE = 'L';
const char UPPER_TRIANGLE = 'U';

const UINT16 VALID_STATION = 1;
const UINT16 INVALID_STATION = 0;
#endif

#endif // DNATYPES_FWD_H_
