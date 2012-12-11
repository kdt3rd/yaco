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

#include <variadic>

static void
test_func_void( void )
{
}

static void
test_func_1( const char * )
{
}

static void
test_func_2( const char *, const char * )
{
}

int
main( int argc, const char *argv[] )
{
	auto xxx = std::make_tuple();
	auto xxx1 = std::make_tuple( "foo" );
	auto xxx2 = std::make_tuple( "foo", "bar" );

	yaco::call_func_from_tuple( &test_func_void, xxx );
	yaco::call_func_from_tuple( &test_func_1, xxx1 );
	yaco::call_func_from_tuple( &test_func_2, xxx2 );

	return 0;
}
