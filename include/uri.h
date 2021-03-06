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

#include "impl/config.h"
#include "strutil.h"

namespace yaco
{

template <typename charT>
class uri
{
public:
	typedef typename std::enable_if<std::is_integral<charT>::value, charT>::type type;
	typedef std::basic_string<type> storage;

	uri( const type *u );
	template <typename traitsT, typename allocT>
	inline uri( const std::basic_string<type, traitsT, allocT> &u )
	{
		parse( u.c_str() );
	}

	uri( const uri &o );
	uri( uri &&o );

	uri &operator=( const uri &o );
	uri &operator=( uri &&o );

	inline void parse( const type *u )
	{
		clear();
		if ( ! u )
			return;

		std::locale l;
		enum parse_state 
		{
			in_scheme,
			in_hier,
			in_query,
			in_fragment
		};
		parse_state curstate = in_scheme;
		while ( *u )
		{
			switch ( curstate )
			{
				case in_scheme:
					if ( *u == type( ':' ) )
						curstate = in_hier;
					else
						_scheme.push_back( std::tolower( *u, l ) );
					break;
				case in_hier:
					if ( *u == type( '?' ) )
					{
						curstate = in_query;
					}
					else
						_hier.push_back( *u );
					break;
				case in_query:
					if ( *u == type( '#' ) )
						curstate = in_fragment;
					else
						_query.push_back( *u );
					break;
				case in_fragment:
					_fragment.push_back( *u );
					break;
			}
			++u;
		}

		split_hierarchy();
	}

	template <typename traitsT, typename allocT>
	inline void parse( const std::basic_string<type, traitsT, allocT> &u )
	{
		return parse( u.c_str() );
	}

	std::string escaped( void );

	inline const storage &scheme( void ) const { return _storage; }
	inline const storage &hierarchy( void ) const { return _storage; }
	inline const storage &query( void ) const { return _storage; }
	inline const storage &fragment( void ) const { return _storage; }

	/// To be used when constructing particular components of a URI such
	/// that reserved characters aren't mis-interpreted
	template <typename xxx, typename traitsT, typename allocT>
	static std::basic_string<xxx, traitsT, allocT>
	escape( const std::basic_string<xxx, traitsT, allocT> &s )
	{
	}

	template <typename xxx, typename traitsT, typename allocT>
	static std::basic_string<xxx, traitsT, allocT>
	unescape( const std::basic_string<xxx, traitsT, allocT> &s )
	{
	}
};

} // namespace yaco

// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:

