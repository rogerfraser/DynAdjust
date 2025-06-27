//============================================================================
// Name         : dnatemplatefuncs.hpp
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
// Description  : Basic template functions using standard data types
//============================================================================

#ifndef DNATEMPLATEFUNCS_H_
#define DNATEMPLATEFUNCS_H_

#if defined(_MSC_VER)
	#if defined(LIST_INCLUDES_ON_BUILD) 
		#pragma message("  " __FILE__) 
	#endif
#endif

/// \cond
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/bind/bind.hpp>
#include <boost/algorithm/string.hpp>
/// \endcond

#include <include/config/dnatypes.hpp>
#include <include/functions/dnastrmanipfuncs.hpp>

template <typename T>
bool isOdd(T integer)
{
	return (integer %2 == 0);
}

template <typename T>
bool is_digit(const T& s)
{
    return (!s.empty() && 
		std::find_if(s.begin(), s.end(), [](unsigned char c) { 
			return !std::isdigit(c); 
		}) == s.end());
}

template <typename T>
bool is_number(const T& s)
{
    return (
		s.find_first_not_of("0123456789") == std::string::npos);
}

template <typename T>
bool is_floatingpoint(const T& s)
{
	return (
		s.find_first_not_of("0123456789.") == std::string::npos);
}

#if defined(_WIN32) || defined(__WIN32__)
#if (_MSC_VER < 1600)
// copy_if is not in the C++ standaard!!!
// define a correct implementation of copy_if here
template <typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator copy_if(InputIterator begin, InputIterator end, OutputIterator dest_begin, Predicate pred)
{
	while (begin != end) {
		if (pred(*begin)) 
			*dest_begin++ = *begin;
		++begin;
	}
	return dest_begin;
}
#endif
#endif

// copy_if continues to the next element when the first return from 
// Predicate Pred is true.  This function iterates through all elements 
template <typename InputIterator, typename Predicate>
void copy_if_all_occurrences(InputIterator begin, InputIterator end, Predicate pred)
{
	InputIterator next;
	while (begin != end) {
		next = begin+1;
		while (next != end)
		{
			if (pred(*begin, *next)) 
				next++;
			else
				next = end;
		}
		++begin;
	}
}


template <typename T, typename InputIterator, typename Predicate> 
void erase_if_impl(T* t, InputIterator begin, InputIterator end, Predicate pred)
{
	t->erase(std::remove_if(begin, end, pred), end);
}

template <typename T, typename Predicate> 
void erase_if(T& t, Predicate pred)
{
	erase_if_impl(&t, t.begin(), t.end(), pred);
}

template <typename T, typename Predicate> 
void erase_if(T* t, Predicate pred)
{
	erase_if_impl(t, t->begin(), t->end(), pred);
}

template <typename T> 
void strip_duplicates(T& t)
{
	std::sort(t.begin(), t.end());	
	typename T::iterator it_newend(std::unique(t.begin(), t.end()));
	if (it_newend != t.end())
		t.resize(it_newend - t.begin());
}

template <typename T> 
void strip_duplicates(T* t)
{
	strip_duplicates(*t);
}

// template function for output to file or cout
template <class T>
void outputObject(T t, std::ostream &os) { os << t; }

//// template functor for equality of (type)value
//template <class C, typename T, typename OP>
//class operator_on_mem_t : public binary_function<C, T, bool>
//{
//public:
//	explicit operator_on_mem_t(T C:: *m, OP Op)
//		: m_Value(m), m_Operator(Op) {}
//	bool operator() (const C& cObject, const T& tValue) const {
//		return m_Operator(cObject.*m_Value, tValue); }
//private:
//	T C::*m_Value;
//	OP m_Operator;
//};
//
//// template functor helper
//template <class C, typename T, typename OP>
//operator_on_mem_t<C, T, OP> operator_on_mem(T C::*m_Value, OP op)
//{
//	return operator_on_mem_t<C, T, OP>(m_Value, op);
//}

