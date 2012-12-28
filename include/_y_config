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

#include <cstdint>

/// This is a mirror of some of what libc++ and similar set up

#ifdef _WIN32
// Need to deal with dll visibility and all that at some point

#ifndef _YACO_INLINE
# if _MSC_VER
#  define _YACO_INLINE __forceinline
# else // MinGW GCC and Clang
#  define _YACO_INLINE __attribute__ ((__always_inline__))
# endif
#endif

#endif

#ifndef _YACO_INLINE
# define _YACO_INLINE __attribute__ ((__visibility__("hidden"), __always_inline__))
#endif


////////////////////////////////////////
// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:
