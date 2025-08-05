//============================================================================
// Name         : associated_station_list_loader.hpp
// Copyright    : Copyright 2025 Geoscience Australia
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
// Description  : DynAdjust associated station file io operations
//============================================================================

#ifndef DYNADJUST_ASL_FILE_LOADER_H_
#define DYNADJUST_ASL_FILE_LOADER_H_

#if defined(_MSC_VER)
  #if defined(LIST_INCLUDES_ON_BUILD)
    #pragma message("  " __FILE__)
  #endif
#endif

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <include/functions/dnaintegermanipfuncs.hpp>
#include <include/io/dynadjust_file.hpp>
#include <include/measurement_types/dnastation.hpp>

namespace dynadjust {
namespace iostreams {

class AslFileLoader : public DynadjustFile {
 public:
  AslFileLoader() = default;
  AslFileLoader(const AslFileLoader& asl)
      : DynadjustFile(asl) {}
  virtual ~AslFileLoader() = default;

  AslFileLoader& operator=(const AslFileLoader& rhs);

  std::uint64_t LoadFile(const std::string& asl_filename,
                         measurements::vASL* binary_asl,
                         vUINT32* free_stn);

  std::optional<std::uint64_t> LoadWithOptional(const std::string& asl_filename,
                                                 measurements::vASL* binary_asl,
                                                 vUINT32* free_stn);

  void WriteFile(const std::string& asl_filename,
                 measurements::pvASLPtr binary_asl);

  void WriteTextFile(const std::string& asl_filename,
                     measurements::pvASLPtr binary_asl,
                     measurements::vdnaStnPtr* stations);

 protected:
};

}  // namespace iostreams
}  // namespace dynadjust

#endif  // DYNADJUST_ASL_FILE_LOADER_H_