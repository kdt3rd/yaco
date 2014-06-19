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
#include "impl/config.h"

namespace yaco
{

/// @brief Inverse of std::lock_guard
///
/// There are some patterns where you want to have a lock
/// while some work is set up, unlock to perform the (slow) task,
/// then re-lock to record said task. That can be accomplished
/// with the traditional lock_guard, but this can reduce the manual 
/// scope blocks having to be inserted into the code.
/// Used just as you would lock_guard, except it unlocks at initialization
/// and then re-locks at destruction time.
template <typename Mutex>
class unlock_guard
{
public:
	typedef Mutex mutex_type;

	_YACO_INLINE explicit unlock_guard( mutex_type &m ) : __m( m ) { __m.unlock(); }
	_YACO_INLINE unlock_guard( mutex_type &m, std::adopt_lock_t ) : __m( m ) {}
	_YACO_INLINE ~unlock_guard( void ) { __m.lock(); }

private:
	unlock_guard( const unlock_guard & ) = delete;
	unlock_guard &operator=( const unlock_guard & ) = delete;

	mutex_type &__m;
};

}


////////////////////////////////////////
// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:
