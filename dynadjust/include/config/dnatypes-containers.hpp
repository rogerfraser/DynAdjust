//============================================================================
// Name         : dnatypes-containers.hpp
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
// Description  : STL container typedefs for DynAdjust
//============================================================================

#pragma once
#ifndef DNATYPES_CONTAINERS_H_
#define DNATYPES_CONTAINERS_H_

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <include/config/dnatypes-basic.hpp>

// Basic STL container typedefs
typedef std::vector<char> vchar;
typedef vchar::iterator _it_chr;

typedef std::vector<double> vdouble;
typedef vdouble::iterator _it_dbl;

typedef std::vector<bool> vbool;
typedef vbool::iterator _it_bool;
typedef std::vector<bool>::reference boolRef;

typedef std::string::iterator _it_str;
typedef std::string::const_iterator _it_str_const;
typedef std::vector<std::string> vstring, *pvstring;
typedef vstring::iterator _it_vstr;
typedef vstring::const_iterator _it_vstr_const;

// Pair typedefs for iterators
typedef std::pair<vchar::iterator, vchar::iterator> _it_pair_vchar;
typedef std::pair<vstring::iterator, vstring::iterator> _it_pair_vstring;

// Queue types
typedef std::queue<UINT32> qUINT32;

// UINT32 vector types
typedef std::vector<UINT32> vUINT32, *pvUINT32;
typedef std::vector<vUINT32> vvUINT32, *pvvUINT32;

typedef vUINT32::iterator it_vUINT32;
typedef vUINT32::const_iterator it_vUINT32_const;
typedef vvUINT32::iterator it_vvUINT32;

// Pair types - strings and UINT32
typedef std::pair<std::string, std::string> string_string_pair;
typedef std::pair<std::string, UINT32> string_uint32_pair;
typedef std::pair<UINT32, UINT32> uint32_uint32_pair;
typedef std::pair<UINT32, std::string> uint32_string_pair;

typedef std::pair<bool, UINT32> bool_uint32_pair;

typedef std::pair<std::string, vstring> string_vstring_pair;

typedef std::pair<uint32_uint32_pair, UINT32> u32u32_uint32_pair;
typedef std::pair<UINT32, uint32_uint32_pair> uint32_u32u32_pair;

// Double pair types
typedef std::pair<double, double> doubledouble_pair;
typedef std::pair<string_string_pair, doubledouble_pair> stringstring_doubledouble_pair;
typedef std::vector<stringstring_doubledouble_pair> v_stringstring_doubledouble_pair, *pv_stringstring_doubledouble_pair;
typedef v_stringstring_doubledouble_pair::iterator _it_string_doubledouble_pair;

typedef std::vector<doubledouble_pair> v_doubledouble_pair, *pv_doubledouble_pair;
typedef std::pair<std::string, v_doubledouble_pair> string_v_doubledouble_pair;
typedef std::vector<string_v_doubledouble_pair> v_string_v_doubledouble_pair, *pv_string_v_doubledouble_pair;
typedef v_string_v_doubledouble_pair::iterator it_v_string_v_doubledouble_pair;

typedef std::pair<uint32_uint32_pair, double> u32u32_double_pair;

typedef std::pair<std::string, bool> stringbool_pair;
typedef std::pair<string_uint32_pair, stringbool_pair> stringuint32_stringbool_pair;

// Map types
typedef std::map<UINT32, UINT32> uint32_uint32_map;
typedef uint32_uint32_map::iterator it_uint32_uint32_map;
typedef std::vector<uint32_uint32_map> v_uint32_uint32_map;
typedef v_uint32_uint32_map::iterator it_v_uint32_uint32_map;

// Vector of pair types
typedef std::vector<string_string_pair> v_string_string_pair, *pv_string_string_pair;
typedef std::vector<string_vstring_pair> v_string_vstring_pair, *pv_string_vstring_pair;

typedef std::vector<string_uint32_pair> v_string_uint32_pair, *pv_string_uint32_pair;
typedef std::vector<uint32_uint32_pair> v_uint32_uint32_pair, *pv_uint32_uint32_pair;
typedef std::vector<uint32_string_pair> v_uint32_string_pair, *pv_uint32_string_pair;
typedef std::vector<u32u32_uint32_pair> v_u32u32_uint32_pair;
typedef std::vector<uint32_u32u32_pair> v_uint32_u32u32_pair;

// Iterators for vector of pairs
typedef v_string_vstring_pair::iterator _it_string_vstring_pair;
typedef v_string_uint32_pair::iterator _it_string_uint32_pair;
typedef v_uint32_string_pair::iterator _it_uint32_string_pair;
typedef v_u32u32_uint32_pair::iterator _it_u32u32_uint32_pair;
typedef v_uint32_u32u32_pair::iterator _it_uint32_u32u32_pair;
typedef v_uint32_uint32_pair::iterator _it_uint32_uint32_pair;

typedef std::pair<_it_u32u32_uint32_pair, _it_u32u32_uint32_pair> _it_pair_u32u32_uint32;

typedef std::vector<v_string_string_pair> vv_string_string_pair;

typedef std::pair<it_vUINT32_const, it_vUINT32_const> _it_pair_vUINT32_const;
typedef std::pair<it_vUINT32, it_vUINT32> it_pair_vUINT32;

typedef v_string_string_pair::iterator it_string_pair;
typedef std::pair<v_string_string_pair::iterator, v_string_string_pair::iterator> it_pair_string;
typedef std::pair<_it_string_uint32_pair, _it_string_uint32_pair> it_pair_string_vUINT32;
typedef std::pair<_it_uint32_string_pair, _it_uint32_string_pair> it_pair_uint32_string;

typedef std::pair<uint32_uint32_map::iterator, uint32_uint32_map::iterator> it_pair_map_vUINT32_vUINT32;

#endif // DNATYPES_CONTAINERS_H_