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
#include <limits>
#include <vector>
#include <iosfwd>

#include "../impl/config.h"

namespace yaco
{

namespace math
{

/// @brief Abstracts the concept of a 2-D region (vs. a 3-D volume)
///        with appropriate operators on the bounding box
///
/// NB: This is a region defined using integers, primarily intended
/// for use in dealing with pixel coverage.
/// A more generic volume class would probably deal with floating point.
///
/// Until then, this remains NOT templated.
class region
{
public:
	typedef int value_type;

	struct infinite_tag_t {};
	constexpr static infinite_tag_t inf = infinite_tag_t();

	/// @brief Create an empty box
	constexpr region( void ) : myL( 0 ), myR( -1 ), myB( 0 ), myT( -1 ) {}
	/// @brief Create a specific region
	constexpr region( value_type l, value_type r, value_type b, value_type t ) : myL( l ), myR( r ), myB( b ), myT( t ) {}
	/// @brief Create an infinite bounding box
	constexpr region( infinite_tag_t ) : myL( std::numeric_limits<value_type>::min() ), myR( std::numeric_limits<value_type>::max() ), myB( std::numeric_limits<value_type>::min() ), myT( std::numeric_limits<value_type>::max() ) {}

	constexpr value_type left( void ) const { return myL; }
	constexpr value_type right( void ) const { return myR; }
	constexpr value_type bottom( void ) const { return myB; }
	constexpr value_type top( void ) const { return myT; }

	constexpr value_type offset_x( void ) const { return myL; }
	constexpr value_type offset_y( void ) const { return myB; }

	/// NB: for infinite regions, this will return 0
	constexpr value_type size_x( void ) const { return myR - myL + 1; }
	/// NB: for infinite regions, this will return 0
	constexpr value_type size_y( void ) const { return myT - myB + 1; }

	void clear( void ) { myL = myB = 0; myR = myT = -1; }
	void reset( infinite_tag_t );
	void reset( value_type l, value_type r, value_type b, value_type t ) { myL = l; myR = r; myB = b; myT = t; }
	void reset_by_size( value_type ox, value_type oy, value_type sx, value_type sy );

	/// preserves size
	void set_offset( int ox, int oy );
	/// Leaves left, bottom (offset) alone
	void set_size( int sx, int sy );

	constexpr bool empty( void ) const { return myR < myL || myT < myB; }
	constexpr bool infinite( void ) const { return infinite_x() || infinite_y(); }
	constexpr bool infinite_x( void ) const { return infinite_l() || infinite_r(); }
	constexpr bool infinite_y( void ) const { return infinite_b() || infinite_t(); }
	constexpr bool infinite_l( void ) const { return myL == std::numeric_limits<value_type>::min(); }
	constexpr bool infinite_r( void ) const { return myR == std::numeric_limits<value_type>::max(); }
	constexpr bool infinite_b( void ) const { return myB == std::numeric_limits<value_type>::min(); }
	constexpr bool infinite_t( void ) const { return myT == std::numeric_limits<value_type>::max(); }

	constexpr bool equal( const region &o ) const { return *this == o; }
	constexpr bool operator==( const region &o ) const { return myL == o.myL && myR == o.myR && myB == o.myB && myT == o.myT; }
	constexpr bool operator!=( const region &o ) const { return !( *this == o ); }
	constexpr bool operator<( const region &o ) const
	{
		return ( ( bottom() < o.bottom() ) ||
				 ( bottom() == o.bottom() && left() < o.left() ) ||
				 ( bottom() == o.bottom() && left() == o.left() && top() < o.top() ) ||
				 ( bottom() == o.bottom() && left() == o.left() && top() == o.top() && right() < o.right() ) );
	}

	/// @brief returns true if this region is inside the provided region
	constexpr bool inside( const region &o ) const { return left() >= o.left() && right() <= o.right() && bottom() >= o.bottom() && top() <= o.top(); }
	constexpr bool intersects( const region &o ) const { return left() <= o.right() && right() >= o.left() && bottom() <= o.top() && top() >= o.bottom(); }

	constexpr bool contains_x( value_type x ) const { return myL <= x && x <= myR; }
	constexpr bool contains_y( value_type y ) const { return myB <= y && y <= myT; }
	constexpr bool contains( value_type x, value_type y ) const { return contains_x( x ) && contains_y( y ); }

	void intersect( const region &o );
	void merge( const region &o );

	/// NB: border conditions exist with very large regions
	///     (real coordinates approaching numeric limits)
	///     and growing / shrinking regions
	/// @{
	inline void grow( value_type i ) { grow( i, i, i, i ); }
	inline void grow( value_type x, value_type y ) { grow( x, x, y, y ); }
	void grow( value_type left, value_type right, value_type bottom, value_type top );
	inline void shrink( value_type i ) { grow( -i, -i, -i, -i ); }
	inline void shrink( value_type x, value_type y ) { grow( -x, -x, -y, -y ); }
	inline void shrink( value_type left, value_type right, value_type bottom, value_type top ) { grow( -left, -right, -bottom, -top ); }
	/// @}

private:
	int myL, myR, myB, myT;
};

typedef std::vector<region> region_list;

std::ostream &operator<<( std::ostream &out, const region &r );
std::ostream &operator<<( std::ostream &out, const region_list &rl );

void sort_and_merge( region_list &a );	

// c is always a region_list, but a and b can be either a region
// or a region_list
// c = a & b;
// c = a | b;
// c = a ^ b;
//
// c = a & ~b;
// c = a | ~b;

/// Computes a set of regions that represents the exact intersection (AND)
/// of the two inputs, returning the result as a list of regions.
/// The resulting list of regions will be merged together where possible:
/// if there are two regions in the list that share a side that are the
/// same dimension along that side, those regions will be merged into a
/// larger region.
/// @{
region_list operator&( region_list a, const region_list &b );
region_list operator&( const region_list &a, const region &b );
region_list operator&( const region &a, const region_list &b );
region_list operator&( const region &a, const region &b );
/// @}
	
/// Computes a set of regions that represents the union (OR)
/// of the two inputs, returning the result as a list of regions.
/// The resulting list of regions will be merged together where possible:
/// if there are two regions in the list that share a side that are the
/// same dimension along that side, those regions will be merged into a
/// larger region.
/// @{
region_list operator|( region_list a, const region_list &b );
region_list operator|( const region_list &a, const region &b );
region_list operator|( const region &a, const region_list &b );
region_list operator|( const region &a, const region &b );
/// @}
	
/// Computes a set of regions that represents the 'exclusive or' (XOR)
/// of the two inputs, returning the result as a list of regions.
/// The resulting list of regions will be merged together where possible:
/// if there are two regions in the list that share a side that are the
/// same dimension along that side, those regions will be merged into a
/// larger region.
/// @{
region_list operator^( region_list a, const region_list &b );
region_list operator^( const region_list &a, const region &b );
region_list operator^( const region &a, const region_list &b );
region_list operator^( const region &a, const region &b );
/// @}
	
/// Generates a list of regions that represents the area outside
/// all of the elements of a (NOT).
/// @{
region_list operator~( const region &a );
region_list operator~( const region_list &a );
/// @}

/// elements of A NOT in B
/// @{
region_list not_in( region_list a, const region_list &b );
region_list not_in( const region_list &a, const region &b );
region_list not_in( const region &a, const region_list &b );
/// @}

}

}

// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:
