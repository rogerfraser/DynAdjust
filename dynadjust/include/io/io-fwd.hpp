//============================================================================
// Name         : io-fwd.hpp
// Author       : Roger Fraser
// Contributors : Dale Roberts <dale.o.roberts@gmail.com>
//              : Dale Roberts
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
// Description  : Forward declarations for DynAdjust I/O classes
//============================================================================

#pragma once
#ifndef IO_FWD_H_
#define IO_FWD_H_

namespace dynadjust {
namespace iostreams {

// File handler classes
class dna_io_base;
class dna_io_adj;
class dna_io_aml;
class dna_io_asl;
class dna_io_bms;
class dna_io_bst;
class dna_io_map;
class dna_io_seg;
class dna_io_snx;
class dna_io_tbu;
class dna_io_tpb;

// Loader classes
class bms_file_loader;
class bst_file_loader;
class map_file_loader;

// Other I/O classes
class dna_scalar;
class dna_file_handler;
class dynadjust_file;

// PDF classes
class dna_io_pdf;
class dna_pdf_file_parameters;

}  // namespace iostreams
}  // namespace dynadjust

#endif // IO_FWD_H_