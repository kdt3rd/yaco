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

#include "vector.h"
#include "matrix.h"

namespace yaco
{

namespace math
{

template <typename T, class Enable = void>
class quaternion;

template <typename StorageType = double>
class quaternion<StorageType, typename std::enable_if<std::is_floating_point<T>::value >::type>
{
public:
	typedef StorageType value_type;
	typedef typename std::complex<value_type> complex_type;
	typedef typename vector<Storage> vector_type;
	typedef typename square_matrix<Storage, 3> matrix_type;

	quaternion( void );
	quaternion( const value_type &a = static_cast<value_type>( 0 ),
				const value_type &b = static_cast<value_type>( 0 ),
				const value_type &c = static_cast<value_type>( 0 ),
				const value_type &d = static_cast<value_type>( 0 ) );
	quaternion( const complex_type &c1,
				const complex_type &c2 = complex_type() );
	template <class OS>
	quaternion( const quaternion<OS> &o );
	~quaternion( void );

	bool operator==( const quaternion &o );
	bool operator!=( const quaternion &o );

	quaternion &operator+=( const value_type &o );
	quaternion &operator+=( const complex_type &o );
	template <typename OtherStorage>
	quaternion &operator+=( const quaternion<OtherStorage> &o );

	quaternion &operator-=( const value_type &o );
	quaternion &operator-=( const complex_type &o );
	template <typename OtherStorage>
	quaternion &operator-=( const quaternion<OtherStorage> &o );

	quaternion &operator*=( const value_type &o );
	quaternion &operator*=( const complex_type &o );
	template <typename OtherStorage>
	quaternion &operator*=( const quaternion<OtherStorage> &o );

	quaternion &operator/=( const value_type &o );
	quaternion &operator/=( const complex_type &o );
	template <typename OtherStorage>
	quaternion &operator/=( const quaternion<OtherStorage> &o );

	/// Returns an angle and axis of rotation
	std::pair<value_type, vector_type> rotation( void ) const;

	/// Converts to a matrix representation
	matrix_type to_matrix( void ) const;

private:
	type x, y, z, w;
};

template <typename S>
quaternion<S> operator*( quaternion<S> &&a, quaternion<S> &&b )
{
	a *= b;
	return std::move( a );
}

template <typename S>
quaternion<S> operator*( quaternion<S> &&a, const quaternion<S> &b )
{
	a *= b;
	return std::move( a );
}

template <typename S>
quaternion<S> operator*( const quaternion<S> &a, quaternion<S> &&b )
{
	b *= a;
	return std::move( b );
}

template <typename S>
quaternion<S> operator*( const quaternion<S> &a, const quaternion<S> &b )
{
	quaternion<S> r( a );
	r *= b;
	return r;
}

template <typename S>
quaternion<S> operator+( quaternion<S> &&a, quaternion<S> &&b )
{
	a += b;
	return std::move( a );
}

template <typename S>
quaternion<S> operator+( quaternion<S> &&a, const quaternion<S> &b )
{
	a += b;
	return std::move( a );
}

template <typename S>
quaternion<S> operator+( const quaternion<S> &a, quaternion<S> &&b )
{
	b += a;
	return std::move( b );
}

template <typename S>
quaternion<S> operator+( const quaternion<S> &a, const quaternion<S> &b )
{
	quaternion<S> r( a );
	r += b;
	return r;
}

}

}

// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:

