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

#include <scope_guard>
#include <iostream>

namespace
{

int
testNormalGuard( void )
{
	int z = 0;
	{
		yaco::scope_guard x = [&]() { z = 0; };
		z = 42;
		x.release();
	}
	if ( z != 42 )
	{
		std::cout << "ERROR: " << __PRETTY_FUNCTION__ << " failed" << std::endl;
		return 1;
	}
	z = 0;
	try
	{
		yaco::scope_guard x = [&]() { z = 42; };
		throw 42;
	}
	catch ( ... )
	{
	}
	if ( z != 42 )
	{
		std::cout << "ERROR: " << __PRETTY_FUNCTION__ << " rollback failed" << std::endl;
		return 1;
	}
	return 0;
}

} // empty namespace


////////////////////////////////////////


int
main( int /*argc*/, char */*argv*/[] )
{
	int retval = 0;
	try
	{
		retval += testNormalGuard();
	}
	catch ( std::exception &e )
	{
		std::cout << "ERROR: " << e.what() << std::endl;
		return -1;
	}

	return retval;
}
