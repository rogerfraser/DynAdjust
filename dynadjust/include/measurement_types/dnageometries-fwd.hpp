//============================================================================
// Name         : dnageometries-fwd.hpp
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
// Description  : Forward declarations for geometry types
//============================================================================

#ifndef DNAGEOMETRIES_FWD_H_
#define DNAGEOMETRIES_FWD_H_

namespace dynadjust {
namespace measurements {

// Forward declare geometry structures
struct coord_t;
struct coord3_t;
struct point2d;
struct point3d;
struct circle;
struct station_id;

// Forward declare comparison functors
struct CompareStationID;

} // namespace measurements
} // namespace dynadjust

#endif // DNAGEOMETRIES_FWD_H_