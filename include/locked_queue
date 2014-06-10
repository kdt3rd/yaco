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

#include <mutex>
#include <condition_variable>
#include <stdexcept>


////////////////////////////////////////


namespace yaco
{

/// @brief Class locked_queue provides a FIFO queue w/ mutex thread safety.
///
/// It is not really meant to be used as a standard container, but has
/// some of the functions similar to std::deque.
/// As an exception, one can swap two locked_queues safely, but not
/// using std::swap
template <typename T>
class locked_queue
{
public:
	typedef T entry_type;

	locked_queue( void ) {}
	~locked_queue( void ) {}

	void push( const entry_type &e )
	{
		std::lock_guard<std::mutex> sg( myLock );
		myEntries.push_back( e );
	}
	
	void push( entry_type &&e )
	{
		std::lock_guard<std::mutex> sg( myLock );
		myEntries.emplace_back( std::move( e ) );
	}

	entry_type pop( void )
	{
		std::unique_lock<std::mutex> sg( myLock );

		// does every compiler we care about have lambda functions?
//		myCond.wait( sg, [this](){ return !myEntries.empty(); } );
		while ( myEntries.empty() )
			myCond.wait( sg );

		// function scope, not global, but if contained
		// element is more complex and has a specific swap
		// override or move semantics, will be more efficient
		using std::swap;
		entry_type ret;
		swap( myEntries.front(), ret );
		myEntries.pop_front();
		return ret;
	}

	entry_type try_pop( const entry_type &emptyVal = entry_type() )
	{
		std::lock_guard<std::mutex> sg( myLock );

		// function scope, not global, but if contained
		// element is more complex and has a specific swap
		// override or move semantics, will be more efficient
		using std::swap;
		entry_type ret( emptyVal );

		if ( ! myEntries.empty() )
		{
			swap( myEntries.front(), ret );
			myEntries.pop_front();
		}

		return ret;
	}

	bool clear( void ) const
	{
		std::lock_guard<std::mutex> sg( myLock );
		return myEntries.clear();
	}

	bool empty( void ) const
	{
		std::lock_guard<std::mutex> sg( myLock );
		return myEntries.empty();
	}

	size_t size( void ) const
	{
		std::lock_guard<std::mutex> sg( myLock );
		return myEntries.size();
	}

	void swap( locked_queue &o )
	{
		if ( &o == this )
			throw std::logic_error( "attempt to swap locked_queue with itself" );

		std::lock_guard<std::mutex> sg( myLock );
		std::lock_guard<std::mutex> so( o.myLock );
		myEntries.swap( o.myEntries );
	}

	// Normal copy construction and assignment not allowed
	locked_queue( const locked_queue & ) = delete;
	locked_queue( locked_queue && ) = delete;
	locked_queue &operator=( const locked_queue & ) = delete;
	locked_queue &operator=( locked_queue && ) = delete;
private:

	std::condition_variable myCond;
	std::mutex myLock;
	std::deque<entry_type> myEntries;
};

template <typename T>
swap( locked_queue<T> &x, locked_queue<T> &y )
{
	x.swap( y );
}

} // namespace yaco


// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:

