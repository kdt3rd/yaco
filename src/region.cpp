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
#include <set>
#include <list>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <algorithm>


////////////////////////////////////////


namespace
{

using namespace yaco::math;

struct edge
{
	region::value_type left, right, y;
	int inA, inB;
	union 
	{
		bool out;
		char __align_buf[alignof(region::value_type)];
	};
	edge( region::value_type l, region::value_type r, region::value_type p,
		  int a, int b, bool o )
			: left( l ), right( r ), y( p ), inA( a ), inB( b ), out( o )
	{}

	bool operator<( const edge &o ) const { return y < o.y; }
};

void add_region( region_list &retval,
				 region_list::iterator &curout,
				 region::value_type l,
				 region::value_type r,
				 region::value_type b,
				 region::value_type t )
{
	region::value_type nt = t;
	if ( nt != std::numeric_limits<region::value_type>::max() )
		--nt;

	if ( curout == retval.end() )
	{
		retval.push_back( region( l, r, b, nt ) );
		curout = retval.end();
	}
	else
	{
		(*curout) = region( l, r, b, nt );
		++curout;
	}
}

void
merge_sweep( region_list &retval,
			 region_list::iterator &curout,
			 std::list<edge> &sweep, std::list<edge>::iterator s,
			 const std::function<bool (int, int)> &op )
{
	auto o = s;
	--o;

	if ( (*s).out == (*o).out && (*s).inA == (*o).inA && (*s).inB == (*o).inB )
	{
		if ( op( (*s).inA, (*s).inB ) )
		{
			if ( (*s).y < (*o).y )
			{
				add_region( retval, curout, (*s).left, (*s).right, (*s).y, (*o).y );
				(*s).y = (*o).y;
			}
			else if ( (*s).y > (*o).y )
			{
				add_region( retval, curout, (*o).left, (*o).right, (*o).y, (*s).y );

				(*s).y = (*o).y;
			}
		}

		(*s).left = (*o).left;
		sweep.erase( o );
	}
}

void
region_set_op( region_list &retval,
			   const region_list &la,
			   const region_list &lb,
			   const std::function<bool (int, int)> &op )
{
	std::multiset<edge> edges;
	std::list<edge> sweep;
	constexpr region::value_type neg_inf = std::numeric_limits<region::value_type>::min();
	constexpr region::value_type pos_inf = std::numeric_limits<region::value_type>::max();

	for ( auto a = la.begin(); a != la.end(); ++a )
	{
		if ( ! (*a).empty() )
		{
			edges.insert( edge( (*a).left(), (*a).right(),
								std::max( (*a).top() + 1, (*a).top() ),
								1, 0, true ) );
			edges.insert( edge( (*a).left(), (*a).right(), (*a).bottom(),
								1, 0, false ) );
		}
	}

	for ( auto b = lb.begin(); b != lb.end(); ++b )
	{
		if ( ! (*b).empty() )
		{
			edges.insert( edge( (*b).left(), (*b).right(),
								std::max( (*b).top() + 1, (*b).top() ),
								0, 1, true ) );
			edges.insert( edge( (*b).left(), (*b).right(), (*b).bottom(),
								0, 1, false ) );
		}
	}

	auto outi = retval.begin();

	sweep.push_front( edge( neg_inf, pos_inf, neg_inf, 0, 0, false ) );

	for ( auto e = edges.cbegin(); e != edges.cend(); ++e )
	{
		auto s = sweep.begin();

		while ( (*e).left > (*s).right )
			++s;

		// intersect the edge with the sweep line and
		// create any regions
		while ( (*e).right >= (*s).left && s != sweep.end() )
		{
			if ( op( (*s).inA, (*s).inB ) && (*s).y < (*e).y )
			{
				add_region( retval, outi, (*s).left, (*s).right, (*s).y, (*e).y );
				(*s).y = (*e).y;
			}

			region::value_type nl = std::max( (*e).left, (*s).left );
			region::value_type nr = std::min( (*e).right, (*s).right );

			// split beginning
			if ( nl != (*s).left )
			{
				if ( nl == neg_inf )
					throw std::runtime_error( "Negative infinity when trying to split edges" );

				sweep.insert( s,
							  edge( (*s).left, nl - 1, (*s).y,
									(*s).inA, (*s).inB, (*s).out ) );

				(*s).left = nl;
			}

			// split end
			if ( nr != (*s).right )
			{
				if ( nr == pos_inf )
					throw std::runtime_error( "Positive infinity when trying to split edges" );

				sweep.insert( s,
							  edge( (*s).left, nr, (*s).y,
									(*s).inA, (*s).inB, (*s).out ) );

				(*s).left = nr + 1;

				// switch to prior
				if ( s == sweep.begin() )
					throw std::runtime_error( "Beginning of sweep list when splitting end" );

				--s;
			}

			// update flags
			if ( (*e).out )
			{
				(*s).inA = (*s).inA - (*e).inA;
				(*s).inB = (*s).inB - (*e).inB;
				if ( (*s).inA < 0 )
					throw std::runtime_error( "Invalid edge count for a list" );
				if ( (*s).inB < 0 )
					throw std::runtime_error( "Invalid edge count for b list" );
			}
			else
			{
				(*s).inA = (*s).inA + (*e).inA;
				(*s).inB = (*s).inB + (*e).inB;
			}

			// new y
			(*s).y = (*e).y;

			if ( s != sweep.begin() )
				merge_sweep( retval, outi, sweep, s, op );

			++s;
		}

		// after trying to intersect, see if we can join the next edge
		if ( s != sweep.end() && s != sweep.end() )
			merge_sweep( retval, outi, sweep, s, op );
	}

	if ( outi != retval.end() )
		retval.erase( outi, retval.end() );

	sort_and_merge( retval );
}

region_list
computeXOR( const region &a, const region &b )
{
	region_list retval;

	if ( a.intersects( b ) )
	{
		if ( a == b )
			return retval;

		// have an intersection we care about
		//
		// up to 4 regions output
		// 
		// +-------+
		// |   | 2 |
		// | 1 +--------+
		// |   |   |    |
		// +---|---+  4 |
		//     | 3 |    |
		//     +--------+
		//
		// +-------+
		// |   1   |
		// +-----------+
		// |       |   |
		// +-------+ 3 |
		// |   2   |   |
		// +-----------+
		//
		// +-------+
		// |   1   |
		// +-------+
		// |       |
		// +-------+
		// |   2   |
		// +-------+
		//
		// +---+---+---+
		// |   |   |   |
		// | 1 |   | 2 |
		// |   |   |   |
		// +---+---+---+
		//
		if ( a.left() != b.left() )
		{
			region::value_type nl = std::min( a.left(), b.left() );
			region::value_type nr = std::max( a.left(), b.left() ) - 1;
			if ( nl == a.left() )
				retval.push_back( region( nl, nr, a.bottom(), a.top() ) );
			else
				retval.push_back( region( nl, nr, b.bottom(), b.top() ) );
		}

		if ( a.bottom() != b.bottom() )
		{
			region::value_type nb = std::min( a.bottom(), b.bottom() );
			region::value_type nt = std::max( a.bottom(), b.bottom() ) - 1;
			if ( nb == a.bottom() )
				retval.push_back( region( a.left(), a.right(), nb, nt ) );
			else
				retval.push_back( region( b.left(), b.right(), nb, nt ) );
		}

		if ( a.right() != b.right() )
		{
			region::value_type nl = std::min( a.right(), b.right() ) + 1;
			region::value_type nr = std::max( a.right(), b.right() );
			if ( nr == a.right() )
				retval.push_back( region( nl, nr, a.bottom(), a.top() ) );
			else
				retval.push_back( region( nl, nr, b.bottom(), b.top() ) );
		}

		if ( a.top() != b.top() )
		{
			region::value_type nb = std::min( a.top(), b.top() ) + 1;
			region::value_type nt = std::max( a.top(), b.top() );
			if ( nt == a.top() )
				retval.push_back( region( a.left(), a.right(), nb, nt ) );
			else
				retval.push_back( region( b.left(), b.right(), nb, nt ) );
		}
	}
	else
	{
		retval.push_back( a );
		retval.push_back( b );
	}

	sort_and_merge( retval );

	return retval;
}

region_list
computeOR( const region &a, const region &b )
{
	region_list retval;

	if ( a == b || a.inside( b ) )
		retval.push_back( b );
	else if ( b.inside( a ) )
		retval.push_back( a );
	else if ( ! a.intersects( b ) )
	{
		retval.push_back( a );
		retval.push_back( b );
	}
	else
	{
		// have an intersection we care about
		//
		// up to 3 regions output
		// 
		// +-------+
		// |   1   |
		// |---+-------+
		// |     2     |    A
		// +---|---+---|
		//     |   3   |
		//     +-------+
		//
		//     +-------+
		//     |   1   |
		// +-----------+
		// |           |    B
		// |   +  2    +
		// |           |
		// +-----------+
		//
		//     +-------+
		//     |       |
		// +----       +
		// |   |       |
		// | 1 |   2   |    C
		// |   |       |
		// +----       +
		//     |       |
		//     +-------+
		//
		// +-------+
		// |   1   |
		// +-----------+
		// |           |    D
		// +     2 +   |
		// |           |
		// +-----------+
		//
		// +-------+
		// |       |
		// +       |---+
		// |       |   |
		// |   1   | 2 |    E
		// |       |   |
		// +       |---+
		// |       |
		// +-------+
		// 
		// +-------+
		// |       |
		// +       +
		// |   1   | F
		// +       +
		// |       |
		// +-------+
		//
		// +---+---+---+
		// |           |
		// |     1     | G
		// |           |
		// +---+---+---+
		//
		if ( a.left() != b.left() )
		{
			region::value_type nl = std::min( a.left(), b.left() );
			region::value_type nr = std::max( a.right(), b.right() );
			region::value_type nb = std::max( a.bottom(), b.bottom() );
			region::value_type nt = std::min( a.top(), b.top() );

			if ( a.bottom() == nb && a.top() == nt )
			{
				// case C
				retval.push_back( region( a.left(), a.right(), nb, nt ) );
				retval.push_back( region( nl, nr, b.bottom(), b.top() ) );
			}
			else if ( b.bottom() == nb && b.top() == nt )
			{
				// case C
				retval.push_back( region( b.left(), b.right(), nb, nt ) );
				retval.push_back( region( nl, nr, a.bottom(), a.top() ) );
			}
			else
			{
				// case A or B
				retval.push_back( region( nl, nr, nb, nt ) );

				if ( a.top() > nt )
					retval.push_back(
						region( a.left(), a.right(), nt + 1, a.top() ) );
				else if ( b.top() > nt )
					retval.push_back(
						region( b.left(), b.right(), nt + 1, b.top() ) );

				if ( a.bottom() < nb )
					retval.push_back(
						region( a.left(), a.right(), a.bottom(), nb - 1 ) );
				else if ( b.bottom() < nb )
					retval.push_back(
						region( b.left(), b.right(), b.bottom(), nb - 1 ) );
			}
		}
		else if ( a.right() != b.right() )
		{
			region::value_type nl = a.left(); // left is the same
			region::value_type nr = std::max( a.right(), b.right() );
			region::value_type nb = std::max( a.bottom(), b.bottom() );
			region::value_type nt = std::min( a.top(), b.top() );

			if ( a.bottom() == nb && a.top() == nt )
			{
				// case E
				retval.push_back( region( a.left(), a.right(), nb, nt ) );
				retval.push_back( region( nl, nr, b.bottom(), b.top() ) );
			}
			else if ( b.bottom() == nb && b.top() == nt )
			{
				// case E
				retval.push_back( region( b.left(), b.right(), nb, nt ) );
				retval.push_back( region( nl, nr, a.bottom(), a.top() ) );
			}
			else
			{
				// case D
				retval.push_back( region( nl, nr, nb, nt ) );

				if ( a.top() > nt )
					retval.push_back(
						region( a.left(), a.right(), nt + 1, a.top() ) );
				else if ( b.top() > nt )
					retval.push_back(
						region( b.left(), b.right(), nt + 1, b.top() ) );

				if ( a.bottom() < nb )
					retval.push_back(
						region( a.left(), a.right(), a.bottom(), nb - 1 ) );
				else if ( b.bottom() < nb )
					retval.push_back(
						region( b.left(), b.right(), b.bottom(), nb - 1 ) );
			}
		}
		else
		{
			region tmp = a;
			tmp.merge( b );
			retval.push_back( tmp );
		}
	}

	sort_and_merge( retval );

	return std::move( retval );
}

} // empty namespace


////////////////////////////////////////


namespace yaco
{
namespace math
{


////////////////////////////////////////


void
region::reset( infinite_tag_t )
{
	myL = myB = std::numeric_limits<value_type>::min();
	myR = myT = std::numeric_limits<value_type>::max();
}


////////////////////////////////////////


void
region::reset_by_size( value_type ox, value_type oy, value_type sx, value_type sy )
{
	myL = ox;
	myB = oy;
	myR = ox + sx - 1;
	myT = oy + sy - 1;
}


////////////////////////////////////////


void
region::set_offset( int ox, int oy )
{
	if ( empty() )
	{
		myL = ox;
		myB = oy;
		myR = ox - 1;
		myT = oy - 1;
	}
	else
	{
		if ( infinite_x() )
		{
			myL = ox;
		}
		else
		{
			value_type s = size_x();
			myL = ox;
			myR = ox + s - 1;
		}

		if ( infinite_y() )
		{
			myB = oy;
		}
		else
		{
			value_type s = size_y();
			myB = oy;
			myT = oy + s - 1;
		}
	}
}


////////////////////////////////////////


void
region::set_size( int sx, int sy )
{
	myR = myL + sx - 1;
	myT = myB + sy - 1;
}


////////////////////////////////////////


void
region::intersect( const region &o )
{
	myL = std::max( myL, o.myL );
	myR = std::min( myR, o.myR );
	if ( myR < myL )
		myR = myL - 1;

	myB = std::max( myB, o.myB );
	myT = std::min( myT, o.myT );
	if ( myT < myB )
		myT = myB - 1;
}


////////////////////////////////////////


void
region::merge( const region &o )
{
	myL = std::min( left(), o.left() );
	myR = std::max( right(), o.right() );
	myB = std::min( bottom(), o.bottom() );
	myT = std::max( top(), o.top() );
}


////////////////////////////////////////


void
region::grow( value_type left, value_type right, value_type bottom, value_type top )
{
	if ( myL > std::numeric_limits<value_type>::min() )
		myL -= left;
	if ( myR < std::numeric_limits<value_type>::max() )
		myR += right;
	if ( myR < myL )
		myR = myL - 1;

	if ( myB > std::numeric_limits<value_type>::min() )
		myB -= bottom;
	if ( myT < std::numeric_limits<value_type>::max() )
		myT += top;
	if ( myT < myB )
		myT = myB - 1;
}


////////////////////////////////////////


std::ostream &
operator<<( std::ostream &out, const region &r )
{
	out << "region( " << r.left() << ", " << r.right() << ", " << r.bottom()
		<< ", " << r.top() << " )";
	return out;
}


////////////////////////////////////////


std::ostream &
operator<<( std::ostream &out, const region_list &rl )
{
	for ( auto r = rl.begin(); r != rl.end(); ++r )
	{
		if ( r != rl.begin() )
			out << ", ";
		out << (*r);
	}
	return out;
}


////////////////////////////////////////


void
sort_and_merge( region_list &a )
{
	if ( a.size() <= 1 )
		return;

	bool made_change;
	do
	{
		made_change = false;
		auto i = a.begin();
		while ( i != a.end() )
		{
			region_list::difference_type xi = i - a.begin();

			if ( (*i).empty() )
			{
				a.erase( i );
				i = a.begin() + xi;
				continue;
			}

			auto o = i;
			++o;
			while ( o != a.end() )
			{
				const region &c = (*i);
				const region &p = (*o);

				if ( p.inside( c ) ||
					 ( c.left() == p.left() && c.right() == p.right() &&
					   ( c.contains_y( p.bottom() ) ||
						 p.contains_y( c.bottom() ) ||
						 c.bottom() == ( p.top() + 1 ) ||
						 c.top() == ( p.bottom() - 1 ) ) ) ||
					 ( c.bottom() == p.bottom() && c.top() == p.top() &&
					   ( c.contains_x( p.left() ) ||
						 p.contains_x( c.left() ) ||
						 c.left() == ( p.right() - 1 ) ||
						 c.right() == ( p.left() + 1 ) ) ) )
				{
					(*i).merge( p );

					a.erase( o );
					i = a.begin() + xi;
					// restart the search in case we can merge more
					o = i;
					++o;
					made_change = true;
					continue;
				}
				++o;
			}

			++i;
		}
	} while ( made_change );

	std::sort( a.begin(), a.end() );
}


////////////////////////////////////////


region_list
operator&( region_list la, const region_list &lb )
{
	region_set_op( la, la, lb, []( int a, int b ) { return a > 0 && b > 0; } );
	return std::move( la );
}


////////////////////////////////////////


region_list
operator&( const region_list &a, const region &b )
{
	region_list tmp{ b };
	return std::move( tmp ) & a;
}


////////////////////////////////////////


region_list
operator&( const region &a, const region_list &b )
{
	region_list tmp{ a };
	return std::move( tmp ) & b;
}


////////////////////////////////////////


region_list
operator&( const region &a, const region &b )
{
	region_list retval;

	region tmp( a );
	tmp.intersect( b );

	if ( ! tmp.empty() )
		retval.push_back( tmp );

	return std::move( retval );
}


////////////////////////////////////////


region_list
operator|( region_list la, const region_list &lb )
{
	region_set_op( la, la, lb, []( int a, int b ) { return a > 0 || b > 0; } );
	return std::move( la );
}


////////////////////////////////////////


region_list
operator|( const region_list &a, const region &b )
{
	region_list tmp{ b };
	return std::move( tmp ) | a;
}


////////////////////////////////////////


region_list
operator|( const region &a, const region_list &b )
{
	region_list tmp{ a };
	return std::move( tmp ) | b;
}


////////////////////////////////////////


region_list
operator|( const region &a, const region &b )
{
	return computeOR( a, b );
}


////////////////////////////////////////


region_list
operator^( region_list la, const region_list &lb )
{
	region_set_op( la, la, lb, []( int a, int b ) { return ( a > 0 && b == 0 ) || ( b > 0 && a == 0 ); } );
	return std::move( la );
}


////////////////////////////////////////


region_list
operator^( const region_list &a, const region &b )
{
	region_list tmp{ b };
	return std::move( tmp ) ^ a;
}


////////////////////////////////////////


region_list
operator^( const region &a, const region_list &b )
{
	region_list tmp{ a };
	return std::move( tmp ) ^ b;
}


////////////////////////////////////////


region_list
operator^( const region &a, const region &b )
{
	return computeXOR( a, b );
}


////////////////////////////////////////


region_list
operator~( const region &a )
{
	return computeXOR( region( region::inf ), a );
}


////////////////////////////////////////


region_list
operator~( const region_list &a )
{
	region_list retval{ region( region::inf ) };

	return std::move( retval ) ^ a;
}


////////////////////////////////////////


region_list
not_in( region_list la, const region_list &lb )
{
	region_set_op( la, la, lb, []( int a, int b ) { return a > 0 && b == 0; } );
	return std::move( la );
}


////////////////////////////////////////


region_list
not_in( const region_list &a, const region &b )
{
	region_list tmp{ b };
	return not_in( std::move( tmp ), a );
}


////////////////////////////////////////


region_list
not_in( const region &a, const region_list &b )
{
	region_list tmp = { a };
	return not_in( std::move( tmp ), b );
}


////////////////////////////////////////


} // math
} // yaco



