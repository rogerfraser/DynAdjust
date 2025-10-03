//============================================================================
// Name         : file_metadata.hpp
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
// Description  : Lightweight file metadata structure
//============================================================================

#ifndef DYNADJUST_NETWORK_IO_FILE_METADATA_HPP
#define DYNADJUST_NETWORK_IO_FILE_METADATA_HPP

/// \cond
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
/// \endcond

namespace dynadjust {
namespace networkadjust {
namespace io {

struct FileMeta {
  std::filesystem::path filename;
  std::uintmax_t file_size = 0;
  std::uint64_t record_count = 0;
  std::uint32_t version = 0;
  std::chrono::system_clock::time_point created_time;
  std::string epsg_code;
  std::string coordinate_system;

  bool operator==(const FileMeta &other) const noexcept {
    return filename == other.filename && file_size == other.file_size &&
           record_count == other.record_count && version == other.version &&
           epsg_code == other.epsg_code &&
           coordinate_system == other.coordinate_system;
  }
};

} // namespace io
} // namespace networkadjust
} // namespace dynadjust

#endif // DYNADJUST_NETWORK_IO_FILE_METADATA_HPP
