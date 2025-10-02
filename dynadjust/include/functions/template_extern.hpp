//============================================================================
// Name         : template_extern.hpp
// Author       : Dale Roberts (generated)
// Contributors : Dale Roberts <dale.o.roberts@gmail.com>
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
// Description  : Extern template declarations to prevent duplicate instantiations
//============================================================================

#ifndef DYNADJUST_TEMPLATE_EXTERN_HPP
#define DYNADJUST_TEMPLATE_EXTERN_HPP

#include <vector>
#include <string>
#include <cstdint>

// Include necessary type definitions
#include <include/config/dnatypes-fwd.hpp>
#include <include/measurement_types/dnameasurement.hpp>
#include <include/measurement_types/dnagpsbaseline.hpp>

using namespace dynadjust::measurements;

// Extern template declarations for dnatemplatestnmsrfuncs.hpp functions
// These prevent the compiler from instantiating these templates in every translation unit

// GetGXMsrStations extern declarations
// Note: Only std::string version is valid since GetFirst()/GetTarget() return strings
extern template void GetGXMsrStations<std::string>(std::vector<CDnaGpsBaseline>*, std::vector<std::string>&);

// GetMsrStations extern declarations
// Note: The second parameter is an index, so only numeric types make sense
extern template void GetMsrStations<uint32_t>(const vmsr_t&, const uint32_t&, std::vector<uint32_t>&);
extern template void GetMsrStations<size_t>(const vmsr_t&, const size_t&, std::vector<size_t>&);

// GetMsrIndices extern declarations
extern template void GetMsrIndices<uint32_t>(const vmsr_t&, const uint32_t&, std::vector<uint32_t>&);
extern template void GetMsrIndices<size_t>(const vmsr_t&, const size_t&, std::vector<size_t>&);

// Extern template declarations for dnatemplatefuncs.hpp functions

// strip_duplicates extern declarations
extern template void strip_duplicates<std::vector<std::string>>(std::vector<std::string>&);
extern template void strip_duplicates<std::vector<uint32_t>>(std::vector<uint32_t>&);
extern template void strip_duplicates<std::vector<size_t>>(std::vector<size_t>&);
extern template void strip_duplicates<std::vector<int>>(std::vector<int>&);

// isOdd extern declarations
extern template bool isOdd<int>(int);
extern template bool isOdd<unsigned int>(unsigned int);
extern template bool isOdd<size_t>(size_t);
// Note: uint32_t is the same as unsigned int on most platforms, so not declaring separately

// Usage:
// Include this header AFTER including the template headers in your .cpp files
// This will prevent the compiler from generating these instantiations again
// Example:
//   #include <include/functions/dnatemplatestnmsrfuncs.hpp>
//   #include <include/functions/dnatemplatefuncs.hpp>
//   #include <include/functions/template_extern.hpp>  // Include this last

#endif // DYNADJUST_TEMPLATE_EXTERN_HPP