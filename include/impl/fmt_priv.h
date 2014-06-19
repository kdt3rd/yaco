//
// Copyright (c) 2012 Kimball Thurston & Ian Godin
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

#include <tuple>
#include <locale>
#include <sstream>
#include <stdexcept>

#include "config.h"

////////////////////////////////////////


namespace yaco
{

namespace __priv
{

// @todo: Remove this once os/x updates xcode with a more current
// libcxx that has the constexpr to_int_type, et al.
template <typename charT> constexpr int fmt_to_int( charT c ) { return int( c ); }
template <typename charT> constexpr charT fmt_to_char( int c ) { return charT( c ); }


template <typename charT, typename FmtIter>
size_t
fmt_number( FmtIter &fmt, const FmtIter end, const char *errorTag )
{
	size_t retval = 0;
	typedef typename std::char_traits<charT>::int_type intT;

	constexpr intT zero = fmt_to_int<char>( '0' );
	intT curC = fmt_to_int<charT>( *fmt );
	if ( fmt == end || ! std::isdigit( curC ) )
		throw std::runtime_error( std::string( "Invalid format string: expecting " ) + errorTag );

	while ( fmt != end && std::isdigit( curC ) )
	{
		retval = retval * 10 + static_cast<size_t>( curC - zero );
		++fmt;
		curC = fmt_to_int<charT>( *fmt );
	}

	return retval;
}


template <typename charT>
class fmt_guard
{
public:
	fmt_guard( std::basic_ostream<charT> &out )
			: _out( out ), _flags( out.flags() ) {}
	~fmt_guard( void ) { _out.flags( _flags ); }

private:
	fmt_guard( void ) = delete;
	fmt_guard( const fmt_guard & ) = delete;
	fmt_guard &operator=( const fmt_guard & ) = delete;

