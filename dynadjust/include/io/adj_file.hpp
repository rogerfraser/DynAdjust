//============================================================================
// Name         : adj_file.hpp
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
// Description  : DynAdjust adjustment output file io operations
//============================================================================

#ifndef DYNADJUST_ADJ_FILE_H_
#define DYNADJUST_ADJ_FILE_H_

#if defined(_MSC_VER)
	#if defined(LIST_INCLUDES_ON_BUILD) 
		#pragma message("  " __FILE__) 
	#endif
#endif

#include <include/io/dynadjust_file.hpp>
#include <include/config/dnatypes.hpp>

namespace dynadjust {
namespace iostreams {

class DNATYPE_API AdjFile : public DynadjustFile
{
public:
	AdjFile(void) {};
	AdjFile(const AdjFile& adj) : DynadjustFile(adj) {};
	virtual ~AdjFile(void) {};

	AdjFile& operator=(const AdjFile& rhs);

	void print_stn_info_col_header(std::ostream& os, const std::string& stn_coord_types, const UINT16& printStationCorrections=false);
	void print_adj_stn_header(std::ostream& os);
	void print_adj_stn_block_header(std::ostream& os, const UINT32& block);

protected:

};

}	// namespace iostreams
}	// namespace dynadjust

#endif  // DYNADJUST_ADJ_FILE_H_
