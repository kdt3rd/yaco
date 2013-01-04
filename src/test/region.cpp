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

#include <math/region>
#include <stdexcept>
#include <iostream>


////////////////////////////////////////


using namespace yaco::math;

static int
testEmpty( void )
{
	region a;
	if ( a.empty() )
		return 0;
	throw std::runtime_error( __PRETTY_FUNCTION__ );
}


////////////////////////////////////////


static int
testSize( void )
{
	region a( 0, 99, 0, 99 );
	if ( a.size_x() == 100 && a.size_y() == 100 )
		return 0;

	std::cout << "size: " << a.size_x() << " x " << a.size_y() << std::endl;
	std::cout << " expected: 100 x 100" << std::endl;

	throw std::runtime_error( __PRETTY_FUNCTION__ );
}


////////////////////////////////////////


static int
testSizeReset( void )
{
	region a( 0, 99, 0, 99 );
	a.reset_by_size( 25, 25, 50, 50 );
	if ( a.right() == 74 && a.top() == 74 )
		return 0;

	throw std::runtime_error( __PRETTY_FUNCTION__ );
}


////////////////////////////////////////


static int
testEquality( void )
{
	region a( 0, 99, 0, 99 );
	region b( 25, 74, 25, 74 );
	region c = a;
	if ( a == c && c != b )
		return 0;
	throw std::runtime_error( __PRETTY_FUNCTION__ );
}


////////////////////////////////////////


static int
testIntersect( void )
{
	region a( 0, 99, 0, 99 );
	region b( 25, 74, 25, 74 );
	region c = a;
	c.intersect( b );
	if ( c == b )
		return 0;
	throw std::runtime_error( __PRETTY_FUNCTION__ );
}


////////////////////////////////////////


static int
testIntersectRegion( void )
{
	region a( 0, 99, 0, 99 );
	region b( 25, 74, 25, 74 );
	region_list andL = a & b;
	if ( andL.size() == 1 && andL[0] == b )
	{
		andL = andL & a;
		if ( andL.size() == 1 && andL[0] == b )
		{
			region_list la{ a };
			region_list lb{ region( -25, 24, -25, 24 ), region( 75, 124, 75, 124 ) };
			andL = la & lb;
			if ( andL.size() == 2 &&
				 andL[0].offset_x() == 0 && andL[0].size_x() == 25 &&
				 andL[0].offset_y() == 0 && andL[0].size_y() == 25 &&
				 andL[1].offset_x() == 75 && andL[1].size_x() == 25 &&
				 andL[1].offset_x() == 75 && andL[1].size_x() == 25 )
				return 0;
		}
	}

	throw std::runtime_error( __PRETTY_FUNCTION__ );
}


////////////////////////////////////////


static int
testUnionRegion( void )
{
	region a( 0, 99, 0, 99 );
	region b( 25, 74, 25, 74 );
	region_list orL = a | b;

	if ( orL.size() == 1 && orL[0] == a )
	{
		orL = orL | b;
		if ( orL.size() == 1 && orL[0] == a )
			return 0;
		std::cout << "union list w/ region: " << orL << std::endl;
	}
	else
	{
		std::cout << "union region w/ region: " << orL << std::endl;
	}

	throw std::runtime_error( __PRETTY_FUNCTION__ );
}


////////////////////////////////////////


static int
testXORRegion( void )
{
	region a( 0, 99, 0, 99 );
	region b( 25, 74, 25, 74 );
	region_list xorL = a ^ b;

	if ( xorL.size() == 4 )
		return 0;

	throw std::runtime_error( __PRETTY_FUNCTION__ );
}


////////////////////////////////////////


int
main( int /*argc*/, char */*argv*/[] )
{
	int retval = 0;
	try
	{
		retval += testEmpty();
		retval += testSize();
		retval += testSizeReset();
		retval += testEquality();
		retval += testIntersect();
		retval += testIntersectRegion();
		retval += testUnionRegion();
		retval += testXORRegion();
	}
	catch ( std::exception &e )
	{
		std::cout << "ERROR: " << e.what() << std::endl;
		return -1;
	}

	return retval;
}