	std::basic_ostream<charT> &_out;
	union 
	{
		std::ios::fmtflags _flags;
		char __align_buf[alignof(std::basic_ostream<charT> &)];
	};
};

template <typename charT, size_t I, size_t N>
struct fmt_arg : public fmt_arg<charT, I + 1, N - 1>
{
	typedef fmt_arg<charT, I + 1, N - 1> base;
	template <typename T>
	static void output( std::basic_ostream<charT> &os, size_t x, const T &args )
	{
		if ( x == I )
		{
			os << std::get<I>( args );
			return;
		}
		base::output( os, x, args );
	}
};

template <typename charT, size_t I>
struct fmt_arg<charT, I, 0>
{
	template <typename T>
	static void output( std::basic_ostream<charT> &, size_t, const T & )
	{
		throw std::runtime_error( "Invalid fmt format string: out of range position indicator (missing arguments)" );
	}
};

template <typename charT, typename FmtIter, typename T>
void
fmt_process( std::basic_ostream<charT> &os, FmtIter &fmt, const FmtIter end, const T &args )
{
	size_t argIdx = fmt_number<charT>( fmt, end, "argument offset" );

	typedef typename std::char_traits<charT>::int_type intT;
	constexpr intT sepTag = fmt_to_int<char>( ',' );
	constexpr intT widthTag = fmt_to_int<char>( 'w' );
	constexpr intT baseTag = fmt_to_int<char>( 'b' );
	constexpr intT upBaseTag = fmt_to_int<char>( 'B' );
	constexpr intT fillTag = fmt_to_int<char>( 'f' );
	constexpr intT plusTag = fmt_to_int<char>( '+' );
	constexpr intT precTag = fmt_to_int<char>( 'p' );
	constexpr intT alignTag = fmt_to_int<char>( 'a' );
	constexpr intT alignLeftTag = fmt_to_int<char>( 'l' );
	constexpr intT alignRightTag = fmt_to_int<char>( 'r' );
	constexpr intT commentTag = fmt_to_int<char>( '#' );
	constexpr intT endFmtTag = fmt_to_int<char>( '}' );

	size_t base = 10;
	int precision = -1;
	while ( std::char_traits<charT>::to_int_type( *fmt ) == sepTag )
	{
		++fmt;
		if ( fmt == end )
			throw std::runtime_error( "invalid format specifier: missing end tag" );

		bool doBreak = false;
		switch ( std::char_traits<charT>::to_int_type( *fmt ) )
		{
			case widthTag:
				++fmt;
				os.width( static_cast<typename std::streamsize>( fmt_number<charT>( fmt, end, "output width size" ) ) );
				break;
			case baseTag:
				++fmt;
				os.unsetf( std::ios_base::uppercase );
				base = fmt_number<charT>( fmt, end, "numeric base" );
				break;
			case upBaseTag:
				++fmt;
				os.setf( std::ios_base::uppercase );
				base = fmt_number<charT>( fmt, end, "numeric base" );
				break;
			case fillTag:
				++fmt;
				if ( fmt != end )
				{
					os.fill( *fmt );
					++fmt;
				}
				else
					throw std::runtime_error( "invalid format specifier: end of string encountered before fill character" );
				break;
			case plusTag:
				++fmt;
				os.setf( std::ios_base::showpos );
				break;
			case precTag:
				++fmt;
				precision = static_cast<int>( fmt_number<charT>( fmt, end, "digits of precision" ) );
				break;
			case alignTag:
				++fmt;
				os.unsetf( std::ios_base::adjustfield );
				if ( fmt == end )
					throw std::runtime_error( "invalid format specifier: end of string before alignment tag" );

				switch ( std::char_traits<charT>::to_int_type( *fmt ) )
				{
					case alignLeftTag: os.setf( std::ios_base::left ); break;
					case alignRightTag: os.setf( std::ios_base::right ); break;
					default:
						throw std::runtime_error( "invalid format specifier: invalid alignment character" );
				}
				break;

			case endFmtTag:
			case commentTag:
				doBreak = true;
				break;
			default:
				throw std::runtime_error( "invalid format specifier: unknown format specifier" );
		}

		if ( doBreak )
			break;

		if ( fmt == end )
			throw std::runtime_error( "invalid format specifier: missing end tag" );
	}

	if ( std::char_traits<charT>::to_int_type( *fmt ) == commentTag )
	{
		while ( fmt != end && std::char_traits<charT>::to_int_type( *fmt ) != endFmtTag )
		{
			++fmt;
		}
	}

	if ( std::char_traits<charT>::to_int_type( *fmt ) != endFmtTag )
		throw std::runtime_error( "invalid format specifier: missing end tag" );

	++fmt;

	os.unsetf( std::ios_base::basefield );
	switch ( base )
	{
		case 8: os.setf( std::ios_base::oct ); break;
		case 10: os.setf( std::ios_base::dec ); break;
		case 16: os.setf( std::ios_base::hex ); break;

		default:
			throw std::runtime_error( "invalid format specifier: unsupported numeric base specifier" );
	}

	os.unsetf( std::ios_base::floatfield );
	if ( precision >= 0 )
	{
		os.setf( std::ios_base::fixed );
		os.precision( precision );
	}
	else
		os.setf( std::ios_base::scientific );

	fmt_arg<charT, 0, std::tuple_size<T>::value>::output( os, argIdx, args );
}

template <typename charT, typename F, typename T>
void
fmt_build( std::basic_ostream<charT> &os, const F &fmt, const T &args )
{
	constexpr charT fmtTag = fmt_to_char<charT>( fmt_to_int<char>( '{' ) );
	constexpr charT fmtEsc = fmt_to_char<charT>( fmt_to_int<char>( '\\' ) );

	size_t numArgs = 0;
	auto f = fmt.begin(), fe = fmt.end();
	while ( f != fe )
	{
		switch ( *f )
		{
			case fmtEsc:
				++f;
				if ( f == fe || *f != fmtTag )
				{
					os << fmtEsc;
				}
				else
				{
					os << fmtTag;
					++f;
				}
				break;
			case fmtTag:
			{
				++f;
				fmt_guard<charT> guard( os );

				fmt_process( os, f, fe, args );
				++numArgs;
				break;
			}

			default:
				os << *f++;
				break;
		}
	}
	if ( numArgs != std::tuple_size<T>::value )
		throw std::runtime_error( "invalid format specification: different number of arguments processed than provided" );
}

} // namespace __priv

} // namespace yaco


////////////////////////////////////////
// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:
