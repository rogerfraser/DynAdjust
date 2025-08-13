//============================================================================
// Name         : xerces_compat.hpp
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
// Description  : Maps current Xerces namespace for backward compatibility
//============================================================================

#ifndef XERCES_COMPAT_HPP
#define XERCES_COMPAT_HPP

#ifdef _WIN32

// Xerces 3.2+ uses XERCES_CPP_NAMESPACE directly without version suffix
// but the generated XSD code expects xercesc_3_1 namespace
#include <xercesc/util/XercesDefs.hpp>

// Create namespace alias to map the current Xerces namespace to 3.1 for compatibility
// with XSD-generated code that expects xercesc_3_1
namespace xercesc_3_1 = XERCES_CPP_NAMESPACE;

#endif // _WIN32

#endif // XERCES_COMPAT_HPP
