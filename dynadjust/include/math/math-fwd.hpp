//============================================================================
// Name         : math-fwd.hpp
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
// Description  : Forward declarations for DynAdjust math classes
//============================================================================

#pragma once
#ifndef MATH_FWD_H_
#define MATH_FWD_H_

namespace dynadjust {
namespace math {

// Matrix classes
template<typename T> class matrix_2d;
template<typename T> class matrix_contiguous;

// Sparse matrix classes
class sparse_matrix;
class sparse_matrix_row;

// Statistical classes
class statistics;
class distribution;

// Geometric calculation classes
class geometry_calculator;
class coordinate_transformer;

}  // namespace math
}  // namespace dynadjust

#endif // MATH_FWD_H_