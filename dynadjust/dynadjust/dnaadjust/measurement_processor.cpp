//============================================================================
// Name         : measurement_processor.cpp
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
// Description  : Measurement Processor implementation
//============================================================================

#include "measurement_processor.hpp"
#include <include/config/dnatypes.hpp>

using namespace dynadjust::measurements;

namespace dynadjust {
namespace networkadjust {
namespace processors {

MeasurementProcessor::MeasurementProcessor(AdjustmentMode mode) : mode_(mode) {}

std::optional<UINT32> MeasurementProcessor::ProcessForMode(
    const measurements::vmsr_t &bmsBinaryRecords, UINT32 bmsr_count,
    vvUINT32 &v_CML, MeasurementCounts &counts) {
  switch (mode_) {
  case AdjustmentMode::Simultaneous:
    return ProcessSimultaneous(bmsBinaryRecords, bmsr_count, v_CML, counts);
  case AdjustmentMode::Phased:
    // For now, just return nullopt for phased mode
    return std::nullopt;
  default:
    return std::nullopt;
  }
}

std::optional<UINT32> MeasurementProcessor::ProcessSimultaneous(
    const measurements::vmsr_t &bmsBinaryRecords, UINT32 bmsr_count,
    vvUINT32 &v_CML, MeasurementCounts &counts) {
  // Basic stub implementation - would need to be completed
  // with the actual measurement processing logic

  v_CML.clear();
  v_CML.push_back(vUINT32());  // Start with empty vector, we'll resize later

  counts.measurement_count = 0;
  counts.measurement_variance_count = 0;

  UINT32 m = 0, clusterID = 0;
  UINT16 axis = 0;
  bool isNewCluster = false;

  for (auto _it_msr = bmsBinaryRecords.begin();
       _it_msr != bmsBinaryRecords.end(); ++_it_msr) {
    // Don't include ignored measurements
    if (_it_msr->ignore)
      continue;

    // Don't include covariance terms
    if (_it_msr->measStart > zMeas)
      continue;

    // Check if this is a new GPS cluster
    isNewCluster = false;
    if (_it_msr->measType == 'X' || _it_msr->measType == 'Y') {
      if (clusterID != _it_msr->clusterID) {
        isNewCluster = true;
        clusterID = _it_msr->clusterID;
      }
    }

    // Handle different measurement types for counting
    if (_it_msr->measType == 'D') {
      // Direction sets: only add the RO (reference orientation) to v_CML
      if (_it_msr->vectorCount1 >= 1) {
        v_CML.at(0).push_back(static_cast<UINT32>(
            std::distance(bmsBinaryRecords.begin(), _it_msr)));
        m++;
      }
      // Count the RO plus all target directions
      if (_it_msr->vectorCount2 > 0) {
        counts.measurement_count += _it_msr->vectorCount2;
        counts.measurement_variance_count += _it_msr->vectorCount2;
      } else if (_it_msr->vectorCount1 >= 1) {
        // Just the RO, no targets
        counts.measurement_count += 1;
        counts.measurement_variance_count += 1;
      }
      continue;
    }

    // Add to v_CML for other measurement types
    switch (_it_msr->measType) {
    case 'Y':
    case 'X':
      // Handle GPS cluster measurements - only add first in cluster
      if (isNewCluster) {
        v_CML.at(0).push_back(static_cast<UINT32>(
            std::distance(bmsBinaryRecords.begin(), _it_msr)));
        m++;  // Only increment m when we actually add to v_CML
      }
      break;
    default:
      v_CML.at(0).push_back(static_cast<UINT32>(
          std::distance(bmsBinaryRecords.begin(), _it_msr)));
      m++;  // Only increment m when we actually add to v_CML
    }

    switch (_it_msr->measType) {
    case 'G':
      // GPS baseline measurements expand to 3 components (X, Y, Z)
      counts.measurement_count += 3;
      counts.measurement_variance_count += 6;  // Upper triangular covariance matrix
      continue;
    case 'X':
    case 'Y':
      // GPS cluster measurements - count each measurement record
      // The binary file contains individual X/Y records for each baseline component
      counts.measurement_count++;
      counts.measurement_variance_count += 1 + axis++;
      
      if (axis > 2)
        axis = 0;
      
      continue;
    }

    counts.measurement_count++;
    counts.measurement_variance_count++;
  }

  // No need to resize since we're using push_back

  counts.measurement_params = counts.measurement_count;

  return m;
}

} // namespace processors
} // namespace networkadjust
} // namespace dynadjust
