//============================================================================
// Name         : measurement_processor.hpp
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
// Description  : Measurement Processor for DynAdjust Network Adjustment
//============================================================================

#ifndef MEASUREMENT_PROCESSOR_HPP_
#define MEASUREMENT_PROCESSOR_HPP_

#include <optional>

#include <include/config/dnatypes.hpp>
#include <include/measurement_types/dnameasurement.hpp>

namespace dynadjust {
namespace networkadjust {
namespace processors {

enum class AdjustmentMode { Simultaneous, Phased };

struct MeasurementCounts {
  UINT32 measurement_count = 0;
  UINT32 measurement_variance_count = 0;
  UINT32 measurement_params = 0;
};

class MeasurementProcessor {
public:
  explicit MeasurementProcessor(AdjustmentMode mode);
  ~MeasurementProcessor() = default;

  MeasurementProcessor(const MeasurementProcessor &) = delete;
  MeasurementProcessor &operator=(const MeasurementProcessor &) = delete;
  MeasurementProcessor(MeasurementProcessor &&) = default;
  MeasurementProcessor &operator=(MeasurementProcessor &&) = default;

  std::optional<UINT32> processForMode(const measurements::vmsr_t &bmsBinaryRecords,
                      UINT32 bmsr_count, vvUINT32 &v_CML,
                      MeasurementCounts &counts);

private:
  std::optional<UINT32> processSimultaneous(const measurements::vmsr_t &bmsBinaryRecords,
                           UINT32 bmsr_count, vvUINT32 &v_CML,
                           MeasurementCounts &counts);

  AdjustmentMode mode_;
};

} // namespace processors
} // namespace networkadjust
} // namespace dynadjust

#endif // MEASUREMENT_PROCESSOR_HPP_
