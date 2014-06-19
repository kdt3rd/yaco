// sequence_pattern.h -*- C++ -*-

//
// Copyright (c) 2014 Kimball Thurston
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

#include <string>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <cctype>

#include "impl/fmt_priv"


////////////////////////////////////////


namespace yaco
{

/// @brief Implements filenames of the pattern foo_#######.tiff
template < typename charT, typename traitsT = std::char_traits<charT> >
struct sequence_marker_pound
{
	typedef std::basic_string<charT, traitsT> value_type;
	typedef typename value_type::size_type size_type;

	static std::pair<size_type, size_type> find( size_type &count, const value_type &a )
	{
		count = 0;
		size_type start = a.find_first_of( charT('#') );
		size_type end = a.find_last_of( charT('#') );

		if ( start != value_type::npos )
			count = end - start + 1;

		return std::make_pair( start, end );
	}

	static value_type encoding( size_t count )
	{
		return value_type( count, charT('#') );
	}
};

/// @brief Implements filenames of the pattern foo_$F7.tiff
template < typename charT, typename traitsT = std::char_traits<charT> >
struct sequence_dollar_var
{
	typedef std::basic_string<charT, traitsT> value_type;
	typedef typename value_type::size_type size_type;

	static std::pair<size_type, size_type> find( size_type &count, const value_type &a )
	{
		count = 0;
		size_type start = a.find( "$F" );
		size_type end = value_type::npos;

		int zero = __priv::fmt_to_int<charT>( '0' );

		if ( start != value_type::npos )
		{
			size_type tmpPos = start + 2;
			while ( std::isdigit( a[tmpPos] ) )
			{
				int dig = __priv::fmt_to_int<charT>( a[tmpPos] ) - zero;
				count = count * 10 + dig;
				++tmpPos;
			}
			if ( count == 0 )
				count = 1;
			end = tmpPos - start;
		}

		return std::make_pair( start, end );
	}

	static value_type encoding( size_t count )
	{
		std::basic_stringstream<charT, traitsT> os;
		os << "$F" << count;
		return os.str();
	}
};

///
/// @brief Class sequence_pattern provides an abstraction around
///        storing a (counted) sequence of files.
///
template < typename charT, typename traitsT = std::char_traits<charT>, typename markerT = sequence_marker_pound<charT, traitsT> >
class sequence_pattern
{
public:
	typedef std::basic_string<charT, traitsT> value_type;
	typedef typename value_type::size_type size_type;
	typedef markerT marker_type;

	inline sequence_pattern( void ) {}
	inline sequence_pattern( const value_type &pattern )
	{
		auto ppos = markerT::find( _count, pattern );
		if ( ppos.first != value_type::npos )
		{
			_prefix = pattern.substr( 0, ppos.first );
			_suffix = pattern.substr( ppos.first + ppos.second );
		}
	}

	inline sequence_pattern( const value_type &prefix, const value_type &suffix, size_type count )
			: _prefix( prefix ), _suffix( suffix ), _count( count )
	{
	}

	inline sequence_pattern( sequence_pattern &&o )
			: _prefix( std::move( o._prefix ) ), _suffix( std::move( o._suffix ) ),
			  _count( std::move( o._count ) )
	{}

	template <typename otherMarkerT>
	inline sequence_pattern( const sequence_pattern<charT, traitsT, otherMarkerT> &o )
			: _prefix( o.prefix() ), _suffix( o.suffix() ), _count( o.count() )
	{}

	template <typename otherMarkerT>
	inline sequence_pattern( sequence_pattern<charT, traitsT, otherMarkerT> &&o )
			: _prefix( std::move( o._prefix ) ), _suffix( std::move( o._suffix ) ), _count( std::move( o._count ) )
	{}

	inline ~sequence_pattern( void ) {}

	inline sequence_pattern &operator=( const sequence_pattern &o )
	{
		if ( this != &o )
		{
			_prefix = o._prefix;
			_suffix = o._suffix;
			_count = o._count;
		}

		return *this;
	}

	inline sequence_pattern &operator=( sequence_pattern &&o )
	{
		_prefix = std::move( o._prefix );
		_suffix = std::move( o._suffix );
		_count = std::move( o._count );
		return *this;
	}

	template <typename otherMarkerT>
	inline sequence_pattern &operator=( const sequence_pattern<charT, traitsT, otherMarkerT> &o )
	{
		if ( this != &o )
		{
			_prefix = o.prefix();
			_suffix = o.suffix();
			_count = o.count();
		}

		return *this;
	}

	template <typename otherMarkerT>
	inline sequence_pattern &operator=( sequence_pattern<charT, traitsT, otherMarkerT> &&o )
	{
		_prefix = std::move( o._prefix );
		_suffix = std::move( o._suffix );
		_count = std::move( o._count );
		return *this;
	}

	inline const value_type &prefix( void ) const { return _prefix; }
	inline const value_type &suffix( void ) const { return _suffix; }
	inline size_type count( void ) const { return _count; }

	inline value_type pattern( void ) const
	{
		if ( _count == 0 )
			return _prefix + _suffix;

		return _prefix + markerT::encoding( _count ) + _suffix;
	}

	inline value_type substitute( int64_t frame )
	{
		if ( _count == 0 )
			return _prefix + _suffix;

		std::basic_stringstream<charT, traitsT> os;
		os << std::setfill( charT('0') ) << std::setw( _count ) << frame;
		return _prefix + os.str() + _suffix;
	}

	// if you have potential extensions with numbers (i.e. .y4m) you
	// should pass in the position of the beginning of the extension
	// to extpos
	inline int64_t parse( const value_type &filename, size_type extpos = value_type::npos )
	{
		clear();

		if ( filename.empty() )
			return -1;

		auto ri = filename.rbegin();
		auto re = filename.rend();

		size_type frameStart = value_type::npos;
		size_type frameEnd = filename.size();
		
		// if the user provided 
		if ( extpos < filename.size() )
		{
			frameEnd = filename.size() - extpos;
			for ( size_type i = 0; i < frameEnd; ++i )
				++ri;
		}

		while ( ri != re )
		{
			if ( std::isdigit( *ri ) )
				break;

			--frameEnd;
			++ri;
		}

		if ( ri == re )
		{
			_prefix = filename;
			return -1;
		}

		_suffix = filename.substr( frameEnd );

		int zero = __priv::fmt_to_int<charT>( '0' );
		int dig = __priv::fmt_to_int<charT>( (*ri) );
		int64_t frm = dig - zero;
		int64_t scale = 10;
		frameStart = frameEnd - 1;
		++ri;
		while ( ri != re && std::isdigit( *ri ) )
		{
			dig = __priv::fmt_to_int<charT>( (*ri) ) - zero;
			frm += static_cast<int64_t>( dig ) * scale;
			scale *= 10;
			--frameStart;
			++ri;
		}
		_prefix = filename.substr( 0, frameStart );
		_count = frameEnd - frameStart;

		return frm;
	}

	inline void clear( void )
	{
		_prefix.clear();
		_suffix.clear();
		_count = 0;
	}

private:
	value_type _prefix;
	value_type _suffix;
	size_type _count = 0;
};

typedef sequence_pattern<char> file_sequence_pattern;

} // namespace yaco

