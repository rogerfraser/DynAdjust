//============================================================================
// Name         : binary_measurement_loader.hpp
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
// Description  : Binary Measurement Loader for DynAdjust Network Adjustment
//============================================================================

#ifndef BINARY_MEASUREMENT_LOADER_HPP_
#define BINARY_MEASUREMENT_LOADER_HPP_

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <include/config/dnatypes.hpp>
#include <include/io/dnaiobms.hpp>

using namespace dynadjust::iostreams;
using namespace dynadjust::measurements;

namespace dynadjust {
namespace networkadjust {
namespace loaders {

class BinaryMeasurementLoader {
public:
  explicit BinaryMeasurementLoader(std::string_view filename);
  ~BinaryMeasurementLoader() = default;

  BinaryMeasurementLoader(const BinaryMeasurementLoader &) = delete;
  BinaryMeasurementLoader &operator=(const BinaryMeasurementLoader &) = delete;
  BinaryMeasurementLoader(BinaryMeasurementLoader &&) = default;
  BinaryMeasurementLoader &operator=(BinaryMeasurementLoader &&) = default;

  std::optional<UINT32> load(vmsr_t *bmsBinaryRecords, binary_file_meta_t &bms_meta);

private:
  std::string filename_;
  std::unique_ptr<dna_io_bms> bms_loader_;
};

} // namespace loaders
} // namespace networkadjust
} // namespace dynadjust

#endif // BINARY_MEASUREMENT_LOADER_HPP_
