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

#pragma once

#include <cmath>
#include <type_traits>
#include <vector>

namespace yaco
{

namespace math
{

template <typename Storage, size_t dim>
class square_matrix
{
public:
	typedef Storage value_type;
	static const size_t size = dim;

private:
	value_type myV[size][size];
};

template <typename Storage>
class square_matrix<Storage, 2>
{
};

template <typename Storage>
class square_matrix<Storage, 3>
{
};

template <typename Storage>
class square_matrix<Storage, 4>
{
};

typedef square_matrix<float, 2> fmat2;
typedef square_matrix<float, 3> fmat3;
typedef square_matrix<float, 4> fmat4;

typedef square_matrix<double, 2> mat2;
typedef square_matrix<double, 3> mat3;
typedef square_matrix<double, 4> mat4;

typedef square_matrix<long double, 2> dmat2;
typedef square_matrix<long double, 3> dmat3;
typedef square_matrix<long double, 4> dmat4;


template <typename Storage, size_t dimX, size_t dimY>
class matrix2d
{
public:
	typedef Storage value_type;
	static const size_t x_size = dimX;
	static const size_t y_size = dimY;

private:
	value_type myV[y_size][x_size];
};

}

}

// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:

