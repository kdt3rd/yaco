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

#include "impl/log_priv.h"

namespace yaco
{

/// @brief name-based log system
///
/// The idea here is to have a syslog-like functionality but with a
/// global log as well as "local" objects (i.e. put in a private
/// namespace in a local source file) that are named, and the specific
/// names can be turned on / off at run time via some application
/// controlled mechanism (config file, UI menu, etc.)

enum class log_type
{
	ERROR,
	INFO,
	DEBUG
};

namespace __priv { std::string get_log_prefix( log_type level ); }

/// @brief Sets an additional file to log output
void set_log_file( const std::string &filename );

/// @brief Changes the log level to be no higher than specified level
///
/// for example, setting the level to ERROR will display only logs
/// that are specified at the ERROR level, so INFO and DEBUG will
/// not appear
void set_log_level( log_type max_level );
bool will_log( log_type level );

/// @brief Starts log services on a global level
void log_start( bool daemon, bool logonly );
/// @brief Stops log services on a global level
void log_stop( void );

template< typename... Args>
void
log( log_type level, const char *fmt, const Args&... args )
{
	if ( will_log( level ) )
	{
		std::stringstream s;
		s << __priv::get_log_prefix( level );
		__priv::fmt_build( s, fmt, std::tie(args...) );
		__priv::log_output( s.str() );
	}
}

template< typename... Args>
void log( log_type level, const std::string &fmt, const Args&... args )
{
	log( level, fmt.c_str(), args... );
}

class logger
{
public:
	logger( const char *name );
	~logger( void );

	template< typename... Args>
	void
	log( log_type level, const char *fmt, const Args&... args )
	{
		if ( enabled( level ) )
		{
			std::stringstream s;
			s << __priv::get_log_prefix( level );
			__priv::log_build( s, fmt, std::tie(args...) );
			__priv::log_output( s.str() );
		}
	}

	template< typename... Args>
	void log( log_type level, const std::string &fmt, const Args&... args )
	{
		log( level, fmt.c_str(), args... );
	}

private:
	bool enabled( log_type level );
	std::string get_log_prefix( log_type level );

	std::string myName;
};

} // namespace yaco


////////////////////////////////////////
// Local Variables:
// mode: C++
// End:
// vim:ft=cpp:
