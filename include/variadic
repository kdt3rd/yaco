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

#include "impl/config"

#include <cstddef>
#include <tuple>
#include <functional>

namespace yaco
{

/// @brief used to generate a sequence of indices as a variadic thing 
/// so we can deref a tuple or vector of values as a series of arguments
template <size_t ...> struct unpack_sequence {};
template <size_t N, size_t ...S> struct gen_sequence : public gen_sequence<N - 1, N - 1, S...> {};
template <size_t ...S>
struct gen_sequence<0, S...>
{
	typedef unpack_sequence<S...> type;
};

// private templated functions used for dispatching a function
// call from a tuple definition of it's arguments
namespace {

template <typename R, typename... FuncArgs, typename TupleType, size_t ...S>
R __dispatch_help( const std::function<R (FuncArgs...)> &f, const TupleType &args, const unpack_sequence<S...> & )
{
	return f( std::get<S>(args)... );
}

template <typename R, typename... FuncArgs, typename TupleType, size_t ...S>
R __dispatch_help( R (*f)(FuncArgs...), const TupleType &args, const unpack_sequence<S...> & )
{
	return f( std::get<S>(args)... );
}

}

/// @brief Enables one to store arguments for a function call as a tuple
/// for later execution, then call that function, dereferencing the tuple
/// into the individual arguments
template <typename R, typename... FuncArgs, typename TupleType>
R
call_func_from_tuple( const std::function<R (FuncArgs...)> &f, const TupleType &args )
{
	static_assert( sizeof...(FuncArgs) == std::tuple_size<TupleType>::value, "Mis-match of function argument count to arguments provided" );
	return __dispatch_help( f, args, typename gen_sequence<sizeof...(FuncArgs)>::type() );
}

template <typename R, typename... FuncArgs, typename TupleType>
void
call_func_from_tuple( R (*f)(FuncArgs...), const TupleType &args )
{
	static_assert( sizeof...(FuncArgs) == std::tuple_size<TupleType>::value, "Mis-match of function argument count to arguments provided" );
	__dispatch_help( f, args, typename gen_sequence<sizeof...(FuncArgs)>::type() );
}

} // namespace yaco


////////////////////////////////////////
// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:
