#
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
# Filename: constructor.py
#
# Author: Kimball Thurston
#
#

import os
import sys

__source_root_dir = os.path.dirname( os.path.abspath( sys.argv[0] ) )
__build_root_dir = os.getcwd()
__rules = []
__targets = {}
__opt_targets = None
__generator = None
__configuration = None
__variables = {}
__build = None
__build_set = {}

if len(sys.argv) > 1:
    __build = sys.argv[1]

CURRENT_SOURCE_DIR = None
CURRENT_REL_SOURCE_DIR = None
CURRENT_BINARY_DIR = None

def BuildConfig( tag, outloc, default = False ):
    global __build_set
    global __build

    __build_set[tag] = outloc
    if default and __build is None:
        __build = tag

def SetBuildType( b ):
    global __build
    __build = b

def Building( b ):
    if __build == b:
        return True
    return False

def SetBuildRoot( root ):
    global __build_root_dir
    __build_root_dir = root

def SetSourceRoot( root ):
    global __source_root_dir
    _source_root_dir = root

def SetGenerator( g ):
    global __generator
    __generator = g

def DefineGlobal( name, value ):
    global __variables
    __variables[name] = value

def AddMainTarget( t ):
    global __targets
    __targets[t.name] = t

def AddOptionalTarget( t ):
    global __opt_targets
    if __opt_targets is None:
        __opt_targets = dict( ( t.name, t ) )

def SetBuildConfiguration( c ):
    return

def FindExecutable( exe ):
    is_exe = lambda fpath: os.path.isfile( fpath ) and os.access( fpath, os.X_OK )

    fpath, fname = os.path.split( exe )
    if fpath:
        if is_exe( exe ):
            return exe
    else:
        for path in os.environ["PATH"].split( os.pathsep ):
            path = path.strip( '"' )
            exe_file = os.path.join( path, exe )
            if is_exe( exe_file ):
                return exe_file
    return None

def FindExternalLibrary( libname, version=None ):
    return None

def ProcessBuildTree():
    global CURRENT_SOURCE_DIR
    global CURRENT_REL_SOURCE_DIR
    global CURRENT_BINARY_DIR
    global __source_root_dir
    global __build_root_dir
    CURRENT_SOURCE_DIR = __source_root_dir
    CURRENT_REL_SOURCE_DIR = ""
    CURRENT_BINARY_DIR = __build_root_dir
    execfile( os.path.join( CURRENT_SOURCE_DIR, 'build.py' ) )
    __writeBuildFiles()

def EnableModule( name ):
    return

def SubDir( name ):
    global CURRENT_SOURCE_DIR
    global CURRENT_REL_SOURCE_DIR
    global CURRENT_BINARY_DIR
    global __source_root_dir
    global __build_root_dir

    curd = CURRENT_REL_SOURCE_DIR
    newd = name
    if len(curd) > 0:
        newd = os.path.join( curd, name )
    CURRENT_REL_SOURCE_DIR = newd
    CURRENT_SOURCE_DIR = os.path.join( __source_root_dir, newd )
    CURRENT_BINARY_DIR = os.path.join( __build_root_dir, newd )
    execfile( os.path.join( CURRENT_SOURCE_DIR, 'build.py' ) )

    CURRENT_REL_SOURCE_DIR = curd
    if len(curd) == 0:
        CURRENT_SOURCE_DIR = __source_root_dir
        CURRENT_BINARY_DIR = __build_root_dir
    else:
        CURRENT_SOURCE_DIR = os.path.join( __source_root_dir, curd )
        CURRENT_BINARY_DIR = os.path.join( __build_root_dir, curd )

def __writeBuildFiles():
    return
