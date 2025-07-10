//============================================================================
// Name         : io_exceptions.hpp
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
// Description  : Standard error handling for network I/O operations
//============================================================================

#ifndef DYNADJUST_NETWORK_IO_EXCEPTIONS_HPP
#define DYNADJUST_NETWORK_IO_EXCEPTIONS_HPP

#include <filesystem>
#include <stdexcept>
#include <string>
#include <system_error>

namespace dynadjust {
namespace networkadjust {
namespace io {

enum class IoErrorCode {
  file_not_found = 1,
  file_read_error,
  file_write_error,
  invalid_format,
  corrupted_data,
  insufficient_data,
  version_mismatch
};

// Forward declaration
const std::error_category &io_error_category();

class IoError : public std::system_error {
public:
  IoError(IoErrorCode code, const std::string &message)
      : std::system_error(static_cast<int>(code), io_error_category(),
                          message) {}

  IoError(IoErrorCode code, const std::string &message,
          const std::filesystem::path &filename)
      : std::system_error(static_cast<int>(code), io_error_category(),
                          message + " (file: " + filename.string() + ")") {}
};

class IoErrorCategory : public std::error_category {
public:
  const char *name() const noexcept override { return "dynadjust_io"; }

  std::string message(int condition) const override {
    switch (static_cast<IoErrorCode>(condition)) {
    case IoErrorCode::file_not_found:
      return "File not found";
    case IoErrorCode::file_read_error:
      return "File read error";
    case IoErrorCode::file_write_error:
      return "File write error";
    case IoErrorCode::invalid_format:
      return "Invalid file format";
    case IoErrorCode::corrupted_data:
      return "Corrupted data detected";
    case IoErrorCode::insufficient_data:
      return "Insufficient data in file";
    case IoErrorCode::version_mismatch:
      return "File version mismatch";
    default:
      return "Unknown I/O error";
    }
  }
};

inline const std::error_category &io_error_category() {
  static IoErrorCategory instance;
  return instance;
}

template <typename Exception>
void rethrow_with_context(const Exception &e, const std::string &operation,
                          const std::filesystem::path &filename) {
  try {
    throw;
  } catch (...) {
    std::string context = operation + " failed for file: " + filename.string();
    std::throw_with_nested(std::runtime_error(context));
  }
}

} // namespace io
} // namespace networkadjust
} // namespace dynadjust

#endif // DYNADJUST_NETWORK_IO_EXCEPTIONS_HPP
