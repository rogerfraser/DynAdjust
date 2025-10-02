//============================================================================
// Name         : dnameasurement-fwd.hpp
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
// Description  : Forward declarations for measurement types
//============================================================================

#ifndef DNAMEASUREMENT_FWD_H_
#define DNAMEASUREMENT_FWD_H_

#include <vector>  // Required for vmsr_t and iterator typedefs

namespace dynadjust {
namespace measurements {

// Forward declare measurement classes
class CDnaAngle;
class CDnaAzimuth;
class CDnaCoordinate;
class CDnaDirection;
class CDnaDirectionSet;
class CDnaDistance;
class CDnaGpsBaseline;
class CDnaGpsBaselineCluster;
class CDnaGpsPoint;
class CDnaGpsPointCluster;
class CDnaHeight;
class CDnaHeightDifference;
class CDnaMeasurement;
class CDnaStation;

// Forward declare measurement container types
template<class T> class CompareClass;

// measurement_t is a typedef of struct msr_t, not a template
struct msr_t;
typedef struct msr_t measurement_t;

// Forward declare measurement vector types
typedef std::vector<measurement_t> vmsr_t;
typedef vmsr_t* pvmsr_t;
typedef vmsr_t::iterator it_vmsr_t;
typedef it_vmsr_t* pit_vmsr_t;
typedef vmsr_t::const_iterator it_vmsr_t_const;

} // namespace measurements
} // namespace dynadjust

#endif // DNAMEASUREMENT_FWD_H_