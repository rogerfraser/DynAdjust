//============================================================================
// Name         : measurement_processor.cpp
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
// Description  : Measurement Processor implementation
//============================================================================

#include "measurement_processor.hpp"
#include <include/config/dnatypes.hpp>

using namespace dynadjust::measurements;

namespace dynadjust {
namespace networkadjust {
namespace processors {

MeasurementProcessor::MeasurementProcessor(AdjustmentMode mode) : mode_(mode) {}

auto MeasurementProcessor::processForMode(
    const measurements::vmsr_t &bmsBinaryRecords, UINT32 bmsr_count,
    vvUINT32 &v_CML, MeasurementCounts &counts) -> std::optional<UINT32> {
  switch (mode_) {
  case AdjustmentMode::Simultaneous:
    return processSimultaneous(bmsBinaryRecords, bmsr_count, v_CML, counts);
  case AdjustmentMode::Phased:
    // For now, just return nullopt for phased mode
    return std::nullopt;
  default:
    return std::nullopt;
  }
}

auto MeasurementProcessor::processSimultaneous(
    const measurements::vmsr_t &bmsBinaryRecords, UINT32 bmsr_count,
    vvUINT32 &v_CML, MeasurementCounts &counts) -> std::optional<UINT32> {
  // Basic stub implementation - would need to be completed
  // with the actual measurement processing logic

  v_CML.clear();
  v_CML.push_back(vUINT32(bmsr_count));

  counts.measurement_count = 0;
  counts.measurement_variance_count = 0;

  UINT32 m = 0, clusterID = 0;
  UINT16 axis = 0;

  for (auto _it_msr = bmsBinaryRecords.begin();
       _it_msr != bmsBinaryRecords.end(); ++_it_msr, ++m) {
    // Don't include ignored measurements
    if (_it_msr->ignore)
      continue;

    // Don't include covariance terms
    if (_it_msr->measStart > zMeas)
      continue;

    switch (_it_msr->measType) {
    case 'Y':
    case 'X':
      // Handle GPS cluster measurements
      if (clusterID != _it_msr->clusterID) {
        v_CML.at(0).at(m) = static_cast<UINT32>(
            std::distance(bmsBinaryRecords.begin(), _it_msr));
        clusterID = _it_msr->clusterID;
      }
      break;
    default:
      v_CML.at(0).at(m) =
          static_cast<UINT32>(std::distance(bmsBinaryRecords.begin(), _it_msr));
    }

    // Handle different measurement types for counting
    if (_it_msr->measType == 'D') {
      if (_it_msr->vectorCount2 > 0) {
        counts.measurement_count += _it_msr->vectorCount2 - 1;
        counts.measurement_variance_count += _it_msr->vectorCount2 - 1;
      }
      continue;
    }

    switch (_it_msr->measType) {
    case 'G':
    case 'X':
    case 'Y':
      counts.measurement_count++;
      counts.measurement_variance_count += 1 + axis++;
      if (axis > 2)
        axis = 0;
      continue;
    }

    counts.measurement_count++;
    counts.measurement_variance_count++;
  }

  // Resize to actual measurement count
  if (m != bmsr_count) {
    v_CML.at(0).resize(m);
  }

  counts.measurement_params = counts.measurement_count;

  return m;
}

} // namespace processors
} // namespace networkadjust
} // namespace dynadjust