/* function object to check the value of a map element */
template <class K, typename V>
class value_equals {
private:
	V value;
public:
	// constructor (initialize value to compare with)
	value_equals (const V& v) : value(v) {
	}
	// comparison
	bool operator() (std::pair<const K, V> elem) {
		return elem.second == value;
	}
};

// insert leading zeros into date string
template <class S = std::string>
S FormatDateString(const S& datestring)
{
	std::vector<S> tokenList;
	S delimiter(".");
	SplitDelimitedString<S>(datestring, delimiter, &tokenList);

	if (tokenList.size() < 3)
		return "";

	if (tokenList.at(0).size() < 2)
		tokenList.at(0).insert(0, "0");
	if (tokenList.at(1).size() < 2)
		tokenList.at(1).insert(0, "0");

	std::stringstream datess;
	datess << tokenList.at(0) << "." << tokenList.at(1) << "." << tokenList.at(2);
	return datess.str();
}


//delimiter should be a space, commar or similar
// Expected input is -38 43 28.24255
// Use of NSEW to indicate negative/positive hemispheres 
// is not supported yet.
template <class T>
T ParseDmsString(const std::string& dmsString, const std::string& delimiter)
{
	std::vector<std::string> tokenList;
	SplitDelimitedString<std::string>(dmsString, delimiter, &tokenList);

	// "-38 43 28.24255"
	// "-38"       -> 38.000000000
	T dms = DoubleFromString<T>(tokenList.at(0));
	
	if (tokenList.size() < 2)
		return dms;

	// "43"        ->  0.430000000
	//             -> 38.430000000
	T min = DoubleFromString<T>(tokenList.at(1));
	min /= 100.;
	if (dms < 0.0)
		dms -= min;
	else
		dms += min;

	if (tokenList.size() < 3)
		return dms;

	// "28.24255"  ->  0.002824255
	//             -> 38.432824255
	T sec = DoubleFromString<T>(tokenList.at(2));
	sec /= 10000.;
	if (dms < 0.0)
		dms -= sec;
	else
		dms += sec;
	
	return dms;

}

//symbols is only optional if withSpaces is true
template <class T>
std::string FormatDmsString(const T& dDegMinSec, const int precision, bool withSpaces, bool withSymbols)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(precision) << dDegMinSec;

	// No symbols or spaces? return std::string
	if (!withSpaces && !withSymbols)
		return ss.str();

	std::string strNumber(ss.str()), strBuf(ss.str());

	size_t decimal(0);
	int precision_fmt(precision);
	int minute_symbol_loc(withSymbols ? 4 : 3), second_symbol_loc(withSymbols ? 8 : 6);

	// Add symbols for degrees minutes and seconds
	if ((decimal = strNumber.find('.')) != std::string::npos)
	{
		// found a decimal point!
		if (decimal == 0)					// decimal point at start, ie >>   .0123
		{
			strBuf.insert(decimal, "0");
			decimal++;
		}

		// add spaces
		if (withSpaces)
			strBuf.replace(decimal, 1, " ");

		// add symbols
		if (withSymbols)
			strBuf.insert(decimal, "\260");		// 272 is the degree symbol

		// add zero after "tens" minute or "tens" second value
		if (precision == 1 || precision == 3)
		{
			strBuf += "0";
			precision_fmt++;
		}

		if (precision > 2)
		{
			// add spaces
			if (withSpaces)
				strBuf.insert((decimal + minute_symbol_loc), " ");

			// add symbols
			if (withSymbols)
				strBuf.insert((decimal + minute_symbol_loc), "\222");		//minutes symbol
		}

		if (precision == 2 && withSymbols)
			strBuf += "\222";

		if (precision > 4)
		{
			strBuf.insert((decimal + second_symbol_loc), ".");
			if (withSymbols)
				strBuf += "\224";
		}

		if (precision == 4 && withSymbols)
			strBuf += "\224";
	}
	else if (withSymbols)
		strBuf += "\260";		// couldn't find a decimal point, so append the degree symbol


	//// Show North and South notation
	//if (strNumber[0] == '-' || strNumber[0] == 's' || strNumber[0] == 'S' || 
	//	strNumber[0] == 'w' || strNumber[0] == 'W')
	//{
	//	if (iFlag == 1 || iFlag == 3)	// input/output latitude
	//		strBuf.replace(0, 1, "S");
	//	else							// input/output longitude
	//		strBuf.replace(0, 1, "W");
	//}
	//else
	//{
	//	if (iFlag == 1 || iFlag == 3)
	//		strBuf = "N" + strBuf;
	//	else
	//		strBuf = "E" + strBuf;
	//}
	//
	//strBuf = strBuf.Mid(0, 1) + " " + strBuf.Mid(1);


	return strBuf;
}

//symbols is only optional if withSpaces is true
template <class T>
std::string FormatDnaDmsString(const T& dDegMinSec, int precision)
{
	std::stringstream ss;
	if (precision < 4)
		precision = 4;
	ss << std::fixed << std::setprecision(precision) << dDegMinSec;
	std::string strBuf(ss.str());
	
	size_t decimal(0);
	std::string d, m, s;
		
	// Format degrees minutes and seconds
	if ((decimal = strBuf.find('.')) != std::string::npos)
	{
		// found a decimal point!
		if (decimal == 0)					// decimal point at start, ie >>   .0123
		{
			strBuf.insert(decimal, "0");
			decimal++;
		}

		// degrees
		d = strBuf.substr(0, decimal);

		// minutes
		m = strBuf.substr(decimal+1, 2);

		// seconds
		s = strBuf.substr(decimal+3);
		if (s.length() > 2)
			s.insert(2, ".");
		
		// cater for rounding
		if (s.substr(0, 2) == "60")
		{
			s.replace(0, 2, "00");
			int mm = atoi(m.c_str()) + 1;
			ss.str("");
			ss << mm;
			m = ss.str();
			if (mm < 10)
				m.insert(0, "0");
		}

		if (m.substr(0, 2) == "60")
		{
			m.replace(0, 2, "00");
			int dd = atoi(d.c_str()) + 1;
			ss.str("");
			ss << dd;
			d = ss.str();
		}
		
		ss.str("");
		ss << std::setw(3) << d << std::setw(3) << std::right << m << " " << std::setw(6) << s;
		strBuf = ss.str();
	}

	return strBuf;
}

	
// used to sort nearby stations
template <typename T = stringstring_doubledouble_pair>
class CompareStationPairs {
public:
	bool operator()(const T& left, const T& right) {
		if (left.first.first == right.first.first)
			return left.first.second < right.first.second;
		return left.first.first < right.first.first;
	}
};




// U = u32u32_uint32_pair
template<typename U=u32u32_uint32_pair>
class CompareBlockStationMapUnique_byBlock
{
public:
	bool operator()(const U& lhs, const U& rhs) {
		if (lhs.second == rhs.second)
			return lhs.first.first < rhs.first.first;
		return lhs.second < rhs.second;
	}
};

// used to sort nearby stations
template <typename U, typename T = u32u32_uint32_pair>
class CompareBlockStationMapUnique_Station {
public:
	bool operator()(const T& left, const T& right) {
		return left.first.first < right.first.first;
	}

	bool operator()(const U& left, const T& right) {
		return left < right.first.first;
	}

	bool operator()(const T& left, const U& right) {
		return left.first.first < right;
	}
};




// S = station_t, U = UINT32
template<typename S, typename U>
class CompareStnFileOrder
{
public:
	CompareStnFileOrder(std::vector<S>* s)
		:  _s(s) {}
	bool operator()(const U& lhs, const U& rhs) {
		return _s->at(lhs).fileOrder < _s->at(rhs).fileOrder;
	}
private:
	std::vector<S>*	_s;
};


// S = CDnaStation
template<typename S, typename T>
class CompareStnName_CDnaStn
{
public:
	bool operator()(const boost::shared_ptr<S> lhs, const boost::shared_ptr<S> rhs) {
		if (lhs.get()->GetName() == rhs.get()->GetName())
			return (lhs.get()->GetfileOrder() < rhs.get()->GetfileOrder());
		return keyLess(lhs.get()->GetName(), rhs.get()->GetName());
	}

