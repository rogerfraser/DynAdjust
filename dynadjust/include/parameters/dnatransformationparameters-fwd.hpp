//============================================================================
// Name         : dnatransformationparameters-fwd.hpp
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
// Description  : Forward declarations for transformation parameters
//============================================================================

#ifndef DNATRANSFORM_PARAM_FWD_H_
#define DNATRANSFORM_PARAM_FWD_H_

namespace dynadjust {
namespace datum_parameters {

// Forward declare enums
typedef enum
{
	__paramForward__ = 0,
	__paramReverse__ = 1
} transformationDirection;

typedef enum
{
	__static_to_static__ =   0,
	__static_to_dynamic__ =  1,
	__static_to_step__ =     2,
	__dynamic_to_step__ =    3,
	__dynamic_to_static__ =  4,
	__dynamic_to_dynamic__ = 5,
	__step_to_dynamic__ =    6,
	__step_to_static__ =     7,
	__plate_motion_model__ = 8
} transformationType;

typedef enum
{
	__frame_frame_same__ = 0,
	__frame_frame_diff__ = 1
} frameSimilarity;

// Forward declare transformation parameter classes
template<class T> class transformation_parameter;
template<class T> class transformation_parameter_set;

} // namespace datum_parameters
} // namespace dynadjust

#endif // DNATRANSFORM_PARAM_FWD_H_