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
#include <include/io/bst_file.hpp>
#include <include/io/map_file.hpp>

#include <include/functions/dnatemplatefuncs.hpp>
#include <include/functions/dnastrmanipfuncs.hpp>
#include <include/functions/dnatemplatestnmsrfuncs.hpp>
#include <include/measurement_types/dnastation.hpp>
#include <include/math/dnamatrix_contiguous.hpp>

#include "measurement_processor.hpp"

using namespace dynadjust::measurements;
using namespace dynadjust::math;

namespace dynadjust {
namespace networkadjust {

// Exception hierarchy for NetworkDataLoader
class NetworkLoadException : public std::runtime_error {
public:
  explicit NetworkLoadException(const std::string& message) 
    : std::runtime_error(message) {}
};

class StationLoadException : public NetworkLoadException {
public:
  explicit StationLoadException(const std::string& message) 
    : NetworkLoadException("Station loading error: " + message) {}
};

class MeasurementLoadException : public NetworkLoadException {
public:
  explicit MeasurementLoadException(const std::string& message) 
    : NetworkLoadException("Measurement loading error: " + message) {}
};

class ConstraintException : public NetworkLoadException {
public:
  explicit ConstraintException(const std::string& message) 
    : NetworkLoadException("Constraint error: " + message) {}
};

using ErrorHandler = std::function<void(const std::string &, UINT32)>;

class NetworkDataLoader {
public:
  explicit NetworkDataLoader(const project_settings &settings);
  ~NetworkDataLoader() = default;

  NetworkDataLoader(const NetworkDataLoader &) = delete;
  NetworkDataLoader &operator=(const NetworkDataLoader &) = delete;
  NetworkDataLoader(NetworkDataLoader &&) = default;
  NetworkDataLoader &operator=(NetworkDataLoader &&) = delete;

  bool LoadInto(vstn_t *bstBinaryRecords, binary_file_meta_t &bst_meta,
            vASL *vAssocStnList, vmsr_t *bmsBinaryRecords,
            binary_file_meta_t &bms_meta, vvUINT32 &v_ISL,
            v_uint32_uint32_map *v_blockStationsMap,
            vvUINT32 *v_CML, UINT32 &bstn_count, UINT32 &asl_count,
            UINT32 &bmsr_count, UINT32 &unknownParams,
            UINT32 &unknownsCount, UINT32 &measurementParams,
            UINT32 &measurementCount, UINT32 &measurementVarianceCount,
            // Additional parameters for simultaneous mode
            UINT32* blockCount = nullptr,
            vvUINT32* v_JSL = nullptr,
            vUINT32* v_unknownsCount = nullptr,
            vUINT32* v_measurementCount = nullptr,
            vUINT32* v_measurementVarianceCount = nullptr,
            vUINT32* v_measurementParams = nullptr,
            vUINT32* v_ContiguousNetList = nullptr,
            std::vector<blockMeta_t>* v_blockMeta = nullptr,
            vvUINT32* v_parameterStationList = nullptr,
            vv_stn_appear* v_paramStnAppearance = nullptr,
            v_mat_2d* v_junctionVariances = nullptr,
            v_mat_2d* v_junctionVariancesFwd = nullptr);


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
                           UINT32 &measurementCount, UINT32 &measurementVarianceCount);

  // Helper methods
  template<typename T>
  void InitializeIfEmpty(T* vector, size_t size, 
                        const typename T::value_type& value = {}) {
    if (vector && vector->empty()) {
      vector->resize(size, value);
    }
  }
  
  void LoadStationMap(const pv_string_uint32_pair station_map, std::string_view map_file);
  void AddDiscontinuitySites(vstring& constraint_stations, vstn_t& stations);
  void InitializeSimultaneousModeVectors(
                                        const vvUINT32& v_ISL,
                                        UINT32 unknownsCount, 
                                        UINT32 measurementParams, 
                                        UINT32 measurementCount,
                                        UINT32 measurementVarianceCount,
                                        UINT32* blockCount,
                                        vvUINT32* v_JSL,
                                        vUINT32* v_unknownsCount,
                                        vUINT32* v_measurementCount,
                                        vUINT32* v_measurementVarianceCount,
                                        vUINT32* v_measurementParams,
                                        vUINT32* v_ContiguousNetList,
                                        std::vector<blockMeta_t>* v_blockMeta,
                                        vvUINT32* v_parameterStationList,
                                        vv_stn_appear* v_paramStnAppearance,
                                        v_mat_2d* v_junctionVariances,
                                        v_mat_2d* v_junctionVariancesFwd);

  const project_settings &settings_;
  std::unique_ptr<dynadjust::iostreams::BstFile> bst_loader_;
  std::unique_ptr<dynadjust::iostreams::BmsFile> bms_loader_;
  std::unique_ptr<dynadjust::iostreams::MapFile> map_loader_;
  std::unique_ptr<processors::MeasurementProcessor> measurement_processor_;

  // State for constraint and measurement processing
  bool apply_discontinuities_ = false;
};

} // namespace networkadjust
} // namespace dynadjust

#endif // NETWORK_DATA_LOADER_HPP_
