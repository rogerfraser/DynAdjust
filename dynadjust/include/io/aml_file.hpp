//============================================================================
// Name         : aml_file.hpp
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
// Description  : DynAdjust associated measurement file io operations
//============================================================================

#ifndef DYNADJUST_AML_FILE_H_
#define DYNADJUST_AML_FILE_H_

#if defined(_MSC_VER)
	#if defined(LIST_INCLUDES_ON_BUILD) 
		#pragma message("  " __FILE__) 
	#endif
#endif

#include <include/io/dynadjust_file.hpp>
#include <include/io/bms_file.hpp>

namespace dynadjust {
namespace iostreams {

class AmlFile : public DynadjustFile
{
public:
	AmlFile(void) {};
	AmlFile(const AmlFile& aml) : DynadjustFile(aml) {};
	virtual ~AmlFile(void) {};

	AmlFile& operator=(const AmlFile& rhs);

	void load_aml_file(const std::string& aml_filename, v_aml_pair* vbinary_aml, measurements::pvmsr_t bmsRecords);
	void write_aml_file(const std::string& aml_filename, pvUINT32 vbinary_aml);
	void write_aml_file_txt(const std::string& bms_filename, const std::string& aml_filename, pvUINT32 vbinary_aml, const measurements::pvASLPtr vAssocStnList, measurements::vdnaStnPtr* vStations);

	void create_msr_to_stn_tally(const measurements::pvASLPtr vAssocStnList, v_aml_pair& vAssocMsrList, 
		measurements::vmsrtally& stnmsrTally, measurements::vmsr_t& bmsBinaryRecords);

	void write_msr_to_stn(std::ostream &os, pvstn_t bstBinaryRecords, 
		pvUINT32 vStationList, measurements::vmsrtally& v_stnmsrTally, measurements::MsrTally* parsemsrTally);
	
protected:

};

}	// namespace measurements
}	// namespace dynadjust

#endif  // DYNADJUST_AML_FILE_H_
