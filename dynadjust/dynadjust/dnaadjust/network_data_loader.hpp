//============================================================================
// Name         : network_data_loader.hpp
// Author       : Roger Fraser
// Contributors : Dale Roberts <dale.o.roberts@gmail.com>
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
// Description  : Network Data Loader for DynAdjust Network Adjustment
//============================================================================

#ifndef NETWORK_DATA_LOADER_HPP_
#define NETWORK_DATA_LOADER_HPP_

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <sstream>

#include <include/config/dnaoptions-interface.hpp>
#include <include/config/dnatypes.hpp>

#include <include/io/asl_file.hpp>
#include <include/io/bms_file.hpp>
#include <include/io/bst_file_loader.hpp>
#include <include/io/map_file_loader.hpp>

#include <include/functions/dnatemplatefuncs.hpp>
#include <include/functions/dnastrmanipfuncs.hpp>
#include <include/functions/dnatemplatestnmsrfuncs.hpp>
#include <include/measurement_types/dnastation.hpp>

#include "measurement_processor.hpp"

using namespace dynadjust::measurements;

namespace dynadjust {
namespace networkadjust {

using ErrorHandler = std::function<void(const std::string &, UINT32)>;

class NetworkDataLoader {
public:
  explicit NetworkDataLoader(const project_settings &settings);
  ~NetworkDataLoader() = default;

  NetworkDataLoader(const NetworkDataLoader &) = delete;
  NetworkDataLoader &operator=(const NetworkDataLoader &) = delete;
  NetworkDataLoader(NetworkDataLoader &&) = default;
  NetworkDataLoader &operator=(NetworkDataLoader &&) = delete;

  void SetErrorHandler(ErrorHandler handler) {
    error_handler_ = std::move(handler);
  }
  void SetConstraintApplier(std::function<void()> applier) {
    constraint_applier_ = std::move(applier);
  }
  void SetInvalidStationRemover(std::function<void(vUINT32 &)> remover) {
    invalid_station_remover_ = std::move(remover);
  }
  void SetNonMeasurementRemover(std::function<void(UINT32)> remover) {
    non_measurement_remover_ = std::move(remover);
  }
  void SetMeasurementCountUpdater(std::function<void(UINT32, UINT32)> updater) {
    measurement_count_updater_ = std::move(updater);
  }

  bool LoadInto(vstn_t *bstBinaryRecords, binary_file_meta_t &bst_meta,
            vASL *vAssocStnList, vmsr_t *bmsBinaryRecords,
            binary_file_meta_t &bms_meta, vvUINT32 &v_ISL,
            v_uint32_uint32_map *v_blockStationsMap,
            vvUINT32 *v_CML, UINT32 &bstn_count, UINT32 &asl_count,
            UINT32 &bmsr_count, UINT32 &unknownParams,
            UINT32 &unknownsCount, UINT32 &measurementParams,
            UINT32 &measurementCount);

  // Exception handling
  [[noreturn]] void SignalException(std::string_view message, UINT32 block_no = 0);

  // Constraint application
  void ApplyConstraints(vstn_t& stations, std::string_view station_map_file = "");

  // Measurement filtering
  void RemoveNonMeasurements(vUINT32& measurement_list, const vmsr_t& measurements);

  // Station filtering  
  void RemoveInvalidStations(vUINT32& station_list, const vASL& associated_stations);

private:
  bool LoadStations(vstn_t *bstBinaryRecords, binary_file_meta_t &bst_meta,
                    UINT32 &bstn_count);

  bool LoadAssociatedStations(vASL *vAssocStnList, vUINT32 &v_ISLTemp,
                              UINT32 &asl_count);

  bool LoadMeasurements(vmsr_t *bmsBinaryRecords, binary_file_meta_t &bms_meta,
                        UINT32 &bmsr_count);

  void ProcessSimultaneousMode(const vUINT32 &v_ISLTemp, vvUINT32 &v_ISL,
                               v_uint32_uint32_map *v_blockStationsMap,
                               UINT32 &unknownParams, UINT32 &unknownsCount);

  void ProcessMeasurements(vmsr_t *bmsBinaryRecords, UINT32 bmsr_count,
                           vvUINT32 *v_CML, UINT32 &measurementParams,
                           UINT32 &measurementCount);

  // Helper methods
  void LoadStationMap(pv_string_uint32_pair station_map, std::string_view map_file);
  void AddDiscontinuitySites(vstring& constraint_stations, vstn_t& stations);

  const project_settings &settings_;
  std::unique_ptr<dynadjust::iostreams::BstFileLoader> bst_loader_;
  std::unique_ptr<dynadjust::iostreams::BmsFile> bms_loader_;
  std::unique_ptr<dynadjust::iostreams::MapFileLoader> map_loader_;
  std::unique_ptr<processors::MeasurementProcessor> measurement_processor_;

  // State for constraint and measurement processing
  mutable bool apply_discontinuities_ = false;

  ErrorHandler error_handler_;
  std::function<void()> constraint_applier_;
  std::function<void(vUINT32 &)> invalid_station_remover_;
  std::function<void(UINT32)> non_measurement_remover_;
  std::function<void(UINT32, UINT32)> measurement_count_updater_;
};

} // namespace networkadjust
} // namespace dynadjust

#endif // NETWORK_DATA_LOADER_HPP_
