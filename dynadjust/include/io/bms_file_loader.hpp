//============================================================================
// Name         : bms_file_loader.hpp
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
// Description  : DynAdjust binary measurement file io operations
//============================================================================

#ifndef DYNADJUST_BMS_FILE_LOADER_H_
#define DYNADJUST_BMS_FILE_LOADER_H_

#if defined(_MSC_VER)
  #if defined(LIST_INCLUDES_ON_BUILD)
    #pragma message("  " __FILE__)
  #endif
#endif

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <include/config/dnatypes.hpp>
#include <include/io/dynadjust_file.hpp>
#include <include/measurement_types/dnameasurement.hpp>

namespace dynadjust {
namespace iostreams {

class BmsFileLoader : public DynadjustFile {
 public:
  BmsFileLoader() = default;
  BmsFileLoader(const BmsFileLoader& bms) : DynadjustFile(bms) {}
  virtual ~BmsFileLoader() = default;

  BmsFileLoader& operator=(const BmsFileLoader& rhs);

  std::uint64_t CreateMsrInputFileMeta(vifm_t& vinput_file_meta,
                                       input_file_meta_t** input_file_meta);

  void LoadFileMeta(const std::string& bms_filename,
                    binary_file_meta_t& bms_meta);

  std::uint64_t LoadFile(const std::string& bms_filename,
                         measurements::pvmsr_t vbinary_msr,
                         binary_file_meta_t& bms_meta);

  std::optional<std::uint64_t> LoadWithOptional(const std::string& bms_filename,
                                                 measurements::pvmsr_t vbinary_msr,
                                                 binary_file_meta_t& bms_meta);

  void WriteFile(const std::string& bms_filename,
                 measurements::pvmsr_t vbinary_msr,
                 binary_file_meta_t& bms_meta);

  void WriteFile(const std::string& bms_filename,
                 measurements::vdnaMsrPtr* vMeasurements,
                 binary_file_meta_t& bms_meta);

 protected:
};

}  // namespace iostreams
}  // namespace dynadjust

#endif  // DYNADJUST_BMS_FILE_LOADER_H_