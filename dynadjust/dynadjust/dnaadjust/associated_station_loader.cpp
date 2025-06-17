//============================================================================
// Name         : associated_station_loader.cpp
// Copyright    : Copyright 2025 Geoscience Australia
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
// Description  : Associated Station List Loader implementation
//============================================================================

#include "associated_station_loader.hpp"
#include <stdexcept>

namespace dynadjust {
namespace networkadjust {
namespace loaders {

AssociatedStationLoader::AssociatedStationLoader(std::string_view filename)
    : filename_(filename)
    , asl_loader_(std::make_unique<dna_io_asl>())
{
}

std::optional<UINT32> AssociatedStationLoader::load(vASL* vAssocStnList, vUINT32* v_ISLTemp)
{
    try {
        return asl_loader_->load_asl_file(filename_, vAssocStnList, v_ISLTemp);
    }
    catch (const std::runtime_error&) {
        return std::nullopt;
    }
}

} // namespace loaders
} // namespace networkadjust
} // namespace dynadjust
