//============================================================================
// Name         : network_data_manager.hpp
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
// Description  : Network Data Manager for DynAdjust Network Adjustment
//============================================================================

#ifndef NETWORK_DATA_MANAGER_HPP_
#define NETWORK_DATA_MANAGER_HPP_

#include <functional>
#include <memory>
#include <string>

#include <include/config/dnaoptions-interface.hpp>
#include <include/config/dnatypes.hpp>

#include "associated_station_loader.hpp"
#include "binary_measurement_loader.hpp"
#include "binary_station_loader.hpp"
#include "measurement_processor.hpp"

using namespace dynadjust::measurements;

namespace dynadjust {
namespace networkadjust {

using ErrorHandler = std::function<void(const std::string &, UINT32)>;

class NetworkDataManager {
public:
  explicit NetworkDataManager(const project_settings &settings);
  ~NetworkDataManager() = default;

  NetworkDataManager(const NetworkDataManager &) = delete;
  NetworkDataManager &operator=(const NetworkDataManager &) = delete;
  NetworkDataManager(NetworkDataManager &&) = default;
  NetworkDataManager &operator=(NetworkDataManager &&) = delete;

  void setErrorHandler(ErrorHandler handler) {
    error_handler_ = std::move(handler);
  }
  void setConstraintApplier(std::function<void()> applier) {
    constraint_applier_ = std::move(applier);
  }
  void setInvalidStationRemover(std::function<void(vUINT32 &)> remover) {
    invalid_station_remover_ = std::move(remover);
  }
  void setNonMeasurementRemover(std::function<void(UINT32)> remover) {
    non_measurement_remover_ = std::move(remover);
  }
  void setMeasurementCountUpdater(std::function<void(UINT32, UINT32)> updater) {
    measurement_count_updater_ = std::move(updater);
  }

  auto loadNetworkFiles(vstn_t *bstBinaryRecords, binary_file_meta_t &bst_meta,
                        vASL *vAssocStnList, vmsr_t *bmsBinaryRecords,
                        binary_file_meta_t &bms_meta, vvUINT32 &v_ISL,
                        v_uint32_uint32_map *v_blockStationsMap,
                        vvUINT32 *v_CML, UINT32 &bstn_count, UINT32 &asl_count,
                        UINT32 &bmsr_count, UINT32 &unknownParams,
                        UINT32 &unknownsCount, UINT32 &measurementParams,
                        UINT32 &measurementCount) -> bool;

private:
  auto loadStations(vstn_t *bstBinaryRecords, binary_file_meta_t &bst_meta,
                    UINT32 &bstn_count) -> bool;

  auto loadAssociatedStations(vASL *vAssocStnList, vUINT32 &v_ISLTemp,
                              UINT32 &asl_count) -> bool;

  auto loadMeasurements(vmsr_t *bmsBinaryRecords, binary_file_meta_t &bms_meta,
                        UINT32 &bmsr_count) -> bool;

  void processSimultaneousMode(const vUINT32 &v_ISLTemp, vvUINT32 &v_ISL,
                               v_uint32_uint32_map *v_blockStationsMap,
                               UINT32 &unknownParams, UINT32 &unknownsCount);

  void processMeasurements(vmsr_t *bmsBinaryRecords, UINT32 bmsr_count,
                           vvUINT32 *v_CML, UINT32 &measurementParams,
                           UINT32 &measurementCount);

  const project_settings &settings_;
  std::unique_ptr<loaders::BinaryStationLoader> station_loader_;
  std::unique_ptr<loaders::AssociatedStationLoader> asl_loader_;
  std::unique_ptr<loaders::BinaryMeasurementLoader> measurement_loader_;
  std::unique_ptr<processors::MeasurementProcessor> measurement_processor_;

  ErrorHandler error_handler_;
  std::function<void()> constraint_applier_;
  std::function<void(vUINT32 &)> invalid_station_remover_;
  std::function<void(UINT32)> non_measurement_remover_;
  std::function<void(UINT32, UINT32)> measurement_count_updater_;
};

} // namespace networkadjust
} // namespace dynadjust

#endif // NETWORK_DATA_MANAGER_HPP_
