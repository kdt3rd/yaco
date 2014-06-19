// argParser.cpp -*- C++ -*-

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

#include <iostream>
#include <sequence_pattern.h>


////////////////////////////////////////


int
main( int argc, char *argv[] )
{
	try
	{
		std::string path;
		if ( argc > 1 )
			path = argv[1];
		else
			path = "foo_001234.tiff";

		yaco::file_sequence_pattern p;

		std::cout << "Examining path: " << path << std::endl;

		int f = p.parse( path );

		std::cout << "Found pattern: " << p.pattern() << std::endl;
		std::cout << "Output framenumber: " << f << std::endl;
		std::cout << "Re-substuted name: " << p.substitute( f ) << std::endl;

		yaco::sequence_pattern<char, std::char_traits<char>, yaco::sequence_dollar_var<char>> otherpattern = p;

		std::cout << "Other ($F<N>) pattern: " << otherpattern.pattern() << std::endl;
		std::cout << "Re-substuted name: " << otherpattern.substitute( f ) << std::endl;

	}
	catch ( std::exception &e )
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

