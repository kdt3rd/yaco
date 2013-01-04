/// -*- mode: C++ -*-
///
/// @file varFuncDisp.cpp
///
/// @author Kimball Thurston

//
// Copyright (c) 2012 Kimball Thurston
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

#include <iostream>

#include <strutil>

static int
test_simple( void )
{
	std::string tmp1( "FOO:BAR:BAZ" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':' );
	if ( s1.size() == 3 && ( s1[0] == "FOO" && s1[1] == "BAR" && s1[2] == "BAZ" ) )
		return 0;

	std::cerr << "strSplit: test_simple failed" << std::endl;
	return 1;
}

static int
test_simple_no_strs( void )
{
	std::string tmp1( "::" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':' );
	if ( s1.size() == 3 && ( s1[0].empty() && s1[1].empty() && s1[2].empty() ) )
		return 0;

	std::cerr << "strSplit: test_simple_no_strs failed" << std::endl;
	return 1;
}

static int
test_simple_end( void )
{
	std::string tmp1( "FOO:BAR:BAZ:" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':' );
	if ( s1.size() == 4 && ( s1[0] == "FOO" && s1[1] == "BAR" && s1[2] == "BAZ" && s1[3].empty() ) )
		return 0;

	std::cerr << "strSplit: test_simple_end failed" << std::endl;
	return 1;
}

static int
test_simple_begin( void )
{
	std::string tmp1( ":FOO:BAR:BAZ" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':' );
	if ( s1.size() == 4 && ( s1[0].empty() && s1[1] == "FOO" && s1[2] == "BAR" && s1[3] == "BAZ" ) )
		return 0;

	std::cerr << "strSplit: test_simple_begin failed" << std::endl;
	return 1;
}

static int
test_simple_middle( void )
{
	std::string tmp1( "FOO::BAR:BAZ" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':' );
	if ( s1.size() == 4 && ( s1[0] == "FOO" && s1[1].empty() && s1[2] == "BAR" && s1[3] == "BAZ" ) )
		return 0;

	std::cerr << "strSplit: test_simple_middle failed" << std::endl;
	return 1;
}


////////////////////////////////////////


static int
test_simple_no_empty( void )
{
	std::string tmp1( "FOO:BAR:BAZ" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':', true );
	if ( s1.size() == 3 && ( s1[0] == "FOO" && s1[1] == "BAR" && s1[2] == "BAZ" ) )
		return 0;

	std::cerr << "strSplit: test_simple_no_empty failed" << std::endl;
	return 1;
}

static int
test_simple_no_strs_no_empty( void )
{
	std::string tmp1( "::" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':', true );
	if ( s1.empty() )
		return 0;

	std::cerr << "strSplit: test_simple_no_strs_no_empty failed" << std::endl;
	return 1;
}

static int
test_simple_end_no_empty( void )
{
	std::string tmp1( "FOO:BAR:BAZ:" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':', true );
	if ( s1.size() == 3 && ( s1[0] == "FOO" && s1[1] == "BAR" && s1[2] == "BAZ" ) )
		return 0;

	std::cerr << "strSplit: test_simple_end_no_empty failed" << std::endl;
	return 1;
}

static int
test_simple_begin_no_empty( void )
{
	std::string tmp1( ":FOO:BAR:BAZ" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':', true );
	if ( s1.size() == 3 && ( s1[0] == "FOO" && s1[1] == "BAR" && s1[2] == "BAZ" ) )
		return 0;

	std::cerr << "strSplit: test_simple_begin_no_empty failed" << std::endl;
	return 1;
}

static int
test_simple_middle_no_empty( void )
{
	std::string tmp1( "FOO::BAR:BAZ" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ':', true );
	if ( s1.size() == 3 && ( s1[0] == "FOO" && s1[1] == "BAR" && s1[2] == "BAZ" ) )
		return 0;

	std::cerr << "strSplit: test_simple_middle_no_empty failed" << std::endl;
	return 1;
}

static int
test_strsep( void )
{
	std::string tmp1( "FOO:=BAR:BAZ:=WHEE=FOO" );
	std::vector<std::string> s1 = yaco::str::split( tmp1, ":=" );
	if ( s1.size() == 7 )
		return 0;

	std::cerr << "strSplit: test_strsep failed: " << s1.size() << std::endl;
	for ( auto s = s1.begin(); s != s1.end(); ++s )
		std::cerr << "'" << (*s) << "', ";
	std::cerr << std::endl;
	return 1;
}

int
main( int /*argc*/, char */*argv*/[] )
{
	int retval = 0;

	retval += test_simple();
	retval += test_simple_no_strs();
	retval += test_simple_end();
	retval += test_simple_begin();
	retval += test_simple_middle();

	retval += test_simple_no_empty();
	retval += test_simple_no_strs_no_empty();
	retval += test_simple_end_no_empty();
	retval += test_simple_begin_no_empty();
	retval += test_simple_middle_no_empty();

	retval += test_strsep();
	
	return retval;
}
