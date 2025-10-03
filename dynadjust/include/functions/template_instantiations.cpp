//============================================================================
// Name         : template_instantiations.cpp
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
// Description  : Explicit template instantiations for commonly used templates
//============================================================================

#include <include/functions/dnatemplatestnmsrfuncs.hpp>
#include <include/functions/dnatemplatefuncs.hpp>
#include <include/functions/dnatemplategeodesyfuncs.hpp>
#include <include/functions/dnatemplatematrixfuncs.hpp>

// Include necessary types
#include <include/config/dnatypes.hpp>
#include <string>
#include <cstdint>
#include <vector>

using namespace dynadjust::measurements;

// Explicit instantiations for dnatemplatestnmsrfuncs.hpp functions
// These are the most commonly used types based on codebase analysis

// GetGXMsrStations instantiations
// Note: Only std::string version is valid since GetFirst()/GetTarget() return strings
template void GetGXMsrStations<std::string>(std::vector<CDnaGpsBaseline>*, std::vector<std::string>&);

// GetMsrStations instantiations
// Note: The second parameter is an index, so only numeric types make sense
template void GetMsrStations<uint32_t>(const vmsr_t&, const uint32_t&, std::vector<uint32_t>&);
template void GetMsrStations<size_t>(const vmsr_t&, const size_t&, std::vector<size_t>&);

// GetMsrIndices instantiations
template void GetMsrIndices<uint32_t>(const vmsr_t&, const uint32_t&, std::vector<uint32_t>&);
template void GetMsrIndices<size_t>(const vmsr_t&, const size_t&, std::vector<size_t>&);

// Explicit instantiations for dnatemplatefuncs.hpp functions

// strip_duplicates instantiations (commonly used with vectors)
template void strip_duplicates<std::vector<std::string>>(std::vector<std::string>&);
template void strip_duplicates<std::vector<uint32_t>>(std::vector<uint32_t>&);
template void strip_duplicates<std::vector<size_t>>(std::vector<size_t>&);
template void strip_duplicates<std::vector<int>>(std::vector<int>&);

// isOdd instantiations
template bool isOdd<int>(int);
template bool isOdd<unsigned int>(unsigned int);
template bool isOdd<size_t>(size_t);
// Note: uint32_t is the same as unsigned int on most platforms, so not instantiating separately

// ===================================================================
// Explicit instantiations for dnatemplategeodesyfuncs.hpp functions
// These are heavily used throughout the codebase for geodetic calculations
// ===================================================================

// Note: Only instantiate templates that actually exist in the header files
// The following are the most commonly used geodetic functions

// GeoToCart - Converting geographic coordinates to Cartesian
// This function is defined in dnatemplategeodesyfuncs.hpp
template void GeoToCart<double>(const double&, const double&, const double&,
    double*, double*, double*, const CDnaEllipsoid*);

// Other geodetic helper functions
template double primeVertical<double>(const CDnaEllipsoid*, const double&);
template double primeMeridian<double>(const CDnaEllipsoid*, const double&);
template void primeVerticalandMeridian<double>(const CDnaEllipsoid*, const double&, double&, double&);
template double averageRadiusofCurvature<double>(const CDnaEllipsoid*, const double&);

// ===================================================================
// Explicit instantiations for dnatemplatematrixfuncs.hpp functions
// Matrix operations are heavily used in adjustment calculations
// ===================================================================

// Note: Add explicit instantiations for matrix functions here if they exist

// Add more instantiations as needed based on profiling data