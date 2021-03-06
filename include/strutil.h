//
// Copyright (c) 2012-2014 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <locale>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <cstring>
#include <type_traits>
#include <ostream>

#include "impl/fmt_priv.h"
#include "const_string.h"

namespace yaco
{

// a collection of string utilities...
namespace str
{

// @todo: how to handle different character types and codecvt conversion?
//
// In theory, we have the locale in the output stream (os.getloc()) and
// can get the appropriate codecvt type, but then need to do the
// same on any string arguments and such...
// also need to take into account some form of gettext to
// retrieve the appropriate translation...
//

template <typename charT, std::size_t N, typename... Args>
inline std::basic_string<charT>
format( const charT (&fmt)[N], const Args&... args )
{
	std::basic_stringstream<charT> tmp;
	__priv::fmt_build( tmp, const_string<charT>( fmt, N ), std::tie(args...) );
	return tmp.str();
}

template <typename charT, typename... Args>
inline std::basic_string<charT>
format( const charT *&fmt, const Args&... args )
{
	std::basic_stringstream<charT> tmp;
	__priv::fmt_build( tmp, const_string<charT>( fmt ), std::tie(args...) );
	return tmp.str();
}

template <typename charT, typename traitsT, typename allocT, typename... Args>
inline std::basic_string<charT, traitsT, allocT>
format( const std::basic_string<charT, traitsT, allocT> &fmt, const Args&... args )
{
	std::basic_stringstream<charT, traitsT, allocT> tmp;
	__priv::fmt_build( tmp, fmt, std::tie(args...) );
	return tmp.str();
}

template <typename charT, std::size_t N, typename... Args >
void
output( std::basic_ostream<charT> &os, const charT (&fmt)[N], const Args&... args )
{
	__priv::fmt_build( os, const_string<charT>( fmt, N ), std::tie(args...) );
}

template <typename charT, typename traitsT, typename... Args >
void
output( std::basic_ostream<charT, traitsT> &os, const charT *&fmt, const Args&... args )
{
	__priv::fmt_build( os, const_string<charT>( fmt ), std::tie(args...) );
}

template <typename charT, class traitsT, class allocTf, typename... Args >
void
output( std::basic_ostream<charT, traitsT> &os, const std::basic_string<charT, traitsT, allocTf> &fmt, const Args&... args )
{
	__priv::fmt_build( os, fmt, std::tie(args...) );
}


////////////////////////////////////////////////////////////////////////////////
// Utility functions to split on either the individual separator or a set of
// separator characters
////////////////////////////////////////////////////////////////////////////////

template <typename stringT>
inline void
split( std::vector<stringT> &__ret, const stringT &__str, const stringT &__sep, bool __skip_empty = false )
{
	__ret.clear();

	if ( __skip_empty )
	{
		typename stringT::size_type __last = __str.find_first_not_of( __sep, 0 );
		typename stringT::size_type __cur = __str.find_first_of( __sep, __last );
		
		while ( __cur != stringT::npos || __last < __str.size() )
		{
			if ( __cur != __last )
				__ret.push_back( __str.substr( __last, __cur - __last ) );
			__last = __str.find_first_not_of( __sep, __cur );
			__cur = __str.find_first_of( __sep, __last );
		}
	}
	else
	{
		typename stringT::size_type __last = 0;
		typename stringT::size_type __cur = __str.find_first_of( __sep, __last );

		do
		{
			if ( __cur == stringT::npos )
			{
				if ( __last == __str.size() )
					__ret.push_back( stringT() );
				else
					__ret.push_back( __str.substr( __last ) );
				break;
			}
			else
				__ret.push_back( __str.substr( __last, __cur - __last ) );

			__last = __cur + 1;
			__cur = __str.find_first_of( __sep, __last );
		} while ( __last != stringT::npos );
	}
}

template <typename stringT>
inline void
split( std::vector<stringT> &__ret, const stringT &__str,
	   const typename stringT::value_type *__sep,
	   bool __skip_empty = false )
{
	split( __ret, __str, stringT( __sep ), __skip_empty );
}

template <typename stringT>
inline std::vector<stringT>
split( const stringT &__str, const stringT &__sep, bool __skip_empty = false )
{
	std::vector<stringT> __ret;
	split( __ret, __str, __sep, __skip_empty );
	return __ret;
}

template <typename stringT>
std::vector<stringT>
split( const stringT &__str, const typename stringT::value_type *__sep,
	   bool __skip_empty = false )
{
	return split( __str, stringT( __sep ), __skip_empty );
}

template <typename stringT>
inline void
split( std::vector<stringT> &__ret, const stringT &__str,
	   typename stringT::value_type __sep,
	   bool __skip_empty = false )
{
	__ret.clear();

	if ( __skip_empty )
	{
		typename stringT::size_type __last = __str.find_first_not_of( __sep, 0 );
		typename stringT::size_type __cur = __str.find_first_of( __sep, __last );

		while ( __cur != stringT::npos || __last < __str.size() )
		{
			if ( __cur != __last )
				__ret.push_back( __str.substr( __last, __cur - __last ) );
			__last = __str.find_first_not_of( __sep, __cur );
			__cur = __str.find_first_of( __sep, __last );
		}
	}
	else
	{
		typename stringT::size_type __last = 0;
		typename stringT::size_type __cur = __str.find_first_of( __sep, __last );

		do
		{
			if ( __cur == stringT::npos )
			{
				if ( __last == __str.size() )
					__ret.push_back( stringT() );
				else
					__ret.push_back( __str.substr( __last ) );
				break;
			}
			else
				__ret.push_back( __str.substr( __last, __cur - __last ) );

			__last = __cur + 1;
			__cur = __str.find_first_of( __sep, __last );
		} while ( __last != stringT::npos );
	}
}

template <typename stringT>
std::vector<stringT>
split( const stringT &__str, typename stringT::value_type __sep,
	   bool __skip_empty = false )
{
	std::vector<stringT> __ret;
	split( __ret, __str, __sep, __skip_empty );
	return __ret;
}

template <typename stringT>
inline int icompare( const stringT &__a, const stringT &__b )
{
//	typedef typename std::basic_string<charT, traitsT, allocT> __s_type;
//	auto &__f = std::use_facet<std::collate<charT>>( std::locale() );
//  the above facet can be used w/ the transform function to enable
//	true lexicographical compare, otherwise it'll just be comparing
//	the lower case versions
	typedef typename stringT::size_type sz_t;
	typedef typename stringT::value_type charT;
	typedef typename stringT::traits_type traitsT;

	sz_t __lhs = __a.size();
    sz_t __rhs = __b.size();
	std::locale __l;

	const charT *__aP = __a.data();
	const charT *__bP = __b.data();
	for ( sz_t __n = std::min( __lhs, __rhs ); __n; --__n, ++__aP, ++__bP )
	{
		charT __aI = std::tolower( *__aP, __l );
		charT __bI = std::tolower( *__bP, __l );
		if ( traitsT::lt( __aI, __bI ) )
			return -1;
		if ( traitsT::lt( __bI, __aI ) )
			return 1;
	}

	if ( __lhs < __rhs )
		return -1;

	if ( __lhs > __rhs )
		return 1;

	return 0;
}

template <typename stringT>
inline bool
iless( const stringT &a, const stringT &b )
{
	return icompare( a, b ) < 0;
}

template <typename stringT>
inline stringT to_lower( const stringT &__str, const std::locale &__l )
{
	return transform( __str.begin(), __str.end(), __str.begin(),
					  bind( tolower, std::placeholders::_1, __l ) );
}

template <typename stringT>
inline stringT to_lower( const stringT &__str )
{
	return to_lower( __str, std::locale() );
}

template <typename stringT>
inline stringT to_upper( const stringT &__str, const std::locale &__l)
{
	return transform( __str.begin(), __str.end(), __str.begin(),
					  bind( toupper, std::placeholders::_1, __l ) );
}

template <typename stringT>
inline stringT to_upper( const stringT &__str )
{
	return to_upper( __str, std::locale() );
}

} // namespace str

} // namespace yaco

// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:

