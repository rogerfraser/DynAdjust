//============================================================================
// Name         : network_data_loader.cpp
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
// Description  : Network Data Loader implementation
//============================================================================

#include "network_data_loader.hpp"
#include <sstream>

namespace dynadjust {
namespace networkadjust {

NetworkDataLoader::NetworkDataLoader(const project_settings &settings)
    : settings_(settings),
      bst_loader_(std::make_unique<dynadjust::iostreams::BstFile>()),
      bms_loader_(std::make_unique<dynadjust::iostreams::BmsFile>()),
      map_loader_(std::make_unique<dynadjust::iostreams::MapFile>()),
      measurement_processor_(std::make_unique<processors::MeasurementProcessor>(
          settings.a.adjust_mode == SimultaneousMode
              ? processors::AdjustmentMode::Simultaneous
              : processors::AdjustmentMode::Phased)) {}

bool NetworkDataLoader::LoadInto(
    vstn_t *bstBinaryRecords, binary_file_meta_t &bst_meta, vASL *vAssocStnList,
    vmsr_t *bmsBinaryRecords, binary_file_meta_t &bms_meta, vvUINT32 &v_ISL,
    v_uint32_uint32_map *v_blockStationsMap, vvUINT32 *v_CML,
    UINT32 &bstn_count, UINT32 &asl_count, UINT32 &bmsr_count,
    UINT32 &unknownParams, UINT32 &unknownsCount, UINT32 &measurementParams,
    UINT32 &measurementCount) {
  if (!LoadStations(bstBinaryRecords, bst_meta, bstn_count))
    return false;

  // Apply constraints internally instead of using callback
  if (bstBinaryRecords && !settings_.a.station_constraints.empty()) {
    ApplyConstraints(*bstBinaryRecords);
  }

  vUINT32 v_ISLTemp;
  if (!LoadAssociatedStations(vAssocStnList, v_ISLTemp, asl_count))
    return false;

  // Remove invalid stations internally
  if (vAssocStnList) {
    RemoveInvalidStations(v_ISLTemp, *vAssocStnList);
  }

  unknownParams = unknownsCount = static_cast<UINT32>(v_ISLTemp.size() * 3);

  if (settings_.a.adjust_mode == SimultaneousMode) {
    ProcessSimultaneousMode(v_ISLTemp, v_ISL, v_blockStationsMap, unknownParams,
                            unknownsCount);
  }

  if (!LoadMeasurements(bmsBinaryRecords, bms_meta, bmsr_count)) {
    return false;
  }

  // Process measurements for both simultaneous and phased modes
  // Phased mode needs measurements available for segmentation file loading
  if (settings_.a.adjust_mode == SimultaneousMode) {
    ProcessMeasurements(bmsBinaryRecords, bmsr_count, v_CML, measurementParams,
                        measurementCount);
                        
    // Apply non-measurement removal for simultaneous mode
    if (v_CML && !v_CML->empty()) {
      RemoveNonMeasurements((*v_CML)[0], *bmsBinaryRecords);
    }
  } else if (settings_.a.adjust_mode == PhasedMode ||
             settings_.a.adjust_mode == Phased_Block_1Mode) {
    // For phased mode, we still need the measurements loaded for segmentation
    // file processing but we don't process them the same way as simultaneous
    // mode
    measurementParams = measurementCount = bmsr_count;
  }

  return true;
}

bool NetworkDataLoader::LoadStations(vstn_t *bstBinaryRecords,
                                      binary_file_meta_t &bst_meta,
                                      UINT32 &bstn_count) {
  auto result = bst_loader_->LoadWithOptional(settings_.a.bst_file, bstBinaryRecords, bst_meta);
  if (!result) {
    if (error_handler_)
      error_handler_("Failed to load binary station file", 0);
    return false;
  }
  bstn_count = static_cast<UINT32>(*result);
  return true;
}

bool NetworkDataLoader::LoadAssociatedStations(vASL *vAssocStnList,
                                                vUINT32 &v_ISLTemp,
                                                UINT32 &asl_count) {
  dynadjust::iostreams::AslFile asl_loader(settings_.s.asl_file);
  auto result = asl_loader.TryLoad();
  if (!result) {
    if (error_handler_)
      error_handler_("Failed to load associated station list file", 0);
    return false;
  }
  
  // Copy results to output parameters
  *vAssocStnList = std::move(result->stations);
  v_ISLTemp = std::move(result->free_stations);
  asl_count = static_cast<UINT32>(result->count);
  return true;
}

bool NetworkDataLoader::LoadMeasurements(vmsr_t *bmsBinaryRecords,
                                          binary_file_meta_t &bms_meta,
                                          UINT32 &bmsr_count) {
  auto result = bms_loader_->LoadWithOptional(settings_.a.bms_file, bmsBinaryRecords, bms_meta);
  if (!result) {
    if (error_handler_)
      error_handler_("Failed to load binary measurement file", 0);
    return false;
  }
  bmsr_count = static_cast<UINT32>(*result);
  return true;
}

void NetworkDataLoader::ProcessSimultaneousMode(
    const vUINT32 &v_ISLTemp, vvUINT32 &v_ISL,
    v_uint32_uint32_map *v_blockStationsMap, UINT32 &unknownParams,
    UINT32 &unknownsCount) {
  v_ISL.clear();
  v_ISL.push_back(v_ISLTemp);

  if (v_blockStationsMap->at(0).max_size() <= v_ISL.at(0).size()) {
    std::stringstream ss;
    ss << "NetworkDataLoader::load(): Could not allocate "
          "sufficient memory for blockStationsMap.";
    if (error_handler_)
      error_handler_(ss.str(), 0);
    return;
  }

  for (UINT32 i = 0; i < v_ISL.at(0).size(); ++i)
    (*v_blockStationsMap)[0][v_ISL.at(0).at(i)] = i;
}

void NetworkDataLoader::ProcessMeasurements(vmsr_t *bmsBinaryRecords,
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

void NetworkDataLoader::SignalException(std::string_view message, UINT32 block_no) {
  std::ostringstream error_stream;
  
  switch (settings_.a.adjust_mode) {
    case Phased_Block_1Mode:
    case PhasedMode:
      error_stream << message << "\n"
                   << "  Phased adjustment terminated whilst processing block " 
                   << (block_no + 1) << "\n";
      break;
    default:
      error_stream << message;
      break;
  }
  
  throw std::runtime_error(error_stream.str());
}

void NetworkDataLoader::ApplyConstraints(vstn_t& stations, std::string_view station_map_file) {
  if (settings_.a.station_constraints.empty()) {
    return;
  }

  // Load station map
  v_string_uint32_pair station_map;
  std::string map_file{station_map_file};
  if (map_file.empty()) {
    map_file = settings_.g.input_folder + FOLDER_SLASH + 
               settings_.g.network_name + ".map";
  }
  LoadStationMap(&station_map, map_file);

  // Parse constraints from comma-delimited string
  vstring constraint_tokens;
  try {
    SplitDelimitedString<std::string>(
        settings_.a.station_constraints,
        std::string(","),
        &constraint_tokens);
  } catch (...) {
    return; // Invalid constraint format, silently ignore
  }

  if (apply_discontinuities_) {
    AddDiscontinuitySites(constraint_tokens, stations);
  }

  // Process station constraints in pairs (station_name, constraint)
  for (auto it = constraint_tokens.begin(); it != constraint_tokens.end(); ++it) {
    const std::string& station_name = *it;
    
    // Get the constraint (next token)
    if (++it == constraint_tokens.end()) {
      break; // No constraint found for this station
    }
    
    std::string constraint = *it;
    std::transform(constraint.begin(), constraint.end(), constraint.begin(), ::toupper);

    // Find station in map
    auto station_range = std::equal_range(
        station_map.begin(), station_map.end(), 
        station_name, 
        StationNameIDCompareName());
    
    if (station_range.first == station_range.second) {
      if (apply_discontinuities_) {
        continue; // Station may have been renamed during discontinuity processing
      }
      
      std::ostringstream ss;
      ss << "The supplied constraint station " << station_name 
         << " is not in the stations map. Please ensure that " 
         << station_name << " is included in the list of stations.";
      SignalException(ss.str());
    }

    // Validate constraint
    if (!CDnaStation::IsValidConstraint(constraint)) {
      std::ostringstream ss;
      ss << "The supplied station constraint " << constraint 
         << " is not a valid constraint.";
      SignalException(ss.str());
    }

    // Apply constraint to station
    const UINT32 station_index = station_range.first->second;
    if (station_index < stations.size()) {
      snprintf(stations[station_index].stationConst, STN_CONST_WIDTH, 
               "%s", constraint.c_str());
    }
  }
}

void NetworkDataLoader::RemoveNonMeasurements(vUINT32& measurement_list, 
                                              const vmsr_t& measurements) {
  if (measurement_list.size() < 2) {
    return;
  }
  
  // Modern C++ approach using lambda instead of custom functor
  auto is_non_measurement = [&measurements](UINT32 index) -> bool {
    return index >= measurements.size() || 
           measurements[index].measStart != xMeas;
  };
  
  // Remove non-measurements using modern algorithm
  auto new_end = std::remove_if(measurement_list.begin(), 
                                measurement_list.end(), 
                                is_non_measurement);
  measurement_list.erase(new_end, measurement_list.end());
  
  // Sort by file order if we have measurements remaining
  if (!measurement_list.empty()) {
    std::sort(measurement_list.begin(), measurement_list.end(),
              [&measurements](UINT32 a, UINT32 b) -> bool {
                if (a >= measurements.size() || b >= measurements.size()) {
                  return a < b;
                }
                return measurements[a].fileOrder < measurements[b].fileOrder;
              });
  }
}

void NetworkDataLoader::RemoveInvalidStations(vUINT32& station_list, const vASL& associated_stations) {
  if (station_list.empty()) {
    return;
  }
  
  // Modern C++ approach using remove_if + erase and lambda  
  // Remove stations that are invalid (equivalent to original FALSE check)
  auto new_end = std::remove_if(station_list.begin(), station_list.end(),
                               [&associated_stations](UINT32 index) -> bool {
    return index >= associated_stations.size() || 
           associated_stations[index].Validity() == 0;  // 0 = INVALID_STATION
  });
  station_list.erase(new_end, station_list.end());
  
  // Sort remaining valid stations by index
  std::sort(station_list.begin(), station_list.end());
}

void NetworkDataLoader::LoadStationMap(pv_string_uint32_pair station_map, 
                                       std::string_view map_file) {
  try {
    map_loader_->LoadFile(std::string{map_file}, station_map);
  } catch (const std::runtime_error& e) {
    SignalException(e.what());
  }
}

void NetworkDataLoader::AddDiscontinuitySites(vstring& constraint_stations, vstn_t& stations) {
  // Sort stations by original name to enable efficient searching
  std::sort(stations.begin(), stations.end(), [](const station_t& lhs, const station_t& rhs) {
    return std::string(lhs.stationNameOrig) < std::string(rhs.stationNameOrig);
  });

  vstring constraint_discontinuity_stations;
  
  // Process constraint stations in pairs (station_name, constraint_code)
  for (auto const_it = constraint_stations.begin(); const_it != constraint_stations.end(); ++const_it) {
    // Find this constraint station in the station records by original name
    // Create a comparator that works with heterogeneous lookup
    auto comparator = [](const station_t& lhs, const station_t& rhs) {
      return std::string(lhs.stationNameOrig) < std::string(rhs.stationNameOrig);
    };
    
    // Create a temporary station object for comparison
    station_t temp_station;
    std::strncpy(temp_station.stationNameOrig, const_it->c_str(), STN_NAME_WIDTH);
    temp_station.stationNameOrig[STN_NAME_WIDTH-1] = '\0';
    
    auto station_range = std::equal_range(stations.begin(), stations.end(), temp_station, comparator);
    
    if (station_range.first == station_range.second) {
      // Station not found in records, skip to next pair
      continue;
    }

    // This constraint station is one of the discontinuity sites.
    // Add the current "discontinuity name" (stationName, not stationNameOrig)
    constraint_discontinuity_stations.push_back(station_range.first->stationName);
    
    // Add the constraint code (next token)
    if (++const_it == constraint_stations.end()) {
      break;
    }
    constraint_discontinuity_stations.push_back(*const_it);
  }

  // Add the constraint discontinuity sites to the original constraint list
  constraint_stations.insert(constraint_stations.end(), 
                             constraint_discontinuity_stations.begin(), 
                             constraint_discontinuity_stations.end());

  // Restore original sort order by regular station name
  std::sort(stations.begin(), stations.end(), [](const station_t& lhs, const station_t& rhs) {
    return std::string(lhs.stationName) < std::string(rhs.stationName);
  });
}

} // namespace networkadjust
} // namespace dynadjust
