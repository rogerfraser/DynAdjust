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
    // TODO For now, just return nullopt for phased mode and fallback to dna_adjust
    // code. This will be implemented later.
    return std::nullopt;
  default:
    return std::nullopt;
  }
}

std::optional<UINT32> MeasurementProcessor::ProcessSimultaneous(
    const measurements::vmsr_t &bmsBinaryRecords, UINT32 bmsr_count,
    vvUINT32 &v_CML, MeasurementCounts &counts) {

  v_CML.clear();
  v_CML.push_back(vUINT32()); 

  counts.measurement_count = 0;
  counts.measurement_variance_count = 0;

  
  UINT32 m = 0, clusterID = 0;
  UINT16 axis = 0;  // Shared axis counter for all GPS measurements (G, X, Y)
  bool isNewCluster = false;
  
  UINT32 g_count = 0, x_count = 0, y_count = 0, d_count = 0, d_sets = 0;
  UINT32 other_count = 0;
  UINT32 ignored_count = 0, covariance_count = 0;
  
  // Process measurements and count them

  for (auto _it_msr = bmsBinaryRecords.begin();
       _it_msr != bmsBinaryRecords.end(); ++_it_msr) {
    // Don't include ignored measurements
    if (_it_msr->ignore) {
      ignored_count++;
      continue;
    }

    // Don't include covariance terms
    if (_it_msr->measStart > zMeas) {
      covariance_count++;
      continue;
    }

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
        d_sets++;
      }
      // Count the angles 
      if (_it_msr->vectorCount2 > 0) {
        d_count += _it_msr->vectorCount2 - 1;
        counts.measurement_count += _it_msr->vectorCount2 - 1;
        counts.measurement_variance_count += _it_msr->vectorCount2 - 1;
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
      // Handle GPS cluster measurements - only add first in cluster to v_CML
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

    // Count all GPS measurements (G, X, Y) regardless of cluster
    switch (_it_msr->measType) {
    case 'G':
      g_count++;
      counts.measurement_count++;
      counts.measurement_variance_count += 1 + axis++;
      if (axis > 2)
        axis = 0;
      continue;
    case 'X':
      x_count++;
      counts.measurement_count++;
      counts.measurement_variance_count += 1 + axis++;
      if (axis > 2)
        axis = 0;
      continue;
    case 'Y':
      y_count++;
      counts.measurement_count++;
      counts.measurement_variance_count += 1 + axis++;
      if (axis > 2)
        axis = 0;
      continue;
    }

    other_count++;
    counts.measurement_count++;
    counts.measurement_variance_count++;
  }

  counts.measurement_params = counts.measurement_count;

  return m;
}

} // namespace processors
} // namespace networkadjust
} // namespace dynadjust
