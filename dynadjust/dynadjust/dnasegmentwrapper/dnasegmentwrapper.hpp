//============================================================================
// Name         : dnasegmentwrapper.hpp
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
// Description  : DynAdjust Network Segmentation Executable
//============================================================================

#ifndef DNASEGMENTWRAPPER_H_
#define DNASEGMENTWRAPPER_H_

#if defined(_MSC_VER)
	#if defined(LIST_INCLUDES_ON_BUILD) 
		#pragma message("  " __FILE__) 
	#endif
#endif

/// \cond
#include <string>
#include <mutex>
// Minimal boost include for milliseconds typedef only
#include <boost/date_time/posix_time/posix_time_duration.hpp>
/// \endcond

// Forward declarations
struct project_settings;

// Need this for _SEGMENT_STATUS_ enum only
#include <include/exception/dnaexception.hpp>

namespace dynadjust {
namespace networksegment {
class dna_segment;
}
}

extern bool running;
extern std::mutex cout_mutex;

class dna_segment_thread
{
public:
	dna_segment_thread(dynadjust::networksegment::dna_segment* dnaSeg, project_settings* p,
		_SEGMENT_STATUS_* segmentStatus, boost::posix_time::milliseconds* s, std::string* status_msg);
	void operator()();
private:
	dynadjust::networksegment::dna_segment* _dnaSeg;
	project_settings* _p;
	_SEGMENT_STATUS_* _segmentStatus;
	boost::posix_time::milliseconds* _s;
	std::string* _status_msg;
};

class dna_segment_progress_thread
{
public:
	dna_segment_progress_thread(dynadjust::networksegment::dna_segment* dnaSeg, project_settings* p);
	void operator()();

private:
	dynadjust::networksegment::dna_segment* _dnaSeg;
	project_settings* _p;
};

#endif
