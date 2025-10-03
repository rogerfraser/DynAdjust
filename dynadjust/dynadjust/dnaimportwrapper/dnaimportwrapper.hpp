//============================================================================
// Name         : dnaimportwrapper.hpp
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
// Description  : DynAdjust Interoperability library Executable
//============================================================================

#ifndef DNAIMPORTWRAPPER_H_
#define DNAIMPORTWRAPPER_H_

#if defined(_MSC_VER)
#if defined(LIST_INCLUDES_ON_BUILD)
#pragma message("  " __FILE__)
#endif
#endif

/// \cond
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <iosfwd>
#include <mutex>
#include <string>
/// \endcond

// cpu_timer is defined in dnatimer.hpp, no need for forward declaration

#include <dynadjust/dnaimport/dnainterop.hpp>
#include <include/config/dnaconsts-interface.hpp>
#include <include/config/dnaoptions-interface.hpp>
#include <include/config/dnaprojectfile.hpp>
#include <include/functions/dnatimer.hpp>

extern bool running;
extern std::mutex cout_mutex;

class dna_import_thread {
   public:
    dna_import_thread(dynadjust::dynamlinterop::dna_import* dnaParse, project_settings* p, const std::string& filename,
                      vdnaStnPtr* vStations, PUINT32 stnCount, vdnaMsrPtr* vMeasurements, PUINT32 msrCount,
                      PUINT32 clusterID, input_file_meta_t* input_file_meta, bool firstFile, std::string* status_msg,
                      boost::posix_time::milliseconds* ms);
    void operator()();
    void SetFile(const std::string& file);

   private:
    dynadjust::dynamlinterop::dna_import* _dnaParse;
    project_settings* _p;
    std::string _filename;
    vdnaStnPtr* _vStations;
    PUINT32 _stnCount;
    vdnaMsrPtr* _vMeasurements;
    PUINT32 _msrCount;
    PUINT32 _clusterID;
    input_file_meta_t* _input_file_meta;
    bool _firstFile;
    std::string* _status_msg;
    boost::posix_time::milliseconds* _ms;
};

class dna_import_progress_thread {
   public:
    dna_import_progress_thread(dynadjust::dynamlinterop::dna_import* dnaParse, project_settings* p);
    void operator()();

   private:
    dynadjust::dynamlinterop::dna_import* _dnaParse;
    project_settings* _p;
};

#endif
