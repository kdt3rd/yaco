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

#include "impl/arg_parser_stores.h"

namespace yaco
{

/// @brief A command line argument parsing class
///
/// This is a basic argument parsing class. for example, it is not
/// designed to be particularly smart about handling repeated
/// arguments of the same argument, by default just overwriting the
/// previous value. if your application requires more complex handling,
/// it is suggested to sub-class and manage the state
///
/// The scope of this and the argument options needs attention. This
/// class can take direct references to variables, to in those
/// scenarios, the variables must exist for the lifetime of this
/// object.
///
/// This class assumes the destination variables have already been
/// initialized to their default values, and as such, does not
/// explicitly initialize any variable references it receives.
class arg_parser
{
public:
	inline arg_parser( void ) {}
	inline virtual ~arg_parser( void ) {}

	// if these are specified, they will be added to the
	// default help
	inline void program_name( const std::string &name ) { _program_name = name; }
	inline const std::string &program_name( void ) const { return _program_name; }

	inline void program_version( const std::string &ver ) { _program_version = ver; }
	inline const std::string &program_version( void ) const { return _program_version; }
	inline void build_date( const std::string &date ) { _build_date = date; }
	inline const std::string &build_date( void ) const { return _build_date; }

	inline const std::string &argv0( void ) const { return _argv0; }

	/// @brief adds a flag to the command line processing
	///
	/// A flag takes no additional arguments
	/// @param key         - shorthand character (i.e. -r for --recursive)
	/// @param name        - the "long name" for the parameter
	/// @param desc        - appears in the help
	/// @param disposition - what to set the destination to if the flag appears.
	///                      This is to provide options like --enable-foo and
	///                      --disable-foo
	/// @param dest        - direct reference to the variable
	/// @param setter      - setter function, allows one to bind to class member
	/// @group {
	inline void add_flag( char key, const char *name, const char *desc,
						  bool disposition, bool &dest );
	inline void add_flag( char key, const char *name, const char *desc,
						  bool disposition,
						  const std::function<void (bool)> &setter );
	/// }

	/// @brief adds a flag that causes a counter to be incremented or decremented
	///
	/// Every time this flag is encountered
	/// @group {
	inline void add_flag_counter( char key, const char *name, const char *desc,
								  int &dest );
	inline void add_flag_counter( char key, const char *name, const char *desc,
								  const std::function<void (int)> &setter );
	/// }

	/// @brief Adds a argument.
	///
	/// if @param required is set true, and this argument is not found
	/// at the end of argument parsing, an error will be generated,
	/// usage printed and the parser will return false.
	///
	/// @group {
	/// Direct variable store
	template <typename T>
	inline void add_argument( char key, const char *name, const char *desc,
							  T &dest,
							  const T &defVal,
							  bool required = false );

	/// Function setter interface
	template <typename T>
	inline void add_argument( char key, const char *name, const char *desc,
							  const std::function<void (const T &)> &setter,
							  const T &defVal,
							  bool required = false );
	/// } group

	/// @brief adds an argument list.
	///
	/// Extracts the next N arguments as a list of type T
	/// @group {
	template <typename T>
	inline void add_argument_list(
		char key, const char *name, const char *desc,
		size_t N,
		std::vector<T> &dest,
		bool required = false );

	template <typename T>
	inline void add_argument_list(
		char key, const char *name, const char *desc,
		size_t N,
		const std::function<void (const std::vector<T> &)> &setter,
		bool required = false );
	/// } group

	inline void set_argument_secure( const char *name, bool s );

	/// @brief Allows one to add additional keys for an already defined key
	///
	/// This allows you to provide additional overloads for the same
	/// argument. A sample of this would be to provide -? for help as well
	/// as -h (although this class handles that already)
	inline void add_additional_keys( char key, const char *keys );

	/// @brief Similar to add_additional_keys, but this time for the
	/// "long" name.
	///
	/// This can be useful for providing legacy support for old options
	/// that have been renamed, but can still support.
	/// @param name      The currently defined name
	/// @param altname   The additional alternate name you want to register
	/// @param warnOnUse if true, warns the user that the option is deprecated
	inline void add_additional_name( const char *name, const char *altname, bool warnOnUse );

	/// if this is set to true (yes), arguments starting with - or --
	/// that are not in the known list of options to this class will
	/// be added to the list of extra arguments and later
	/// retrievable. Otherwise, it will fail
	inline void allow_unknown( bool yesno ) { _allow_unknown = yesno; }
	inline bool allow_unknown( void ) const { return _allow_unknown; }

	/// if this is set to true, allows key options to be specified without
	/// a leading dash, in a bundled fashion, like calling "tar xvf -".
	/// NB: like other programs that allow this, it is only allowed
	/// as the starting argument
	inline void allow_bundled_keys( bool yesno ) { _allow_bundled_keys = yesno; }
	inline bool allow_bundled_keys( void ) const { return _allow_bundled_keys; }
	