	bool operator()(const T lhs, const boost::shared_ptr<S> rhs) {
		return keyLess(lhs, rhs.get()->GetName());
	}

	bool operator()(const boost::shared_ptr<S> lhs, const T rhs) {
		return keyLess(lhs.get()->GetName(), rhs);
	}

private:
	// the "real" comparison function
	bool keyLess(const T& k1, const T& k2) const {
		return k1 < k2;
	}
};

// S = CDnaStation
template<typename S>
class CompareStnFileOrder_CDnaStn
{
public:
	bool operator()(const boost::shared_ptr<S> lhs, const boost::shared_ptr<S> rhs) {
		if (lhs.get()->GetfileOrder() == rhs.get()->GetfileOrder())
			return (lhs.get()->GetName() < rhs.get()->GetName());
		return (lhs.get()->GetfileOrder() < rhs.get()->GetfileOrder());
	}
};

// S = station_t, U = stn_block_map_t<UINT32, double>
template<typename S, typename U>
class CompareStnFileOrder_StnBlockMap
{
public:
	CompareStnFileOrder_StnBlockMap(std::vector<S>* s)
		:  _s(s) {}
	bool operator()(const U& lhs, const U& rhs) {
		return _s->at(lhs.station_id).fileOrder < _s->at(rhs.station_id).fileOrder;
	}
private:
	std::vector<S>*	_s;
};


// S = station_t, U = stn_block_map_t<UINT32, double>
template<typename U>
class CompareStnOrder_StnBlockMap
{
public:
	bool operator()(const U& lhs, const U& rhs) {
		if (lhs.block_no == rhs.block_no)
			return lhs.station_id < rhs.station_id;
		return lhs.block_no < rhs.block_no;
	}
};


// T = station_t, S = std::string
template<typename T>
class CompareStnName
{
public:
	bool operator()(const T& lhs, const T& rhs) {
		return std::string(lhs.stationName) < std::string(rhs.stationName);
	}
};


// T = station_t, S = std::string
template<typename T, typename S>
class CompareStnOriginalName
{
public:
	bool operator()(const T& lhs, const T& rhs) {
		return std::string(lhs.stationNameOrig) < std::string(rhs.stationNameOrig);
	}
	bool operator()(const T& lhs, const S& rhs) {
		return std::string(lhs.stationNameOrig) < rhs;
	}
	bool operator()(const S& lhs, const T& rhs) {
		return lhs < std::string(rhs.stationNameOrig);
	}
};


// S = station_t, U = UINT32
template<typename S, typename U>
class CompareStnLongitude
{
public:
	CompareStnLongitude(std::vector<S>* s, bool leftToRight=true)
		:  _s(s)
		, _leftToRight(leftToRight) {}
	bool operator()(const U& lhs, const U& rhs) {
		if (_leftToRight)
			return _s->at(lhs).initialLongitude < _s->at(rhs).initialLongitude;
		else
			return _s->at(lhs).initialLongitude > _s->at(rhs).initialLongitude;
	}
private:
	std::vector<S>*	_s;
	bool		_leftToRight;
};





template<typename T = scalar_t>
class CompareScalars
{
public:
	// used for lower_bound, upper_bound, etc...
	bool operator()(const T& lhs, const T& rhs)
	{
		if (lhs.station1 == rhs.station1)
			return lhs.station2 < rhs.station2;
		return lhs.station1 < rhs.station1;
	}
};

template<typename T = scalar_t, typename S = std::string>
class CompareScalarStations
{
public:
	CompareScalarStations(const S& s1, const S& s2)
		: s1_(s1), s2_(s2) {}
	inline void SetComparands(const S& s1, const S& s2) {
		s1_ = s1; 
		s2_ = s2;
	}
	// used for lower_bound, upper_bound, etc...
	bool operator()(T& scalar) {
		return (scalar.station1 == s1_ && 
			scalar.station2 == s2_);
	}
private:
	S s1_;
	S s2_;
};




