//============================================================================
// Name         : dnaadjust_threads.hpp
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
// Description  : Thread helper classes for DynAdjust Network Adjustment
//============================================================================

#ifndef DNAADJUST_THREADS_H_
#define DNAADJUST_THREADS_H_

/// \cond
#include <exception>
#include <vector>
/// \endcond

#include <include/config/dnatypes-fwd.hpp>
#include <dynadjust/dnaadjust/dnaadjust_fwd.hpp>

namespace dynadjust {
namespace networkadjust {

class adjust_prepare_thread {
  public:
    adjust_prepare_thread(dna_adjust* dnaAdj, std::exception_ptr& error)
        : main_adj_(dnaAdj), error_(error) {}
    void operator()();

  private:
    dna_adjust* main_adj_;
    std::exception_ptr& error_;

    // Prevent assignment operator
    adjust_prepare_thread& operator=(const adjust_prepare_thread& rhs);
};

class adjust_process_prepare_thread {
  public:
    adjust_process_prepare_thread(dna_adjust* dnaAdj, const UINT32& id,
                                  std::exception_ptr& error,
                                  std::vector<std::exception_ptr>& prep_errors)
        : main_adj_(dnaAdj), thread_id_(id), error_(error),
          prep_errors_(prep_errors) {}
    void operator()();

  private:
    dna_adjust* main_adj_;
    UINT32 thread_id_;
    std::exception_ptr& error_;
    std::vector<std::exception_ptr>& prep_errors_;

    // Prevent assignment operator
    adjust_process_prepare_thread&
    operator=(const adjust_process_prepare_thread& rhs);
};

class adjust_forward_thread {
  public:
    adjust_forward_thread(dna_adjust* dnaAdj, std::exception_ptr& error)
        : main_adj_(dnaAdj), error_(error) {}
    void operator()();

  private:
    dna_adjust* main_adj_;
    std::exception_ptr& error_;

    // Prevent assignment operator
    adjust_forward_thread& operator=(const adjust_forward_thread& rhs);
};

class adjust_reverse_thread {
  public:
    adjust_reverse_thread(dna_adjust* dnaAdj, std::exception_ptr& error)
        : main_adj_(dnaAdj), error_(error) {}
    void operator()();

  private:
    dna_adjust* main_adj_;
    std::exception_ptr& error_;

    // Prevent assignment operator
    adjust_reverse_thread& operator=(const adjust_reverse_thread& rhs);
};

class adjust_combine_thread {
  public:
    adjust_combine_thread(dna_adjust* dnaAdj, std::exception_ptr& error)
        : main_adj_(dnaAdj), error_(error) {}
    void operator()();

  private:
    dna_adjust* main_adj_;
    std::exception_ptr& error_;

    // Prevent assignment operator
    adjust_combine_thread& operator=(const adjust_combine_thread& rhs);
};

class adjust_process_combine_thread {
  public:
    adjust_process_combine_thread(dna_adjust* dnaAdj, const UINT32& id,
                                  std::exception_ptr& error,
                                  std::vector<std::exception_ptr>& cmb_errors)
        : main_adj_(dnaAdj), thread_id_(id), error_(error),
          cmb_errors_(cmb_errors) {}
    void operator()();

  private:
    dna_adjust* main_adj_;
    UINT32 thread_id_;
    std::exception_ptr& error_;
    std::vector<std::exception_ptr>& cmb_errors_;

    // Prevent assignment operator
    adjust_process_combine_thread&
    operator=(const adjust_process_combine_thread& rhs);
};

} // namespace networkadjust
} // namespace dynadjust

#endif // DNAADJUST_THREADS_H_