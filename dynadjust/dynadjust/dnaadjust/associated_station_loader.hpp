//============================================================================
// Name         : associated_station_loader.hpp
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
// Description  : Associated Station List Loader for DynAdjust Network
// Adjustment
//============================================================================

#ifndef ASSOCIATED_STATION_LOADER_HPP_
#define ASSOCIATED_STATION_LOADER_HPP_

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <include/config/dnatypes.hpp>
#include <include/io/dnaioasl.hpp>

using namespace dynadjust::iostreams;
using namespace dynadjust::measurements;

namespace dynadjust {
namespace networkadjust {
namespace loaders {

class AssociatedStationLoader {
public:
  explicit AssociatedStationLoader(std::string_view filename);
  ~AssociatedStationLoader() = default;

  AssociatedStationLoader(const AssociatedStationLoader &) = delete;
  AssociatedStationLoader &operator=(const AssociatedStationLoader &) = delete;
  AssociatedStationLoader(AssociatedStationLoader &&) = default;
  AssociatedStationLoader &operator=(AssociatedStationLoader &&) = default;

  auto load(vASL *vAssocStnList, vUINT32 *v_ISLTemp) -> std::optional<UINT32>;

private:
  std::string filename_;
  std::unique_ptr<dna_io_asl> asl_loader_;
};

} // namespace loaders
} // namespace networkadjust
} // namespace dynadjust

#endif // ASSOCIATED_STATION_LOADER_HPP_
