//============================================================================
// Name         : map_file.cpp
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
// Description  : DynAdjust station map file io operations
//============================================================================

#include <include/io/map_file.hpp>

#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>

#include <include/functions/dnaiostreamfuncs.hpp>
#include <include/functions/dnastrmanipfuncs.hpp>

namespace dynadjust {
namespace iostreams {

MapFile& MapFile::operator=(const MapFile& rhs) {
  if (this == &rhs) {
    return *this;
  }
  DynadjustFile::operator=(rhs);
  return *this;
}

std::uint64_t MapFile::LoadFile(const std::string& map_filename,
                                      pv_string_uint32_pair station_map) {
  std::ifstream map_file;
  std::ostringstream os;
  os << "LoadFile(): An error was encountered when opening " << map_filename
     << "." << std::endl;

  try {
    file_opener(map_file, map_filename, std::ios::in | std::ios::binary,
                binary, true);
  } catch (const std::runtime_error& e) {
    os << e.what();
    throw std::runtime_error(os.str());
  } catch (...) {
    throw std::runtime_error(os.str());
  }

  os.str("");
  os << "LoadFile(): An error was encountered when reading from "
     << map_filename << "." << std::endl;

  station_map->clear();
  std::uint32_t map_size;
  string_uint32_pair station_id;
  char station_name[STN_NAME_WIDTH];

  try {
    ReadFileInfo(map_file);

    map_file.read(reinterpret_cast<char*>(&map_size), sizeof(std::uint32_t));
    station_map->reserve(map_size);

    for (std::uint32_t i = 0; i < map_size; i++) {
      map_file.read(station_name, sizeof(station_name));
      station_id.first = station_name;
      map_file.read(reinterpret_cast<char*>(&station_id.second),
                    sizeof(std::uint32_t));
      station_map->push_back(station_id);
    }
  } catch (const std::ios_base::failure& f) {
    os << f.what();
    throw std::runtime_error(os.str());
  } catch (const std::runtime_error& e) {
    os << e.what();
    throw std::runtime_error(os.str());
  } catch (...) {
    throw std::runtime_error(os.str());
  }

  map_file.close();

  if (station_map->size() < map_size) {
    throw std::runtime_error(
        "LoadFile(): Could not allocate sufficient memory for the Station "
        "map.");
  }

  return static_cast<std::uint64_t>(station_map->size());
}

std::optional<std::uint64_t> MapFile::LoadWithOptional(
    const std::string& map_filename, pv_string_uint32_pair station_map) {
  if (map_filename.empty()) {
    return std::nullopt;
  }

  try {
    return LoadFile(map_filename, station_map);
  } catch (const std::runtime_error&) {
    return std::nullopt;
  }
}

void MapFile::WriteFile(const std::string& map_filename,
                              pv_string_uint32_pair station_map) {
  std::ofstream map_file;
  std::ostringstream os;
  os << "WriteFile(): An error was encountered when opening " << map_filename
     << "." << std::endl;

  try {
    file_opener(map_file, map_filename, std::ios::out | std::ios::binary,
                binary);
  } catch (const std::runtime_error& e) {
    os << e.what();
    throw std::runtime_error(os.str());
  } catch (...) {
    throw std::runtime_error(os.str());
  }

  os.str("");
  os << "WriteFile(): An error was encountered when writing to "
     << map_filename << "." << std::endl;

  v_string_uint32_pair::const_iterator it_station_map(station_map->begin());
  std::uint32_t map_value(static_cast<std::uint32_t>(station_map->size()));

  try {
    WriteFileInfo(map_file);

    map_file.write(reinterpret_cast<char*>(&map_value),
                   sizeof(std::uint32_t));
    char station_name[STN_NAME_WIDTH];
    std::memset(station_name, '\0', sizeof(station_name));

    for (; it_station_map != station_map->end(); ++it_station_map) {
      std::strcpy(station_name, it_station_map->first.c_str());
      map_value = it_station_map->second;
      map_file.write(station_name, sizeof(station_name));
      map_file.write(reinterpret_cast<char*>(&map_value),
                     sizeof(std::uint32_t));
    }
  } catch (const std::ios_base::failure& f) {
    os << f.what();
    throw std::runtime_error(os.str());
  } catch (const std::runtime_error& e) {
    os << e.what();
    throw std::runtime_error(os.str());
  } catch (...) {
    throw std::runtime_error(os.str());
  }

  map_file.close();
}

void MapFile::WriteTextFile(const std::string& map_filename,
                                  pv_string_uint32_pair station_map) {
  std::ofstream map_file;
  std::ostringstream os;
  os << "WriteTextFile(): An error was encountered when opening "
     << map_filename << "." << std::endl;

  try {
    file_opener(map_file, map_filename);
  } catch (const std::runtime_error& e) {
    os << e.what();
    throw std::runtime_error(os.str());
  } catch (...) {
    throw std::runtime_error(os.str());
  }

  os.str("");
  os << "WriteTextFile(): An error was encountered when writing to "
     << map_filename << "." << std::endl;

  try {
    std::ostringstream ss_map;
    ss_map << station_map->size() << " stations";
    map_file << std::left << std::setw(STATION) << ss_map.str();
    map_file << std::setw(HEADER_20) << std::right << "Stn. index"
             << std::endl;

    v_string_uint32_pair::const_iterator it_station_map(station_map->begin());
    for (it_station_map = station_map->begin();
         it_station_map != station_map->end(); ++it_station_map) {
      map_file << std::setw(STATION) << std::left
               << it_station_map->first.c_str() << std::setw(HEADER_20)
               << std::right << it_station_map->second << std::endl;
    }
  } catch (const std::ios_base::failure& f) {
    os << f.what();
    throw std::runtime_error(os.str());
  } catch (const std::runtime_error& e) {
    os << e.what();
    throw std::runtime_error(os.str());
  } catch (...) {
    throw std::runtime_error(os.str());
  }

  map_file.close();
}

}  // namespace iostreams
}  // namespace dynadjust