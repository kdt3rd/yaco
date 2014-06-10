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

#include <arg_parser>
#include <iostream>


////////////////////////////////////////


int verbosity = 0;
static void
bumpVerbose( int x )
{
	verbosity += x;
}


////////////////////////////////////////


int
main( int argc, char *argv[] )
{
	yaco::arg_parser args;

	try
	{
		args.program_name( "Arg Parser Unit Test" );
		args.program_version( "v1.0" );
		args.build_date( __DATE__ );

		bool targ = false;
		args.add_flag( 't', "test", "Test Argument", true, targ );
		args.add_flag_counter( 'v', "verbose", "Verbosity", bumpVerbose );

		std::string n;
		args.add_argument( 0, "name", "Name Argument", n, std::string() );
		double v = 0.0;
		args.add_argument( 'd', "number", "Number Argument", v, 0.0 );
		int cnt = 3;
		args.add_argument( 'c', "count", "Required Count", cnt, cnt, true );

		std::cout << "test usage:\n-------\n";
		args.print_usage();
		std::cout << "\n-------\n";

		if ( args.parse( argc, argv ) )
			std::cout << "arg parse succeeded" << std::endl;
		else
			std::cout << "arg parse failed" << std::endl;

		std::cout << "test: " << (targ ? "true" : "false") << std::endl;
		std::cout << "verbosity: " << verbosity << std::endl;
	}
	catch ( std::exception &e )
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

