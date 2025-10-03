//============================================================================
// Name         : map_file_loader.hpp
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
// Description  : DynAdjust station map file io operations
//============================================================================

#ifndef DYNADJUST_MAP_FILE_LOADER_H_
#define DYNADJUST_MAP_FILE_LOADER_H_

#if defined(_MSC_VER)
  #if defined(LIST_INCLUDES_ON_BUILD)
    #pragma message("  " __FILE__)
  #endif
#endif

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <include/config/dnatypes-fwd.hpp>
#include <include/io/dynadjust_file.hpp>

namespace dynadjust {
namespace iostreams {

class MapFileLoader : public DynadjustFile {
 public:
  MapFileLoader() = default;
  MapFileLoader(const MapFileLoader& map) : DynadjustFile(map) {}
  virtual ~MapFileLoader() = default;

  MapFileLoader& operator=(const MapFileLoader& rhs);

  std::uint64_t LoadFile(const std::string& map_filename,
                         pv_string_uint32_pair station_map);

  std::optional<std::uint64_t> LoadWithOptional(const std::string& map_filename,
                                                 pv_string_uint32_pair station_map);

  void WriteFile(const std::string& map_filename,
                 pv_string_uint32_pair station_map);

  void WriteTextFile(const std::string& map_filename,
                     pv_string_uint32_pair station_map);

 protected:
};

}  // namespace iostreams
}  // namespace dynadjust

#endif  // DYNADJUST_MAP_FILE_LOADER_H_