//============================================================================
// Name         : dnastation-fwd.hpp
// Author       : Roger Fraser
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
// Description  : Forward declarations for station types
//============================================================================

#ifndef DNASTATION_FWD_H_
#define DNASTATION_FWD_H_

#include <vector>
#include <memory>

namespace dynadjust {
namespace measurements {

// Forward declare station classes
class CDnaStation;
// CompareStationName is a template class defined in dnatemplatestnmsrfuncs.hpp
// Not forward declaring here to avoid ambiguity
// template<typename T, typename S> class CompareStationName;
class equal_station_name;

// DR: Renamed from vstn_t to avoid conflict with global ::vstn_t (vector<station_t>)
typedef std::vector<CDnaStation> vdnastn_t;
typedef std::shared_ptr<CDnaStation> stn_ptr;
typedef std::vector<stn_ptr> vstn_ptr_t;

} // namespace measurements
} // namespace dynadjust

#endif // DNASTATION_FWD_H_
