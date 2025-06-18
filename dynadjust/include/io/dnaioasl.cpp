//============================================================================
// Name         : dnaioasl.cpp
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
// Description  : DynAdjust associated station file io operations
//============================================================================

#include <include/io/dnaioasl.hpp>
#include <include/functions/dnaiostreamfuncs.hpp>
#include <include/functions/dnatemplatestnmsrfuncs.hpp>
#include <include/functions/dnatemplatefuncs.hpp>

namespace dynadjust { 
namespace iostreams {

dna_io_asl& dna_io_asl::operator=(const dna_io_asl& rhs)
{
	if (this == &rhs)
		return *this;

	dna_io_base::operator=(rhs);
	return *this;
}

// open the asl file. the asl file is required to ensure invalid stations are
// excluded from the adjustment. invalid stations are those with no measurements.
std::uint64_t dna_io_asl::load_asl_file(const std::string& asl_filename, vASL* vbinary_asl, vUINT32* vfree_stn)
{	
	std::ifstream asl_file;
	std::ostringstream os;
	os << "load_asl_file(): An error was encountered when opening " << asl_filename << "." << std::endl;

	try {
		// open stations asl file.  Throws runtime_error on failure.
		file_opener(asl_file, asl_filename, std::ios::in | std::ios::binary, binary, true);
	}
	catch (const std::runtime_error& e) {
		os << e.what();
		throw std::runtime_error(os.str());
	}
	catch (...) {
		throw std::runtime_error(os.str());
	}
	
	os.str("");
	os << "load_asl_file(): An error was encountered when reading from " << asl_filename << "." << std::endl;

	std::uint64_t stnCount;
	
	try {
		// read the file information
		readFileInfo(asl_file);
		
		// Get number of records and resize AML vector
		asl_file.read(reinterpret_cast<char *>(&stnCount), sizeof(std::uint64_t));
		
		// initialise free station list
		initialiseIncrementingIntegerVector(vfree_stn, static_cast<UINT32>(stnCount));
		
		// initialise assocaiated station list
		vbinary_asl->clear();
		vbinary_asl->resize(stnCount);
		
		for (auto& asl_entry : *vbinary_asl) {
			asl_file >> &asl_entry;
		}

	}
	catch (const std::ios_base::failure& f) {
		os << f.what();
		throw std::runtime_error(os.str());
	}
	catch (const std::runtime_error& e) {
		os << e.what();
		throw std::runtime_error(os.str());
	}
	catch (...) {
		throw std::runtime_error(os.str());
	}

	asl_file.close();

	return stnCount;
}

void dna_io_asl::write_asl_file(const std::string& asl_filename, pvASLPtr vbinary_asl) 
{	
	std::ofstream asl_file;
	std::ostringstream os;
	os << "write_asl_file(): An error was encountered when opening " << asl_filename << "." << std::endl;

	try {
		// create binary asl file.  Throws runtime_error on failure.
		file_opener(asl_file, asl_filename,
			std::ios::out | std::ios::binary, binary);
	}
	catch (const std::runtime_error& e) {
		os << e.what();
		throw std::runtime_error(os.str());
	}
	catch (...) {
		throw std::runtime_error(os.str());
	}
	
	os.str("");
	os << "write_asl_file(): An error was encountered when writing to " << asl_filename << "." << std::endl;

	// Calculate number of station records and write to binary file
	std::uint64_t aslCount = static_cast<std::uint64_t>(vbinary_asl->size());
	
	try {
		// write the file information 
		writeFileInfo(asl_file);
		
		// write the data
		asl_file.write(reinterpret_cast<char *>(&aslCount), sizeof(std::uint64_t));
		for (const auto& asl_entry : *vbinary_asl) {
			if (asl_entry.get()) {
				asl_file << asl_entry.get();
			}
		}			// see dnastation.hpp (region CAStationList stream handlers)
	}
	catch (const std::ios_base::failure& f) {
		os << f.what();
		throw std::runtime_error(os.str());
	}
	catch (const std::runtime_error& e) {
		os << e.what();
		throw std::runtime_error(os.str());
	}
	catch (...) {
		throw std::runtime_error(os.str());
	}
	
	asl_file.close();
}

// prints ASL entries sorted according to measurement count, which is useful when determining 
// the starting station to begin network segmentation.
void dna_io_asl::write_asl_file_txt(const std::string& asl_filename, pvASLPtr vbinary_asl, vdnaStnPtr* vStations) 
{	
	std::ofstream asl_file;
	std::ostringstream os;
	os << "write_asl_file_txt(): An error was encountered when opening " << asl_filename << "." << std::endl;

	try {
		// create binary asl file.  Throws runtime_error on failure.
		file_opener(asl_file, asl_filename);
	}
	catch (const std::runtime_error& e) {
		os << e.what();
		throw std::runtime_error(os.str());
	}
	catch (...) {
		throw std::runtime_error(os.str());
	}
	
	os.str("");
	os << "write_asl_file_txt(): An error was encountered when writing to " << asl_filename << "." << std::endl;

	// Calculate number of station records and write to binary file
	size_t aslCount = vbinary_asl->size();
	
	// Pump to text file
	std::stringstream ss_asl;
	ss_asl << aslCount << " stations";
	asl_file << std::left << std::setw(STATION) << ss_asl.str();
	asl_file << std::setw(HEADER_20) << std::right << "No. connected msrs";
	asl_file << std::setw(STATION) << std::right << "AML index";
	asl_file << std::setw(STATION) << std::right << "Unused?" << std::endl;

	// Create an incrementing list and sort on number of measurements to each station
	vUINT32 aslPtrs(vbinary_asl->size());
	initialiseIncrementingIntegerVector<UINT32>(aslPtrs, static_cast<UINT32>(vbinary_asl->size()));

	// Sort on measurement count
	CompareMeasCount2<ASLPtr, UINT32> msrcountCompareFunc(vbinary_asl);
	std::sort(aslPtrs.begin(), aslPtrs.end(), msrcountCompareFunc);

	vbinary_asl->at(0).get()->GetAMLStnIndex();

	try {

		for (const auto& asl_ptr : aslPtrs) {
			if (!vbinary_asl->at(asl_ptr))	// unused station
				continue;

			auto _it_pasl = vbinary_asl->begin() + asl_ptr;
			
			// Name and measurement count
			asl_file << std::setw(STATION) << std::left << 
				vStations->at(asl_ptr)->GetName() << 
				std::setw(HEADER_20) << std::right << _it_pasl->get()->GetAssocMsrCount();
			// Aml station index
			if (_it_pasl->get()->GetAssocMsrCount() == 0)
				asl_file << std::setw(STATION) << "-";
			else
				asl_file << std::setw(STATION) << std::right << _it_pasl->get()->GetAMLStnIndex();
			// Valid station?
			asl_file << std::setw(STATION) << std::right << (_it_pasl->get()->IsValid() ? " " : "*") << std::endl;
		}
	}
	catch (const std::ios_base::failure& f) {
		os << f.what();
		throw std::runtime_error(os.str());
	}
	catch (const std::runtime_error& e) {
		os << e.what();
		throw std::runtime_error(os.str());
	}
	catch (...) {
		throw std::runtime_error(os.str());
	}
	
	asl_file.close();
}

} // dnaiostreams
} // dynadjust
