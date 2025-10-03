//============================================================================
// Name         : dnaadjust_fwd.hpp
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
// Description  : Forward declarations for DynAdjust Network Adjustment library
//============================================================================

#ifndef DNAADJUST_FWD_H_
#define DNAADJUST_FWD_H_

namespace dynadjust {
namespace networkadjust {

// Forward declarations
class dna_adjust;
class DynAdjustPrinter;
class NetworkDataLoader;

// Thread helper classes
class adjust_prepare_thread;
class adjust_process_prepare_thread;
class adjust_forward_thread;
class adjust_reverse_thread;
class adjust_combine_thread;
class adjust_process_combine_thread;

} // namespace networkadjust
} // namespace dynadjust

#endif // DNAADJUST_FWD_H_