	/// if this is set to true, flag options change behavior slightly
	/// in that a leading dash (-) will set the flag to false,
	/// a plus (+) will enable the flag
	inline void plus_minus_flags( bool yesno ) { _plus_minus_flags = yesno; }
	inline bool plus_minus_flags( void ) const { return _plus_minus_flags; }

	/// This defaults to true, but when it is enabled, allows
	/// --option=foo syntax instead of just --option foo
	inline void allow_equal( bool yesno ) { _equal_check = yesno; }
	inline bool allow_equal( void ) const { return _equal_check; }

	/// Allows retrieval of any unknown options or extra arguments
	const std::vector<std::string> &get_extra_args( void ) { return _extra_args; }

	/// The main parse routine which processes the arguments
	inline bool parse( int argc, char *argv[] );

	/// Outputs the help string to describe how to pass arguments to
	/// this program.
	inline virtual void print_usage( void );

protected:
	typedef std::shared_ptr<arg_parser_impl::arg_option_store_base> arg_ptr;

	inline arg_ptr fill_and_add( char key, const char *name, const char *desc,
								 const arg_ptr &narg )
	{
		narg->name( name );
		narg->description( desc );

		if ( key != '\0' )
			_key_map[key] = narg;

		_name_map[name] = narg;
		_args.push_back( narg );
		return narg;
	}

	inline bool processOption( char *arg, int &curarg, int argc, char *argv[] );
	inline bool processBundledKeys( char *arg, int &curarg, int argc, char *argv[] );

	/// Allows sub-classes to define their own standard output or
	/// error output streams, which might be useful for special case
	/// programs where re-directing std::cout and std::cerr isn't
	/// appropriate.
	inline virtual std::ostream &get_output_stream( void ) { return std::cout; }
	inline virtual std::ostream &get_error_stream( void ) { return std::cerr; }

	std::vector<arg_ptr> _args;
	std::map<char, arg_ptr> _key_map;
	std::map<std::string, arg_ptr> _name_map;

	std::vector<std::string> _extra_args;

	std::string _program_name;
	std::string _program_version;
	std::string _build_date;
	std::string _argv0;

