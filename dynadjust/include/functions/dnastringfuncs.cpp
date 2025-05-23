//============================================================================
// Name         : dnastringfuncs.cpp
// Author       : Roger Fraser
// Contributors :
// Version      : 1.00
// Copyright    : Copyright 2017 Geoscience Australia
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
// Description  : Common String Functions
//============================================================================

#include <include/functions/dnastringfuncs.hpp>
#include <include/config/dnaversion-stream.hpp>
#include <include/functions/dnatemplatedatetimefuncs.hpp>

// Returns the number of fields with valid data
int GetFields(char *line, char delim, bool multiple_delim_as_one, const char *fmt, ...)
{
	va_list ap;
	size_t length = strlen(line);

	// 1. determine "expected" number of fields
	size_t field_count = strlen(fmt);
	size_t* field = new size_t[field_count+1];
	
	size_t i(0), j(0);

	// 2. Determine indices of the beginning of each field
	
	// first field commences at 0
	field[j++] = 0;
	
	// 3. Determine indices of remaining fields, but don't let j
	//    go beyond the number of 'wanted' fields (field_count)
	while (line[i] != '\n' && i < length && j <= field_count)
	//while (line[i] != '\n' && i < length)
	//while(line[i] != '\n' && line[i] != '\0')
	{
		// replace all delimiters with NULL
		if (line[i] == delim)
		{
			if (multiple_delim_as_one && i < length-1)
			{
				if (line[i+1] == delim)
				{
					i++;
					continue;
				}
			}
			line[i] = '\0';
			field[j++] = i+1;
		}
		i++;
	}

	// 4. terminate end of the wanted string with null
	line[i] = '\0';	
	i = 0;
	va_start(ap, fmt);

	int success(0);

	char* p;

	// 5. get the data
	while (i < j && *fmt != '\0') 
	{
		p = &line[field[i]];
		
		switch(*fmt) {
		case 'd':
			if (strlen(p) == 0)
				*(va_arg(ap, int *)) = INT_MIN;
			else
			{
				success++;
				*(va_arg(ap, int *)) = atoi(p);
			}
			break;
		case 'f':
			if (strlen(p) == 0)
				*(va_arg(ap, double *)) = MAX_DBL_VALUE;
			else
			{
				success++;
				*(va_arg(ap, double *)) = atof(p);
			}
			break;
		case 's':
			if (strlen(p) != 0)
				success++;
			strcpy((va_arg(ap, char *)), p);
			break;
		default :
			delete[] field;
			return 0;
		}
		i++;
		fmt++;
	}

	va_end(ap);
	delete[] field;

	// 6. Return number of fields with non-empty data
	return success;
}
	
// Prints a summary of the software, like the following:
//    +---------------------------------------------------------------------------
//    + Title:        adjust
//    + Description : Geodetic network adjustment software
//    + Version :     1.0.0, Release(64 - bit)
//    + Build :       May 14 2018, 10 : 30 : 07 (MSVC++ 14.13, VS2017)
//    + Copyright :   (C)2018 Geoscience Australia.
//                    This software is released under the Apache License.
//    + Contact :     geodesy@ga.gov.au
//    + ---------------------------------------------------------------------------
//
void fileproc_help_header(std::string* msg)
{
	std::stringstream str;

	str << "+---------------------------------------------------------------------------" << std::endl;
	
	str << "+ ";
	output_binaryname(str);
	str << " - ";
	output_theappname(str);
	str << ".";
	str << std::endl;

	str << "+ ";
	output_binarydescription(str);
	str << std::endl;
	
	str << "+ ";
	output_version(str);
	str << std::endl;
	
	str << "+ ";
	output_build(str);
	str << std::endl;
	
	*msg = str.str();
	
	*msg += "+ Copyright:    (C) ";
	*msg += __COPYRIGHT_YEAR__;
	*msg += " ";
	*msg += __COPYRIGHT_OWNER__;
	*msg += ".\n";
	*msg += "                ";
	*msg += __COPYRIGHT_MSG__;
	*msg += "\n";
	*msg += "+ Contact:      ";
	*msg += __CONTACT_EMAIL__;
	*msg += "\n";
	*msg += "+---------------------------------------------------------------------------\n\n";
}

	
void dynaml_header(std::ostream& os, const std::string& fileType, const std::string& referenceFrame, const std::string& epoch)
{
	os << "<?xml version=\"1.0\"?>" << std::endl;
	os << "<DnaXmlFormat type=\"" << fileType << 
		"\" referenceframe=\"" << referenceFrame <<
		"\" epoch=\"" << epoch <<
		"\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"DynaML.xsd\">" << std::endl;
	
	os << "<!-- ";
	output_createdby(os);
	os << " -->" << std::endl;
	os << "<!-- ";
	output_version(os);
	os << " -->" << std::endl;
	os << "<!-- ";
	output_build(os);
	os << " -->" << std::endl;
}
	

void dynaml_footer(std::ostream& os)
{
	os << "</DnaXmlFormat>" << std::endl;
}
	
	
void dynaml_comment(std::ostream& os, const std::string& comment)
{
	os << "<!-- " << comment << " -->" << std::endl;
}

std::string snx_softwarehardware_text()
{
	std::stringstream str, str2;
	
	// name
	output_globalname(str2);
	str << " SOFTWARE           " << std::left << std::setw(60) << str2.str() << std::endl;
	
	// version
	str2.str("");
	output_version(str2);
	str << " SOFTWARE           " << std::left << std::setw(60) << str2.str() << std::endl;
	
	// build
	str2.str("");
	output_build(str2);	
	str << " SOFTWARE           " << std::left << std::setw(60) << str2.str() << std::endl;
	
	// hardware
	str << " HARDWARE           " << std::left << std::setw(60) << __HARDWARE__ << std::endl;


	return str.str();
}
	

void dna_header(std::ostream& os, const std::string& fileVersion, const std::string& fileType, const std::string& reference_frame, const std::string& epoch_version, const size_t& count)
{
	std::stringstream str;
	str << "!#=DNA " << fileVersion << " " << fileType;

	boost::gregorian::date today(boost::gregorian::day_clock::local_day());
	
	// creation date
	str << std::setw(14) << std::right << stringFromDate<boost::gregorian::date>(today);
	// default reference frame
	str << std::setw(14) << std::right << reference_frame;
	// default epoch
	str << std::setw(14) << std::right << epoch_version;
	// count of records
	str << std::setw(10) << std::right << count;

	os << str.str() << std::endl;

	// Provide comments on the version of DynAdjust that is creating this file
	os << "* ";
	output_createdby(os);
	os << ". " << std::endl;
	os << "* ";
	output_version(os);
	os << ". " << std::endl;
	os << "* ";
	output_build(os);
	os << std::endl;
}
	

void dna_comment(std::ostream& os, const std::string& comment)
{
	os << "* " << comment << std::endl;
}

void dnaproj_header(std::ostream& os, const std::string& comment)
{
	// DynAdjust version
	os << "# " << comment << ". ";
	output_createdby(os);
	os << ". ";
	output_version(os);
	os << ". ";
	output_build(os);
}

void dnaproj_comment(std::ostream& os, const std::string& comment)
{
	os << "# " << comment << std::endl;
}

