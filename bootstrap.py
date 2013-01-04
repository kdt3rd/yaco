#! /usr/bin/env python2.7
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
import errno
import string

from optparse import OptionParser

_build = None
_bootstrap = None
x = 0

while x < len(sys.argv):
    if string.find( sys.argv[x], '=' ) >= 0:
        continue

    fp = os.path.abspath( sys.argv[x] )
    bn = os.path.basename( fp )
    if bn == "env" or bn.startswith( 'python' ):
        continue
    _bootstrap = fp
    break

_source_root_dir = os.path.dirname( _bootstrap )
_build_root_dir = os.getcwd()
_build_start_dir = None

generator = 'ninja'

#sys.path.insert( 0, os.path.join( _source_root_dir, 'constructor' ) )

if sys.platform == 'win32':
    raise NotImplementedError

_build = None
_build_set = {}
_build_dirs = []

_cur_src_dir = None
_cur_rel_src_dir = None
_cur_bin_dir = None
_cur_build_file = None

# In theory we should worry about the file encoding in python3,
# but...
def _source_file( filename, globs = None, locs = None ):
    global _cur_build_file
    if globs is None:
        globs = globals()
    if locs is None:
        locs = locals()

    fn = os.path.join( _cur_src_dir, filename )
    _cur_build_file = fn
    with open( fn, "r") as fh:
        exec( compile( fh.read() + "\n", filename, 'exec' ), globs, locs )
    _cur_build_file = None

def BuildConfig( tag, outloc, default = False ):
    global _build_set
    global _build
    global _cur_bin_dir
    global _build_root_dir
    global _build_dirs
    global _build_start_dir
    _build_set[tag] = outloc
    if default and _build is None:
        _build = tag
    if tag == _build and _cur_bin_dir is None:
        _build_start_dir = _build_root_dir
        _build_root_dir = os.path.join( _build_root_dir, outloc )
        _build_dirs.append( _build_root_dir )
        _cur_bin_dir = os.path.join( _build_root_dir, _cur_rel_src_dir )

def Building( b ):
    if _build == b:
        return True
    return False

def SetBuildRoot( root ):
    global _build_root_dir
    _build_root_dir = root

def SetSourceRoot( root ):
    global _source_root_dir
    _source_root_dir = root

def GetCurrentSourceDir():
    return _cur_src_dir

def GetCurrentSourceRelDir():
    return _cur_rel_src_dir

def GetCurrentBuildFile():
    return _cur_build_file

def GetBuildRootDir():
    return _build_root_dir

def GetCurrentBinaryDir():
    if _build is None:
        raise RuntimeError( "No build specified, binary directory is undefined until then" )
    return _cur_bin_dir

def FindExecutable( exe ):
    if exe is None:
        return None

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

def ProcessBuildTree():
    global _cur_src_dir
    global _cur_rel_src_dir
    global _cur_bin_dir
    global _source_root_dir
    global _build_root_dir
    _cur_src_dir = _source_root_dir
    _cur_rel_src_dir = ""
    _source_file( 'config.py' )
    generator.AddConfigureFile( 'config.py' )
    _source_file( 'build.py' )
    generator.AddConfigureFile( 'build.py' )
    _writeBuildFiles()

class Generator(dict):
    def __init__( self, d ):
        for k, v in d.iteritems():
            self[k] = v
    def __getattr__( self, attr ):
        return self[attr]
    def __setattr__( self, attr, value ):
        self[attr] = value
    def update( d ):
        for k, v in d.iteritems():
            self[k] = v

def SetGenerator( name ):
    global _cur_src_dir
    global _source_root_dir
    global generator

    curd = _cur_src_dir
    _cur_src_dir = os.path.join( _source_root_dir, 'constructor' )

    namespace = globals().copy()
    _source_file( name + '_generator.py', namespace, namespace )
    all_names = namespace.get("__all__")
    new_globs = {}
    if all_names is None:
        for key, val in namespace.iteritems():
            if key[0] != '_' and globals().get( key ) is None:
                new_globs[key] = val
    else:
        for key in all_names:
            new_globs[key] = namespace[key]

    generator = Generator( new_globs )
    generator.AddConfigureFile( name + '_generator.py' )
    _cur_src_dir = curd
    generator.AddConfigureFile( _bootstrap )
    return

_active_module_list = []
def EnableModule( name ):
    global _source_root_dir
    global _cur_src_dir
    global _active_module_list

    if name in _active_module_list:
        return
    _active_module_list.append( name )

    curd = _cur_src_dir
    _cur_src_dir = os.path.join( _source_root_dir, 'constructor' )

    namespace = globals().copy()

    _source_file( name + '_module.py', namespace, namespace )
    generator.AddConfigureFile( name + '_module.py' )

    all_names = namespace.get("__all__")
    new_globs = {}
    if all_names is None:
        for key, val in namespace.iteritems():
            if key[0] != '_':
                new_globs[key] = val
    else:
        for key in all_names:
            new_globs[key] = namespace[key]

    globals().update( new_globs )

    _cur_src_dir = curd
    return

def SubDir( name ):
    global _cur_src_dir
    global _cur_rel_src_dir
    global _cur_bin_dir
    global _source_root_dir
    global _build_root_dir
    global _build_dirs

    curd = _cur_rel_src_dir
    newd = name
    if len(curd) > 0:
        newd = os.path.join( curd, name )
    _cur_rel_src_dir = newd
    _cur_src_dir = os.path.join( _source_root_dir, newd )
    _cur_bin_dir = os.path.join( _build_root_dir, newd )


    _source_file( 'build.py' )
    generator.AddConfigureFile( 'build.py' )

    _cur_rel_src_dir = curd
    if len(curd) == 0:
        _cur_src_dir = _source_root_dir
        _cur_bin_dir = _build_root_dir
    else:
        _cur_src_dir = os.path.join( _source_root_dir, curd )
        _cur_bin_dir = os.path.join( _build_root_dir, curd )

def _writeBuildFiles():
    try:
        os.makedirs( _build_root_dir )
    except OSError as e:
        if e.errno == errno.EEXIST:
            pass
        else:
            raise

    generator.WriteBuildFiles( _build_root_dir, _build_start_dir, _build )
    return

if __name__ == "__main__":
    parser = OptionParser( usage="usage: %prog [options]", version="%prog 1.0" )

    parser.add_option( '--build',
                       help='Allows one to specify build type',
                       type="string",
                       default=None )
    parser.add_option( '--build-dir',
                       type="string",
                       help='Specifies output directory used for building (by default <current directory>/<build-type>)' )
    parser.add_option( '--generator',
                       type="string",
                       help='Allows one to specify the generator type',
                       default=generator )

    (options, args) = parser.parse_args()
    if args:
        parser.error( "Unknown arguments specified" )

    if options.build is not None and len(options.build) > 0:
        _build = options.build

    if options.build_dir is not None and len(options.build_dir) > 0:
        _build_root_dir = os.path.abspath( options.build_dir )

    SetGenerator( options.generator )

    ProcessBuildTree()
