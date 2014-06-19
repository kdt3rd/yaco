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
#include <algorithm>
#include <cctype>

#include "strutil.h"

namespace yaco
{

namespace __priv
{

template <typename String>
inline typename String::size_type
find_next_slash( const String &path, typename String::size_type pos = 0 )
{
	typename String::size_type nextSlash = path.find_first_of( String::value_type('/'), pos );
#ifdef _WIN32
	// TODO: Is it more efficient to use find_first_of( "/\\" ) instead
	// of dispatching twice?
	typename String::size_type nextBackSlash = path.find_first_of( String::value_type('\\'), pos );
	if ( nextSlash == String::npos )
		nextSlash = nextBackSlash;
	else
		nextSlash = std::min( nextSlash, nextBackSlash );
#endif
	return nextSlash;
}

template <typename String>
inline typename String::size_type
find_last_slash( const String &path, typename String::size_type pos = String::npos )
{
	typename String::size_type lastSlash = path.find_last_of( String::value_type('/'), pos );
#ifdef _WIN32
	// TODO: Is it more efficient to use find_last_of( "/\\" ) instead
	// of dispatching twice?
	typename String::size_type lastBackSlash = path.find_last_of( String::value_type('\\'), pos );
	if ( lastSlash == String::npos )
		lastSlash = lastBackSlash;
	else
		lastSlash = std::max( lastSlash, lastBackSlash );
#endif
	return lastSlash;
}

template <typename String>
inline std::pair< typename String::size_type, typename String::size_type >
find_path_split_point( const String &path, typename String::size_type pos = String::npos )
{
	typename String::size_type lastSlash = __priv::find_last_slash( path );
	typename String::size_type pathSz = path.size();
	typename String::size_type chopEnd = pathSz;
	while ( lastSlash != String::npos && ( lastSlash + 1 ) == chopEnd )
	{
		chopEnd = lastSlash;
		lastSlash = __priv::find_last_slash( path, lastSlash );
	}

	return std::make_pair( lastSlash, chopEnd == pathSz ? String::npos : chopEnd );
}

} // namespace __priv

template <typename String>
inline std::pair<String, String>
split_path( const String &path )
{
	auto sp = __priv::find_path_split_point( path );

	if ( sp.first == String::npos || sp.first == 0 )
		return std::make_pair( String(), path );
	return std::make_pair( path.substr( 0, sp.first ), path.substr( sp.first + 1, sp.second ) );
}

template <typename String>
inline String
basename( const String &path )
{
	auto sp = __priv::find_path_split_point( path );

	if ( sp.first == String::npos || sp.first == 0 )
		return path;
	return path.substr( sp.first + 1, sp.second );
}

template <typename String>
inline String
dirname( const String &path )
{
	auto sp = __priv::find_path_split_point( path );

	if ( sp.first == String::npos )
		return String();
	return path.substr( sp.first + 1, sp.second );
}

template <typename String>
inline String
extension( const String &filename )
{
	typename String::size_type lastDot = filename.find_last_of( String::value_type( '.' ) );
	if ( lastDot == String::npos )
		return String();
	return filename.substr( lastDot + 1 );
}
	
} // namespace yaco

////////////////////////////////////////
// Local Variables:
// mode: C++
// End:
// vm:ft=cpp:
//
