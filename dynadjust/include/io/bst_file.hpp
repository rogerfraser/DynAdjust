//============================================================================
// Name         : bst_file.hpp
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
// Description  : DynAdjust binary station file io operations
//============================================================================

#ifndef DYNADJUST_BST_FILE_H_
#define DYNADJUST_BST_FILE_H_

#if defined(_MSC_VER)
  #if defined(LIST_INCLUDES_ON_BUILD)
    #pragma message("  " __FILE__)
  #endif
#endif

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <include/config/dnatypes-fwd.hpp>
#include <include/config/dnatypes-structs.hpp>  // For vifm_t, input_file_meta_t, binary_file_meta_t
#include <include/io/dynadjust_file.hpp>
#include <include/measurement_types/dnameasurement.hpp>

namespace dynadjust {
namespace iostreams {

class DNATYPE_API BstFile : public DynadjustFile {
 public:
  BstFile() = default;
  BstFile(const BstFile& bst) : DynadjustFile(bst) {}
  virtual ~BstFile() = default;

  BstFile& operator=(const BstFile& rhs);

  std::uint64_t CreateStnInputFileMeta(vifm_t& vinput_file_meta,
                                       input_file_meta_t** input_file_meta);

  void LoadFileMeta(const std::string& bst_filename,
                    binary_file_meta_t& bst_meta);

  std::uint64_t LoadFile(const std::string& bst_filename,
                         pvstn_t vbinary_stn,
                         binary_file_meta_t& bst_meta);

  std::optional<std::uint64_t> LoadWithOptional(const std::string& bst_filename,
                                                 pvstn_t vbinary_stn,
                                                 binary_file_meta_t& bst_meta);

  void WriteFile(const std::string& bst_filename,
                 pvstn_t vbinary_stn,
                 binary_file_meta_t& bst_meta);

  bool WriteFile(const std::string& bst_filename,
                 measurements::vdnaStnPtr* vStations,
                 pvstring vUnusedStns,
                 binary_file_meta_t& bst_meta,
                 bool flagUnused);

 protected:
};

}  // namespace iostreams
}  // namespace dynadjust

#endif  // DYNADJUST_BST_FILE_H_