	bool _allow_unknown = false;
	bool _allow_bundled_keys = false;
	bool _plus_minus_flags = false;
	bool _equal_check = true;
};


////////////////////////////////////////


inline void
arg_parser::add_flag( char key, const char *name, const char *desc,
					  bool disposition, bool &dest )
{
	fill_and_add( key, name, desc, arg_ptr(
					  new arg_parser_impl::arg_option_flag( disposition, dest ) ) );
}


////////////////////////////////////////


inline void
arg_parser::add_flag( char key, const char *name, const char *desc,
					  bool disposition,
					  const std::function<void (bool)> &setter )
{
	fill_and_add( key, name, desc, arg_ptr(
					  new arg_parser_impl::arg_option_flag( disposition, setter ) ) );
}


////////////////////////////////////////


inline void
arg_parser::add_flag_counter( char key, const char *name, const char *desc,
							  int &dest )
{
	using namespace arg_parser_impl;
	fill_and_add( key, name, desc, arg_ptr(
					  new arg_parser_impl::arg_option_counter( dest ) ) );
}


////////////////////////////////////////


inline void
arg_parser::add_flag_counter( char key, const char *name, const char *desc,
							  const std::function<void (int)> &setter )
{
	fill_and_add( key, name, desc, arg_ptr(
					  new arg_parser_impl::arg_option_counter( setter ) ) );
}


////////////////////////////////////////


template <typename T>
inline void
arg_parser::add_argument( char key, const char *name, const char *desc,
						  T &dest,
						  const T &defVal,
						  bool required )
{
	arg_ptr narg = fill_and_add(
		key, name, desc, arg_ptr(
			new arg_parser_impl::arg_option_store<T>( defVal, dest ) ) );
	narg->required( required );
}


////////////////////////////////////////


template <typename T>
inline void
arg_parser::add_argument( char key, const char *name, const char *desc,
						  const std::function<void (const T &)> &setter,
						  const T &defVal,
						  bool required )
{
	arg_ptr narg = fill_and_add(
		key, name, desc, arg_ptr(
			new arg_parser_impl::arg_option_store<T>( defVal, setter ) ) );
	narg->required( required );
}


////////////////////////////////////////


template <typename T>
inline void
arg_parser::add_argument_list(
	char key, const char *name, const char *desc,
	size_t N,
	std::vector<T> &dest,
	bool required )
{
}


////////////////////////////////////////


template <typename T>
inline void
arg_parser::add_argument_list(
	char key, const char *name, const char *desc,
	size_t N,
	const std::function<void (const std::vector<T> &)> &setter,
	bool required )
{
}


////////////////////////////////////////


inline void
arg_parser::set_argument_secure( const char *name, bool s )
{
}


////////////////////////////////////////


inline void
arg_parser::add_additional_keys( char key, const char *keys )
{
	if ( ! keys )
		throw std::runtime_error( "NULL list of keys specified" );

	auto x = _key_map.find( key );
	if ( x == _key_map.end() )
		throw std::runtime_error( "Unknown baseline key" );

	while ( *keys )
		_key_map[*keys++] = x->second;
}


////////////////////////////////////////


inline void
arg_parser::add_additional_name( const char *name, const char *altname, bool warnOnUse )
{
}


////////////////////////////////////////


inline bool
arg_parser::parse( int argc, char *argv[] )
{
	if ( argc < 1 )
		throw std::runtime_error( "Invalid argument count" );
	_argv0 = argv[0];
	_extra_args.clear();

	bool haderr = false;
	try
	{
		bool unknown = false;
		for ( int curarg = 1; curarg < argc; ++curarg )
		{
			char *arg = argv[curarg];
			if ( ! arg )
				throw std::runtime_error( "Invalid argument pointer" );

			if ( curarg == 1 && _allow_bundled_keys )
			{
				if ( processBundledKeys( arg, curarg, argc, argv ) )
					break;
			}

			if ( ! processOption( arg, curarg, argc, argv ) )
			{
				_extra_args.push_back( std::string( arg ) );
			}
		}
	}
	catch ( std::exception &e )
	{
		get_error_stream() << e.what() << std::endl;
		haderr = true;
	}

	bool missing = false;
	for ( auto a = _args.begin(); !haderr && a != _args.end(); ++a )
	{
		if ( (*a)->required() && ! (*a)->seen() )
		{
			get_error_stream() << "Required command line option '" << (*a)->name() << "' not found." << std::endl;
			missing = true;
		}
	}

	if ( haderr || missing )
		return false;

	return true;
}


////////////////////////////////////////


inline void
arg_parser::print_usage( void )
{
}


////////////////////////////////////////


inline bool
arg_parser::processOption( char *arg, int &curarg, int argc, char *argv[] )
{
	bool processOption = false;
	bool plus = false;
	int offset = 0;
	if ( arg[0] == '-' )
	{
		processOption = true;
		++offset;
		if ( arg[offset] == '-' )
			++offset;
	}
#ifdef _WIN32
	// under windows, we want to also support the '/' prefix
	else if ( arg[0] == '/' )
	{
		processOption = true;
		++offset;
	}
#endif
	else if ( _plus_minus_flags && arg[0] == '+' )
	{
		processOption = true;
		plus = true;
		++offset;
	}

	if ( ! processOption )
		return false;

	// Have an option, let's find it by name first. if that fails,
	// we'll look it up by key
	// 
	// First handle the equal check (option=foo)
	std::string procarg = arg + offset;
	std::string val;
	std::string::size_type eqPos = procarg.find_first_of( '=' );
	if ( _equal_check )
	{
		if ( eqPos != std::string::npos )
		{
			val = procarg.substr( eqPos + 1 );
			procarg = procarg.substr( 0, eqPos );
		}
	}

	arg_ptr argStore;
	auto nmi = _name_map.find( procarg );
	if ( nmi != _name_map.end() )
		argStore = nmi->second;
	else if ( procarg.size() == 1 )
	{
		auto kmi = _key_map.find( procarg[0] );
		if ( kmi != _key_map.end() )
			argStore = kmi->second;
	}

	if ( ! argStore )
	{
		if ( _allow_unknown )
		{
			_extra_args.push_back( std::string( argv[curarg] ) );
			return true;
		}
		throw std::runtime_error( "Unknown argument '" + std::string( argv[curarg] ) + "'" );
	}
	
	int tmparg = curarg;
	bool rval = argStore->process( val, eqPos != std::string::npos,
								   _plus_minus_flags, plus,
								   curarg, argc, argv );
	if ( argStore->secure() )
	{
		int clearIdx = tmparg + 1;
		if ( eqPos != std::string::npos )
			clearIdx = tmparg;

		for ( ; clearIdx <= curarg; ++clearIdx )
		{
			char *dst = argv[clearIdx];
			if ( eqPos != std::string::npos )
			{
				dst = dst + offset + eqPos + 1;
				eqPos = std::string::npos;
			}
			while ( dst && *dst != '\0' )
				*dst++ = '*';
		}
	}

	return true;
}


////////////////////////////////////////


inline bool
arg_parser::processBundledKeys( char *arg, int &curarg, int argc, char *argv[] )
{
	return false;
}

} // namespace yaco

// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:

