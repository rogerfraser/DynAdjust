//============================================================================
// Name         : binary_measurement_loader.cpp
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
// Description  : Binary Measurement Loader implementation
//============================================================================

#include "binary_measurement_loader.hpp"
#include <stdexcept>

namespace dynadjust {
namespace networkadjust {
namespace loaders {

BinaryMeasurementLoader::BinaryMeasurementLoader(std::string_view filename)
    : filename_(filename)
    , bms_loader_(std::make_unique<dna_io_bms>())
{
}

std::optional<UINT32> BinaryMeasurementLoader::load(vmsr_t* bmsBinaryRecords, binary_file_meta_t& bms_meta)
{
    try {
        return bms_loader_->load_bms_file(filename_, bmsBinaryRecords, bms_meta);
    }
    catch (const std::runtime_error&) {
        return std::nullopt;
    }
}

} // namespace loaders
} // namespace networkadjust
} // namespace dynadjust
