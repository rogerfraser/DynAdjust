//============================================================================
// Name         : dnaprocessfuncs.hpp
// Author       : Roger Fraser
// Contributors :
// Version      : 1.00
// Copyright    : Copyright 2017 Geoscience Australia
//
//                Licensed under the Apache License, Version 2.0 (the
//                "License"); you may not use this file except in compliance
//                with the License. You may obtain a copy of the License at
//
//                http ://www.apache.org/licenses/LICENSE-2.0
//
//                Unless required by applicable law or agreed to in writing,
//                software distributed under the License is distributed on an
//                "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
//                either express or implied. See the License for the specific
//                language governing permissions and limitations under the
//                License.
//
// Description  : Common process and fork functions
//============================================================================
#ifndef DNAPROCESSFUNCS_HPP_
#define DNAPROCESSFUNCS_HPP_

#if defined(_MSC_VER)
#if defined(LIST_INCLUDES_ON_BUILD)
#pragma message("  " __FILE__)
#endif
#endif

/// \cond
#include <string>
/// \endcond

#if defined(_WIN32) || defined(__WIN32__)
// prevent conflict with std::min(...) std::max(...)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

bool run_command(const std::string& executable_path, bool quiet = false);

class dna_create_threaded_process {
  public:
    dna_create_threaded_process(const std::string& command_path,
                                bool quiet = false)
        : _command_path(command_path), _quiet(quiet) {}

    void operator()() { run_command(_command_path, _quiet); }

  private:
    std::string _command_path;
    bool _quiet;
};

#endif // DNAPROCESSFUNCS_HPP_
