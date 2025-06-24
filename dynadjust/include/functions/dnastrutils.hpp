//============================================================================
// Name         : dnastrutils.hpp
// Copyright    : Copyright 2025 Geoscience Australia
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
// Description  : String utility functions to replace boost::algorithm
//============================================================================

#ifndef DNASTRUTILS_HPP_
#define DNASTRUTILS_HPP_

#if defined(_MSC_VER)
	#if defined(LIST_INCLUDES_ON_BUILD) 
		#pragma message("  " __FILE__) 
	#endif
#endif

#include <string>
#include <algorithm>
#include <cctype>

namespace dynadjust {

// Case-insensitive character comparison
inline bool ichar_equals(char a, char b) {
    return std::tolower(static_cast<unsigned char>(a)) == 
           std::tolower(static_cast<unsigned char>(b));
}

// Case-insensitive string comparison
inline bool iequals(const std::string& a, const std::string& b) {
    return a.size() == b.size() && 
           std::equal(a.begin(), a.end(), b.begin(), ichar_equals);
}

// Case-sensitive string comparison (for consistency and to replace equals)
inline bool equals(const std::string& a, const std::string& b) {
    return a == b;
}

// Case-insensitive substring search (replaces icontains)
inline bool icontains(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) {
        return true;
    }
    if (haystack.size() < needle.size()) {
        return false;
    }
    
    auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        ichar_equals
    );
    
    return it != haystack.end();
}

} // namespace dynadjust

// Import into global namespace for easier migration
using dynadjust::iequals;
using dynadjust::equals;
using dynadjust::icontains;

#endif // DNASTRUTILS_HPP_
