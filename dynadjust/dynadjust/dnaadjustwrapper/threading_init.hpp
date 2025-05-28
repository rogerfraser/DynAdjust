#pragma once
#include <algorithm>
#include <cstdlib>

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#if defined(_OPENMP)
#include <omp.h>
#endif

#if defined(MKL_VERSION) || defined(INTEL_MKL_VERSION) || defined(__INTEL_MKL__)
#define USE_MKL
#include <mkl.h>

#elif defined(OPENBLAS_VERSION) || defined(__OPENBLAS_CONFIG_H) || defined(OPENBLAS_LOPT_H)
#define USE_OPENBLAS
#include <cblas.h>
#include <openblas_config.h>
extern "C" { void openblas_set_num_threads(int); }

#elif defined(__APPLE__)
#include <Accelerate/Accelerate.h>
#endif

inline int init_linear_algebra_threads(int requested_threads = 0) {
    int n = requested_threads;

#if defined(_OPENMP)
    if (n <= 0) {
        if (const char* env = std::getenv("OMP_NUM_THREADS"); env && *env) {
            int v = std::atoi(env);
            if (v > 0) n = v;
        }
    }
    if (n <= 0) n = std::max(1, omp_get_max_threads());
#elif defined(__APPLE__)
    if (n == 1) {
        BLASSetThreading(BLAS_THREADING_SINGLE_THREADED);
    } else {
        BLASSetThreading(BLAS_THREADING_MULTI_THREADED);
    }
#else
    if (n <= 0) n = 1;
#endif

#if defined(_OPENMP)
    omp_set_dynamic(0);
#if _OPENMP >= 201511
    omp_set_max_active_levels(1);
#else
    omp_set_nested(0);
#endif
    omp_set_num_threads(n);
#endif

#ifdef USE_MKL
    mkl_set_dynamic(0);
    mkl_set_num_threads(n);
#if defined(MKL_THREAD_LOCAL)
    mkl_set_num_threads_local(n);
#endif
#elif defined(USE_OPENBLAS)
    openblas_set_num_threads(n);
#endif

    return n;
}