template<typename T>
class ComparePairSecond
{
public:
	bool operator()(const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const {
		return pair_secondless(lhs.second, rhs.second);
	}
	bool operator()(const std::pair<T, T>& lhs, const T& rhs) {
		return pair_secondless(lhs.second, rhs);
	}
	bool operator()(const T& lhs, const std::pair<T, T>& rhs) {
		return pair_secondless(lhs, rhs.second);
	}
private:
	bool pair_secondless(const T& s1, const T& s2) const {
		return s1 < s2;
	}
};

template<typename T>
class ComparePairFirst
{
public:
	bool operator()(const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const {
		return pair_firstless(lhs.first, rhs.first);
	}
	bool operator()(const std::pair<T, T>& lhs, const T& rhs) {
		return pair_firstless(lhs.first, rhs);
	}
	bool operator()(const T& lhs, const std::pair<T, T>& rhs) {
		return pair_firstless(lhs, rhs.first);
	}
private:
	bool pair_firstless(const T& s1, const T& s2) const {
		return s1 < s2;
	}
};

	
template<typename T, typename U>
class CompareOddPairFirst
{
public:
	bool operator()(const std::pair<T, U>& lhs, const std::pair<T, U>& rhs) const {
		return lhs.first < rhs.first;
	}
};

// S = station_t, T = UINT32, U = std::string
template<typename S, typename T, typename U>
class CompareOddPairFirst_FileOrder
{
public:
	CompareOddPairFirst_FileOrder(std::vector<S>* s)
		:  _s(s) {}
	bool operator()(const std::pair<T, U>& lhs, const std::pair<T, U>& rhs) {
		return _s->at(lhs.first).fileOrder < _s->at(rhs.first).fileOrder;
	}
private:
	std::vector<S>*	_s;
};


template<typename T>
class ComparePairSecondf
{
public:
	ComparePairSecondf(T t) :  _t(t) {}
	bool operator()(const std::pair<T, T>& t) {
		return t.second == _t;
	}
	
	inline void SetComparand(const T& t) { _t = t; }

private:
	T	_t;
};
	
// tweak the binary search so it returns the iterator of the object found
template<typename Iter, typename C>
Iter binary_search_index_noval(Iter begin, Iter end, C compare)
{
	Iter i = lower_bound(begin, end, compare);
	if (i != end)
		return i;
	else
		return end;
}

// tweak the binary search so it returns the iterator of the object found
template<typename Iter, typename T, typename C>
Iter binary_search_index(Iter begin, Iter end, T value, C compare)
{
	Iter i = lower_bound(begin, end, value, compare);
	if (i != end)
		return i;
	else 
		return end;
}

// tweak the binary search so it returns the iterator of the object found
template<typename Iter, typename T>
Iter binary_search_index_pair(Iter begin, Iter end, T value)
{
	Iter i = lower_bound(begin, end, value, ComparePairFirst<T>());
	if (i != end && i->first == value)
		return i;
	else 
		return end;
}




template <typename T, typename U>
class PairCompareFirst {
public: //functions
	// comparison func for sorting
	//bool operator()(const std::pair<T, U>& lhs, const std::pair<T, U>& rhs) const {
	bool operator()(const string_string_pair& lhs, const string_string_pair& rhs) const {
		return keyLess(lhs.first, rhs.first);
	}
	// comparison func for lookups
	//bool operator()(const std::pair<T, U>& lhs, const std::pair<T, U>::first_type& rhs) const {
	bool operator()(const string_string_pair& lhs, const string_string_pair::first_type& rhs) const {
		return keyLess(lhs.first, rhs);
	}
	// comparison func for lookups
	//bool operator()(const std::pair<T, U>::first_type& lhs, const std::pair<T, U>& rhs) const {
	bool operator()(const string_string_pair::first_type& lhs, const string_string_pair& rhs) const {
		return keyLess(lhs, rhs.first);
	}
private:
	// the "real" comparison function
	//bool keyLess(const std::pair<T, U>::first_type& k1, const std::pair<T, U>::first_type& k2) const {
	bool keyLess(const string_string_pair::first_type& k1, const string_string_pair::first_type& k2) const {
		return k1 < k2;
	}
};


#endif /* DNATEMPLATEFUNCS_H_ */
