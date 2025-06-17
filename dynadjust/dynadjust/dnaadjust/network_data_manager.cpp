//============================================================================
// Name         : network_data_manager.cpp
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
// Description  : Network Data Manager implementation
//============================================================================

#include "network_data_manager.hpp"
#include <sstream>

namespace dynadjust {
namespace networkadjust {

NetworkDataManager::NetworkDataManager(const project_settings &settings)
    : settings_(settings),
      station_loader_(
          std::make_unique<loaders::BinaryStationLoader>(settings.a.bst_file)),
      asl_loader_(std::make_unique<loaders::AssociatedStationLoader>(
          settings.s.asl_file)),
      measurement_loader_(std::make_unique<loaders::BinaryMeasurementLoader>(
          settings.a.bms_file)),
      measurement_processor_(std::make_unique<processors::MeasurementProcessor>(
          settings.a.adjust_mode == SimultaneousMode
              ? processors::AdjustmentMode::Simultaneous
              : processors::AdjustmentMode::Phased)) {}

auto NetworkDataManager::loadNetworkFiles(
    vstn_t *bstBinaryRecords, binary_file_meta_t &bst_meta, vASL *vAssocStnList,
    vmsr_t *bmsBinaryRecords, binary_file_meta_t &bms_meta, vvUINT32 &v_ISL,
    v_uint32_uint32_map *v_blockStationsMap, vvUINT32 *v_CML,
    UINT32 &bstn_count, UINT32 &asl_count, UINT32 &bmsr_count,
    UINT32 &unknownParams, UINT32 &unknownsCount, UINT32 &measurementParams,
    UINT32 &measurementCount) -> bool {
  if (!loadStations(bstBinaryRecords, bst_meta, bstn_count))
    return false;

  if (constraint_applier_)
    constraint_applier_();

  vUINT32 v_ISLTemp;
  if (!loadAssociatedStations(vAssocStnList, v_ISLTemp, asl_count))
    return false;

  if (invalid_station_remover_)
    invalid_station_remover_(v_ISLTemp);

  unknownParams = unknownsCount = static_cast<UINT32>(v_ISLTemp.size() * 3);

  if (settings_.a.adjust_mode == SimultaneousMode) {
    processSimultaneousMode(v_ISLTemp, v_ISL, v_blockStationsMap, unknownParams,
                            unknownsCount);
  }

  std::cerr << "DEBUG NetworkDataManager: About to load measurements. Current "
               "bmsBinaryRecords size: "
            << (bmsBinaryRecords ? bmsBinaryRecords->size() : 0) << std::endl;

  if (!loadMeasurements(bmsBinaryRecords, bms_meta, bmsr_count)) {
    std::cerr << "DEBUG NetworkDataManager: loadMeasurements failed!"
              << std::endl;
    return false;
  }

  std::cerr
      << "DEBUG NetworkDataManager: loadMeasurements completed. bmsr_count: "
      << bmsr_count << ", bmsBinaryRecords size: "
      << (bmsBinaryRecords ? bmsBinaryRecords->size() : 0) << std::endl;

  // Process measurements for both simultaneous and phased modes
  // Phased mode needs measurements available for segmentation file loading
  if (settings_.a.adjust_mode == SimultaneousMode) {
    processMeasurements(bmsBinaryRecords, bmsr_count, v_CML, measurementParams,
                        measurementCount);
  } else if (settings_.a.adjust_mode == PhasedMode ||
             settings_.a.adjust_mode == Phased_Block_1Mode) {
    std::cerr << "DEBUG NetworkDataManager: Processing phased mode measurements"
              << std::endl;
    // For phased mode, we still need the measurements loaded for segmentation
    // file processing but we don't process them the same way as simultaneous
    // mode
    measurementParams = measurementCount = bmsr_count;
    std::cerr << "DEBUG NetworkDataManager: Set "
                 "measurementParams/measurementCount to: "
              << bmsr_count << std::endl;
  }

  return true;
}

auto NetworkDataManager::loadStations(vstn_t *bstBinaryRecords,
                                      binary_file_meta_t &bst_meta,
                                      UINT32 &bstn_count) -> bool {
  auto result = station_loader_->load(bstBinaryRecords, bst_meta);
  if (!result) {
    if (error_handler_)
      error_handler_("Failed to load binary station file", 0);
    return false;
  }
  bstn_count = *result;
  return true;
}

auto NetworkDataManager::loadAssociatedStations(vASL *vAssocStnList,
                                                vUINT32 &v_ISLTemp,
                                                UINT32 &asl_count) -> bool {
  auto result = asl_loader_->load(vAssocStnList, &v_ISLTemp);
  if (!result) {
    if (error_handler_)
      error_handler_("Failed to load associated station list file", 0);
    return false;
  }
  asl_count = *result;
  return true;
}

auto NetworkDataManager::loadMeasurements(vmsr_t *bmsBinaryRecords,
                                          binary_file_meta_t &bms_meta,
                                          UINT32 &bmsr_count) -> bool {
  auto result = measurement_loader_->load(bmsBinaryRecords, bms_meta);
  if (!result) {
    if (error_handler_)
      error_handler_("Failed to load binary measurement file", 0);
    return false;
  }
  bmsr_count = *result;
  return true;
}

void NetworkDataManager::processSimultaneousMode(
    const vUINT32 &v_ISLTemp, vvUINT32 &v_ISL,
    v_uint32_uint32_map *v_blockStationsMap, UINT32 &unknownParams,
    UINT32 &unknownsCount) {
  v_ISL.clear();
  v_ISL.push_back(v_ISLTemp);

  if (v_blockStationsMap->at(0).max_size() <= v_ISL.at(0).size()) {
    std::stringstream ss;
    ss << "NetworkDataManager::loadNetworkFiles(): Could not allocate "
          "sufficient memory for blockStationsMap.";
    if (error_handler_)
      error_handler_(ss.str(), 0);
    return;
  }

  for (UINT32 i = 0; i < v_ISL.at(0).size(); ++i)
    (*v_blockStationsMap)[0][v_ISL.at(0).at(i)] = i;
}

void NetworkDataManager::processMeasurements(vmsr_t *bmsBinaryRecords,
                                             UINT32 bmsr_count, vvUINT32 *v_CML,
                                             UINT32 &measurementParams,
                                             UINT32 &measurementCount) {
  processors::MeasurementCounts counts;
  auto result = measurement_processor_->processForMode(
      *bmsBinaryRecords, bmsr_count, *v_CML, counts);

  if (!result) {
    std::stringstream ss;
    ss << "No measurements were found.\n"
       << "  If measurements were successfully loaded on import, ensure that\n"
       << "  all measurements have not been ignored.";
    if (error_handler_)
      error_handler_(ss.str(), 0);
    return;
  }

  if (non_measurement_remover_)
    non_measurement_remover_(0);

  measurementParams = measurementCount = counts.measurement_count;

  // Update measurement vectors in the calling class
  if (measurement_count_updater_) {
    // Debug output
    std::cout << "DEBUG: Setting measurement_count=" << counts.measurement_count
              << ", measurement_variance_count="
              << counts.measurement_variance_count << std::endl;
    measurement_count_updater_(counts.measurement_count,
                               counts.measurement_variance_count);
  }
}

} // namespace networkadjust
} // namespace dynadjust
