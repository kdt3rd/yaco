#! /usr/bin/env python
# Copyright (c) 2012 Kimball Thurston
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#
# Filename: bootstrap.py
#
# Author: Kimball Thurston
#
#


import os
import sys

src_root = os.path.dirname( os.path.abspath( sys.argv[0] ) )
sys.path.insert( 0, os.path.join( src_root, 'constructor' ) )
import constructor

#from optparse import OptionParser
#parser = OptionParser( usage="usage: %prog [options]", version="%prog 1.0" )
#
#parser.add_option( '--debug', action='store_true',
#                   help='Compiles library in debug mode',
#                   default=False )
#parser.add_option( '--release', action='store_true',
#                   help='Compiles library in release mode',
#                   default=False )
#parser.add_option( '--build-dir',
#                   help='Specifies output directory used for building (by default <current directory>/<build-type>)' )
#
#(options, args) = parser.parse_args()
#if args:
#    parser.error( "Unknown arguments specified" )
#    pass
#
#if options.debug and options.release:
#    parser.error( "Unable to compile in both debug and release mode" )
#    pass
#build_type = 'build'
#if options.debug:
#    build_type = 'debug'
#    pass
#if options.release:
#    build_type = 'release'
#    pass

if sys.platform == 'win32':
    sys.stdout.write( 'ERROR: Win32 support not yet completed' )
    sys.exit( 1 )
    pass

constructor.ProcessBuildTree()
