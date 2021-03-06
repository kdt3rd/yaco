// const_string.h -*- C++ -*-

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
#include <ostream>


////////////////////////////////////////


namespace yaco
{

template < typename charT, typename traitsT = std::char_traits<charT> > class const_string;

typedef const_string<char> cstring;
typedef const_string<wchar_t> wcstring;
typedef const_string<char16_t> u16cstring;
typedef const_string<char32_t> u32cstring;

///
/// @brief Class const_string provides a simple memory reference string
///        class, primarily designed to wrap static (const) strings as a
///        class to use other algorithms without making a copy into a
///        basic_string.
///
/// NB: the string passed in MUST be in an outer scope of this object.
///     conststr is very lightweight, does not copy, and even things
///     like substr just update pointers, but don't modify the underlying
///     values, so printing a conststr is a bit harder than a normal
///     string.
template <class charT, typename traitsT>
class const_string
{
public:
	typedef traitsT traits_type;
	typedef typename traitsT::char_type value_type;
	typedef std::size_t size_type;
	typedef const value_type * const_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	static const size_type npos = static_cast<size_type>(-1);

	template <std::size_t N>
	constexpr const_string( const value_type (&s)[N] ) : _str( s ), _sz( N - 1 ) {}
	constexpr const_string( const value_type *s, size_type N ) : _str( s ), _sz( N - 1 ) {}
	template <typename allocTs>
	constexpr const_string( const std::basic_string<charT, traitsT, allocTs> &s ) : _str( s.data() ), _sz( s.size() ) {}
	constexpr const_string( const value_type *s ) : _str( s ), _sz( std::strlen( s ) ) {}

	constexpr const value_type operator[]( size_type i ) const { return _str[i]; }
	constexpr const value_type at( size_type i ) const { return i <= _sz ? _str[i] : throw std::out_of_range( "const_string access out of range" ); }
	constexpr size_type size( void ) const { return _sz; }

	constexpr const_iterator begin( void ) const { return _str; }
	constexpr const_iterator end( void ) const { return _str + _sz + 1; }

	constexpr const_reverse_iterator rbegin( void ) const { return const_reverse_iterator(end()); }
	constexpr const_reverse_iterator rend( void ) const { return const_reverse_iterator(begin()); }

	int
	compare( const const_string &s ) const
	{
		const size_type osz = s.size();
		const size_type n = std::min( _sz, osz );

		int r = traits_type::compare( begin(), s.begin(), n );
		if ( r == 0 )
		{
			if ( _sz == osz )
				return 0;
			return _sz < osz ? -1 : 1;
		}
		return r;
	}

	int compare( size_type pos, size_type n, const const_string &s ) const
	{
		return substr( pos, n ).compare( s );
	}

	int compare( size_type pos1, size_type n1, const const_string &s, size_type pos2, size_type n2 ) const
	{
		return substr( pos1, n1 ).compare( s.substr( pos2, n2 ) );
	}

	size_type find_first_of( value_type c, size_type pos = 0 ) const
	{
		size_type retval = npos;
		if ( pos < _sz )
		{
			const size_type n = _sz - pos;
			const value_type *p = traits_type::find( _str + pos, n, c );
			if ( p )
				retval = p - _str;
		}
		return retval;
	}

	size_type find_first_not_of( value_type c, size_type pos = 0 ) const
	{
		for ( ; pos < _sz; ++pos )
		{
			if ( ! traits_type::eq( _str[pos], c ) )
				return pos;
		}
		return npos;
	}

	size_type find_last_of( value_type c, size_type pos = npos ) const
	{
		size_type s = _sz;
		if ( s )
		{
			if ( --s > pos )
				s = pos;
			for ( ++s; s-- > 0; )
			{
				if ( traits_type::eq( _str[s], c ) )
					return s;
			}
		}
		return npos;
	}

	const_string substr( size_type pos = 0, size_type n = npos ) const
	{
		if ( pos > _sz )
			throw std::out_of_range( "Invalid start position for constant string" );
		const bool nValid = n < _sz - pos;
		return const_string( _str + pos, nValid ? n : _sz - pos );
	}

	std::basic_string<value_type, traits_type> str( void ) const
	{
		return std::basic_string<value_type, traits_type>( _str, _sz );
	}

private:
	const_string( void ) = delete;

	const value_type *_str;
	size_type _sz;
};

template <typename streamT, typename charT, typename traitsT>
inline streamT &operator<<( streamT &os, const const_string<charT, traitsT> &s )
{
	os.write( s.begin(), s.size() );
	return os;
}

template <typename charT, typename traitsT>
inline bool
operator==( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) == 0;
}

template <typename charT, typename traitsT>
inline bool
operator!=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return !( lhs == rhs );
}

template <typename charT, typename traitsT>
inline bool
operator<( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) < 0;
}

template <typename charT, typename traitsT>
inline bool
operator>( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) > 0;
}

template <typename charT, typename traitsT>
inline bool
operator<=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) <= 0;
}

template <typename charT, typename traitsT>
inline bool
operator>=( const const_string<charT, traitsT> &lhs, const const_string<charT, traitsT> &rhs )
{
	return lhs.compare( rhs ) >= 0;
}

} // namespace yaco




