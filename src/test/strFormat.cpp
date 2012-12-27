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

#include <strutil>
#include <iostream>


////////////////////////////////////////

using namespace yaco::str;

static int
testFormatVoid( void )
{
	std::cout << format( "Hello, World!" ) << std::endl;
	return 0;
}


////////////////////////////////////////


static int
testFormatSimple( void )
{
//	std::string xxx = format( "Hello {0,f ,w10}", 3.1415 );
	std::cout << format( "Hello {0,f ,w20}", 3.1415 ) << std::endl;
	return 0;
}


////////////////////////////////////////


static int
testReorderedSimple( void )
{
//	std::string xxx = format( "Hello {0,f ,w10}", 3.1415 );
	std::cout << format( "Hello, {1,#comment}: pi is approximately {0,+,p3}", 22.0/7.0, "world!" ) << std::endl;
	return 0;
}


////////////////////////////////////////


int
main( int argc, char *argv[] )
{
	int retval = 0;
	try
	{
		retval += testFormatVoid();
		retval += testFormatSimple();
		retval += testReorderedSimple();
	}
	catch ( std::exception &e )
	{
		std::cout << "ERROR: " << e.what() << std::endl;
		return -1;
	}

	return retval;
}
