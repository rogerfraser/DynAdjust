#pragma once
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct NetworkState {
  UINT32 bstn_count, asl_count, bmsr_count;
  UINT32 unknownParams, unknownsCount, measurementParams, measurementCount;
  vUINT32 v_measurementCount, v_measurementVarianceCount, v_measurementParams;
  vUINT32 v_unknownsCount;
  vvUINT32 v_ISL, v_CML;
  v_uint32_uint32_map v_blockStationsMap;
};

namespace detail {

template <typename T>
bool diff_scalar(const T &a, const T &b, const std::string &path,
                 std::ostream &out) {
  if (a == b)
    return true;
  out << path << " : " << a << "  ≠  " << b << '\n';
  return false;
}

template <typename Vec>
bool diff_vector(const Vec &a, const Vec &b, const std::string &path,
                 std::ostream &out) {
  bool same = true;
  if (a.size() != b.size()) {
    out << path << ".size() : " << a.size() << "  ≠  " << b.size() << '\n';
    same = false;
  }
  const std::size_t n = std::min(a.size(), b.size());
  for (std::size_t i = 0; i < n; ++i)
    same &= diff_scalar(a[i], b[i], path + '[' + std::to_string(i) + ']', out);
  return same;
}

template <typename VecOfVec>
bool diff_vector2(const VecOfVec &a, const VecOfVec &b, const std::string &path,
                  std::ostream &out) {
  bool same = true;
  if (a.size() != b.size()) {
    out << path << ".size() : " << a.size() << "  ≠  " << b.size() << '\n';
    same = false;
  }
  const std::size_t m = std::min(a.size(), b.size());
  for (std::size_t i = 0; i < m; ++i)
    same &= diff_vector(a[i], b[i], path + '[' + std::to_string(i) + ']', out);
  return same;
}

inline bool diff_map(const uint32_uint32_map &a, const uint32_uint32_map &b,
                     const std::string &path, std::ostream &out) {
  bool same = true;
  if (a.size() != b.size()) {
    out << path << ".size() : " << a.size() << "  ≠  " << b.size() << '\n';
    same = false;
  }

  auto ia = a.begin(), ib = b.begin();
  for (; ia != a.end() && ib != b.end(); ++ia, ++ib) {
    same &= diff_scalar(ia->first, ib->first, path + "{key}", out);
    same &= diff_scalar(ia->second, ib->second,
                        path + '{' + std::to_string(ia->first) + '}', out);
  }
  return same;
}

inline bool diff_vector_of_maps(const v_uint32_uint32_map &a,
                                const v_uint32_uint32_map &b,
                                const std::string &path, std::ostream &out) {
  bool same = true;
  if (a.size() != b.size()) {
    out << path << ".size() : " << a.size() << "  ≠  " << b.size() << '\n';
    same = false;
  }
  const std::size_t n = std::min(a.size(), b.size());
  for (std::size_t i = 0; i < n; ++i)
    same &= diff_map(a[i], b[i], path + '[' + std::to_string(i) + ']', out);
  return same;
}

} // namespace detail

inline bool diff_network_state(const NetworkState &lhs, const NetworkState &rhs,
                               std::ostream &out = std::cerr) {
  using namespace detail;
  bool same = true;

  same &= diff_scalar(lhs.bstn_count, rhs.bstn_count, "bstn_count", out);
  same &= diff_scalar(lhs.asl_count, rhs.asl_count, "asl_count", out);
  same &= diff_scalar(lhs.bmsr_count, rhs.bmsr_count, "bmsr_count", out);
  same &=
      diff_scalar(lhs.unknownParams, rhs.unknownParams, "unknownParams", out);
  same &=
      diff_scalar(lhs.unknownsCount, rhs.unknownsCount, "unknownsCount", out);
  same &= diff_scalar(lhs.measurementParams, rhs.measurementParams,
                      "measurementParams", out);
  same &= diff_scalar(lhs.measurementCount, rhs.measurementCount,
                      "measurementCount", out);

  same &= diff_vector(lhs.v_measurementCount, rhs.v_measurementCount,
                      "v_measurementCount", out);
  same &= diff_vector(lhs.v_measurementVarianceCount,
                      rhs.v_measurementVarianceCount,
                      "v_measurementVarianceCount", out);
  same &= diff_vector(lhs.v_measurementParams, rhs.v_measurementParams,
                      "v_measurementParams", out);
  same &= diff_vector(lhs.v_unknownsCount, rhs.v_unknownsCount,
                      "v_unknownsCount", out);

  same &= diff_vector2(lhs.v_ISL, rhs.v_ISL, "v_ISL", out);
  same &= diff_vector2(lhs.v_CML, rhs.v_CML, "v_CML", out);

  same &= diff_vector_of_maps(lhs.v_blockStationsMap, rhs.v_blockStationsMap,
                              "v_blockStationsMap", out);

  if (same)
    out << "NetworkState objects are identical\n";
  else
    out << "NetworkState objects differ\n";

  return same;
}
