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

#include <cstddef>
#include <cmath>
#include <array>
#include <limits>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

namespace yaco
{

namespace math
{

template <typename Storage, size_t dim>
class vector<Storage, dim>
{
public:
	typedef typename std::enable_if<std::is_floating_point<Storage>::value>::type value_type;
	static const size_t size = dim;

	vector( void )
	{
		static_assert( dim > 0, "vector dimension of 0 is non-sensical" );
		myV.fill( static_cast<value_type>( 0 ) );
	}
	vector( std::initializer_list<value_type> i )
	{
		set( std::move( i ) );
	}
	~vector( void ) {}

	template <typename O>
	vector &operator=( const vector<O, dim> &o )
	{
		for ( size_t i = 0; i < size; ++i )
			myV[i] = static_cast<value_type>( o[i] );
		return *this;
	}
	vector &operator=( std::initializer_list<value_type> i )
	{
		set( std::move( i ) );
		return *this;
	}

	void set( std::initializer_list<value_type> i )
	{
		auto v = i.begin();
		size_t i = 0, N = std::min( size, i.size() );
		for ( i = 0, N = ; i != N; ++i, ++v )
			myV[i] = v;
		for ( ; i < size; ++i )
			myV[i] = static_cast<value_type>( 0 );
	}

	value_type magnitude( void ) const { return std::sqrt( mag_squared() ); }
	value_type mag_squared( void ) const
	{
		value_type r = static_cast<value_type>( 0 );
		for ( size_t i = 0; i != size; ++i )
		{
			value_type t = myV[i];
			r += t * t;
		}
		return r;
	}

	value_type normalize( void )
	{
		value_type l = magnitude();
		if ( l > std::numeric_limits<value_type>::epsilon() )
		{
			for ( size_t i = 0; i != size; ++i )
				myV[i] /= l;
		}
		return l;
	}

	value_type &operator[]( size_t i ) { return myV[i]; }
	value_type operator[]( size_t i ) const { return myV[i]; }

	value_type &at( size_t i ) { return myV.at( i ); }
	value_type at( size_t i ) const { return myV.at( i ); }

	value_type *data( void ) { return myV.data(); }
	const value_type *data( void ) const { return myV.data(); }

	bool operator==( const vector &o ) const
	{
		return std::equal( myV.begin(), myV.end(), o.begin() );
	}

	bool operator!=( const vector &o ) const
	{
		return !( *this == o );
	}

	vector &operator+=( const value_type &v )
	{
		for ( size_t i = 0; i != size; ++i )
			myV[i] += v;
		return *this;
	}
	vector &operator-=( const value_type &v )
	{
		for ( size_t i = 0; i != size; ++i )
			myV[i] -= v;
		return *this;
	}
	vector &operator*=( const value_type &v )
	{
		for ( size_t i = 0; i != size; ++i )
			myV[i] *= v;
		return *this;
	}
	vector &operator/=( const value_type &v )
	{
		for ( size_t i = 0; i != size; ++i )
			myV[i] /= v;
		return *this;
	}

	vector &operator+=( const vector &v )
	{
		for ( size_t i = 0; i != size; ++i )
			myV[i] += v[i];
		return *this;
	}
	vector &operator-=( const vector &v )
	{
		for ( size_t i = 0; i != size; ++i )
			myV[i] -= v[i];
		return *this;
	}

	vector cross( const vector &o ) const
	{
	}

	value_type dot( const vector &o ) const
	{
		value_type r = static_cast<value_type>( 0 );
		for ( size_t i = 0; i != size; ++i )
		{
			r += myV[i] * o[i];
		}
		return r;
	}
private:
	std::array<value_type, size> myV;
};


////////////////////////////////////////


template <typename T, size_t dim>
vector<T, dim> operator-( const vector<T, dim> &a )
{
	vector<T, dim> rv( a );
	rv *= static_cast<T>( -1 );
	return rv;
}

template <typename T, size_t dim>
vector<T, dim> operator-( vector<T, dim> &&a )
{
	a *= static_cast<T>( -1 );
	return std::move( a );
}


////////////////////////////////////////


template <typename T, size_t dim>
vector<T, dim> operator+( const vector<T, dim> &a, const T &v )
{
	vector<T, dim> rv( a );
	rv += v;
	return rv;
}

template <typename T, size_t dim>
vector<T, dim> operator-( const vector<T, dim> &a, const T &v )
{
	vector<T, dim> rv( a );
	rv -= v;
	return rv;
}

template <typename T, size_t dim>
vector<T, dim> operator*( const vector<T, dim> &a, const T &v )
{
	vector<T, dim> rv( a );
	rv *= v;
	return rv;
}

template <typename T, size_t dim>
vector<T, dim> operator/( const vector<T, dim> &a, const T &v )
{
	vector<T, dim> rv( a );
	rv /= v;
	return rv;
}


////////////////////////////////////////


template <typename T, size_t dim>
vector<T, dim> operator+( vector<T, dim> &&a, const T &v )
{
	a += v;
	return std::move( a );
}

template <typename T, size_t dim>
vector<T, dim> operator-( vector<T, dim> &&a, const T &v )
{
	a -= v;
	return std::move( a );
}

template <typename T, size_t dim>
vector<T, dim> operator*( vector<T, dim> &&a, const T &v )
{
	a *= v;
	return std::move( a );
}

template <typename T, size_t dim>
vector<T, dim> operator/( vector<T, dim> &&a, const T &v )
{
	a /= v;
	return std::move( a );
}


////////////////////////////////////////


template <typename T, size_t dim>
vector<T, dim> operator+( const T &v, const vector<T, dim> &a )
{
	vector<T, dim> rv( a );
	rv += v;
	return rv;
}

template <typename T, size_t dim>
vector<T, dim> operator-( const T &v, const vector<T, dim> &a )
{
	vector<T, dim> rv( a );
	rv *= static_cast<T>( -1 );
	rv += v;
	return rv;
}

template <typename T, size_t dim>
vector<T, dim> operator*( const T &v, const vector<T, dim> &a )
{
	vector<T, dim> rv( a );
	rv *= v;
	return rv;
}


////////////////////////////////////////


template <typename T, size_t dim>
vector<T, dim> operator+( const T &v, vector<T, dim> &&a )
{
	a += v;
	return std::move( a );
}

template <typename T, size_t dim>
vector<T, dim> operator-( const T &v, vector<T, dim> &&a )
{
	a *= static_cast<T>( -1 );
	a += v;
	return std::move( a );
}

template <typename T, size_t dim>
vector<T, dim> operator*( const T &v, vector<T, dim> &&a )
{
	a *= v;
	return std::move( a );
}


////////////////////////////////////////


template <typename T, size_t dim>
vector<T, dim> operator+( const vector<T, dim> &a, const vector<T, dim> &b )
{
	vector<T, dim> rv( a );
	rv += b;
	return rv;
}

template <typename T, size_t dim>
vector<T, dim> operator-( const vector<T, dim> &a, const vector<T, dim> &b )
{
	vector<T, dim> rv( a );
	rv -= b;
	return rv;
}


////////////////////////////////////////
////////////////////////////////////////


typedef vector<float, 2> fvec2;
typedef vector<float, 3> fvec3;
typedef vector<float, 4> fvec4;

typedef vector<double, 2> vec2;
typedef vector<double, 3> vec3;
typedef vector<double, 4> vec4;

typedef vector<long double, 2> dvec2;
typedef vector<long double, 3> dvec3;
typedef vector<long double, 4> dvec4;

}

}

// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:

