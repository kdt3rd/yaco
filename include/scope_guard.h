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

#include <functional>

#include "impl/config.h"

namespace yaco
{

/// @brief Mechanism to apply RAII principles to exception cleanup handling
///
/// Much like std::lock_guard for mutex-like things, or std::unique_ptr for
/// memory, this is a more generic version for doing just about anything
/// on exit of a particular scope, unless the guard is released
///
/// It has semantics similar to unique_lock or unique_ptr where
/// the "ownership" of the cleanup can be transfered via std::move
///
/// for example, to always close a file descriptor on exit of a function:
/// void foo()
/// {
///    int fd = ::open( "/dev/null", O_WRONLY );
///    scope_guard x = [&]() { if ( fd > -1 ) ::close( fd ); fd = -1 };
///    ... stuff
/// }
///
class scope_guard
{
public:
	scope_guard( void ) {}
	scope_guard( const std::function<void()> &f ) : myF( f ) {}
	scope_guard( std::function<void()> &&f ) : myF( f ) {}
	template <typename Lambda>
	scope_guard( Lambda &&f ) : myF( f ) {}
	scope_guard( scope_guard &&o ) noexcept { myF.swap( o.myF ); }

	scope_guard &operator=( scope_guard &&o ) noexcept
	{
		if ( myF )
			myF();

		myF = o.myF;
		o.myF = nullptr;
		return *this;
	}

	~scope_guard( void )
	{
		if ( myF )
			myF();
	}

	void release( void ) noexcept { myF = nullptr; }

private:
	scope_guard( const scope_guard & ) = delete;
	scope_guard &operator=( const scope_guard & ) = delete;

	std::function<void()> myF;
};

}


// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:
