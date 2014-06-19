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

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>

namespace yaco
{

namespace arg_parser_impl
{


////////////////////////////////////////


class arg_option_store_base
{
public:
	arg_option_store_base( void ) {}
	virtual ~arg_option_store_base( void ) {}

	inline void name( const std::string &n ) { _name = n; }
	inline const std::string &name( void ) const { return _name; }

	inline void description( const std::string &d ) { _desc = d; }
	inline const std::string &description( void ) const { return _desc; }

	inline void required( bool req ) { _required = req; }
	inline bool required( void ) const { return _required; }

	inline void add_alt_key( char key, bool warnOnUse ) { _alt_keys.push_back( std::make_pair( key, warnOnUse ) ); }
	inline void add_alt_name( const std::string &name, bool warnOnUse ) { _alt_names.push_back( std::make_pair( name, warnOnUse ) ); }

	inline void secure( bool s ) { _secure = s; }
	inline bool secure( void ) const { return _secure; }

	inline bool seen( void ) const { return _seen; }

	inline bool process( const std::string &earlyValue, bool hadEqual,
						 bool overrideFlag, bool isPlus,
						 int &curarg, int argc, char *argv[] )
	{
		_seen = true;
		return store_item( earlyValue, hadEqual, overrideFlag, isPlus, curarg, argc, argv );
	}

	virtual int expected_arg_count( void ) const = 0;
	virtual std::string default_string( void ) const = 0;

protected:
	inline void throw_invalid_argument_index( const std::string &val )
	{
		std::stringstream msg;
		msg << "Option '" << this->name() << "': Unable to parse argument '" << val << "'";
		throw std::runtime_error( msg.str() );
	}

	inline void throw_invalid_argument_index( int curarg )
	{
		std::stringstream msg;
		msg << "Option '" << this->name() << "': Invalid argument index " << curarg << " (NULL string)";
		throw std::runtime_error( msg.str() );
	}

	inline void throw_missing_option( const std::string &extra = std::string() )
	{
		std::stringstream msg;
		msg << "Option '" << this->name() << "': missing argument" << extra;
		throw std::runtime_error( msg.str() );
	}

	std::string extractArgument( const std::string &earlyValue,
								 bool hadEqual, bool allowEmpty,
								 int &curarg, int argc, char *argv[] )
	{
		if ( hadEqual )
		{
			if ( earlyValue.empty() )
			{
				if ( allowEmpty )
					return std::string();

				throw_missing_option( " empty values not allowed" );
			}

			return earlyValue;
		}

		++curarg;
		if ( curarg >= argc )
			throw_missing_option( " end of argument list reached" );

		if ( ! argv[curarg] )
			throw_invalid_argument_index( curarg );

		if ( ! allowEmpty && *(argv[curarg]) == '\0' )
			throw_missing_option( " empty values not allowed" );

		return std::string( argv[curarg] );
	}

	virtual bool store_item( const std::string &earlyValue, bool hadEqual,
							 bool overrideFlag, bool isPlus,
							 int &curarg, int argc, char *argv[] ) = 0;

	std::string _name;
	std::string _desc;
	bool _required = false;
	bool _secure = false;
	bool _seen = false;

	std::vector<std::pair<char, bool>> _alt_keys;
	std::vector<std::pair<std::string, bool>> _alt_names;
};		


////////////////////////////////////////


class arg_option_flag : public arg_option_store_base
{
public:
	arg_option_flag( bool disp, bool &dest )
			: _disposition( disp ),
			  _func( [&dest]( bool v ) { dest = v; } )
	{}
	arg_option_flag( bool disp, const std::function<void (bool)> &f )
			: _disposition( disp ), _func( f )
	{}
	virtual ~arg_option_flag( void ) {}

	virtual std::string default_string( void ) const
	{
		return std::string( _disposition ? "false" : "true" );
	}

	virtual int expected_arg_count( void ) const { return 0; }

protected:
	virtual bool store_item( const std::string &earlyValue, bool hadEqual,
							 bool overrideFlag, bool isPlus,
							 int &curarg, int argc, char *argv[] )
	{
		if ( overrideFlag )
			_func( isPlus );
		else
			_func( _disposition );
		return true;
	}

private:
	bool _disposition;
	std::function<void (bool)> _func;
};


////////////////////////////////////////


class arg_option_counter : public arg_option_store_base
{
public:
	arg_option_counter( int &dest )
			: _func( [&dest]( int v ) { dest += v; } )
	{}
	arg_option_counter( const std::function<void (int)> &f )
			: _func( f )
	{}
	virtual ~arg_option_counter( void ) {}

	virtual std::string default_string( void ) const
	{
		return std::string( "0" );
	}

	virtual int expected_arg_count( void ) const { return 0; }

protected:
	virtual bool store_item( const std::string &earlyValue, bool hadEqual,
							 bool overrideFlag, bool isPlus,
							 int &curarg, int argc, char *argv[] )
	{
		if ( overrideFlag )
			_func( isPlus ? 1 : -1 );
		else
			_func( 1 );
		return true;
	}

private:
	std::function<void (int)> _func;
};


////////////////////////////////////////


template <typename T>
class arg_option_store : public arg_option_store_base
{
public:
	inline arg_option_store( const T &defVal, T &dest )
			: _default( defVal ),
			  _func( [&dest]( const T &x ) { dest = x; } )
	{}
	inline arg_option_store( const T &defVal, const std::function<void (const T &)> &s )
			: _default( defVal ), _func( s )
	{}

	virtual std::string default_string( void ) const
	{
		std::ostringstream out;
		out << _default;
		return out.str();
	}

	virtual int expected_arg_count( void ) const { return 1; }

protected:
	virtual bool store_item( const std::string &earlyValue, bool hadEqual,
							 bool overrideFlag, bool isPlus,
							 int &curarg, int argc, char *argv[] )
	{
		std::string sval = extractArgument( earlyValue, hadEqual, false, curarg, argc, argv );
		std::istringstream in( sval );

		T val;
		in >> val;
		if ( ! in )
			throw_invalid_argument_index( sval );

		_func( val );
		return true;
	}

private:
	T _default;
	std::function<void (bool)> _func;
};

template <>
class arg_option_store<bool> : public arg_option_store_base
{
public:
	inline arg_option_store( bool defVal, bool &dest )
			: _default( defVal ), _func( [&dest]( const bool &x ) { dest = x; } )
	{}
	inline arg_option_store( bool defVal, const std::function<void (const bool &)> &s )
			: _default( defVal ), _func( s )
	{}

	virtual std::string default_string( void ) const
	{
		return std::string( _default ? "true" : "false" );
	}

	virtual int expected_arg_count( void ) const { return 1; }

protected:
	virtual bool store_item( const std::string &earlyValue, bool hadEqual,
							 bool overrideFlag, bool isPlus,
							 int &curarg, int argc, char *argv[] )
	{
		std::string sval = extractArgument( earlyValue, hadEqual, false, curarg, argc, argv );

		char lc = tolower( sval[0] );
		switch ( lc )
		{
			case 't':
			case 'y':
			case '1':
				_func( true );
				break;
			default:
				_func( false );
				break;
		}

		return true;
	}

private:
	bool _default;
	std::function<void (const bool &)> _func;
};


////////////////////////////////////////


template <>
class arg_option_store<std::string> : public arg_option_store_base
{
public:
	inline arg_option_store( const std::string &defVal, std::string &dest )
			: _default( defVal ), _func( [&dest]( const std::string &x ) { dest = x; } )
	{}
	inline arg_option_store( const std::string &defVal, const std::function<void (const std::string &)> &s )
			: _default( defVal ), _func( s )
	{}

	virtual std::string default_string( void ) const
	{
		return _default;
	}

	virtual int expected_arg_count( void ) const { return 1; }

protected:
	virtual bool store_item( const std::string &earlyValue, bool hadEqual,
							 bool overrideFlag, bool isPlus,
							 int &curarg, int argc, char *argv[] )
	{
		_func( extractArgument( earlyValue, hadEqual, true, curarg, argc, argv ) );
		return true;
	}

private:
	std::string _default;
	std::function<void (const std::string &)> _func;
};

} // namespace arg_parser_impl

} // namespace yaco

// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:

