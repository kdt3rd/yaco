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
# Filename: lang_cpp_module.py
#
# Author: Kimball Thurston
#
#

_object_file_ext = '.o'
_exe_ext = ''
_static_lib_ext = '.a'
_shared_lib_ext = '.so'

_inVar = generator.GetInputListVarName()
_outVar = generator.GetOutputVarName()

_cxxLine = [ generator.Var( "CXX" ), generator.Var( "CFLAGS" ), 
             generator.Var( "CXXFLAGS" ), generator.Var( "DEFS" ),
             generator.Var( "WARN" ), generator.Var( "INC" ),
             generator.Var( "SYSINC" ),
             '-MMD', '-MF', _outVar + '.d',
             '-o', _outVar,
             '-c', _inVar ]
_statLibLine = [ 'rm', '-f', _outVar, ';',
                 generator.Var( "AR" ), generator.Var( "ARFLAGS" ), "cur",
                 _outVar, _inVar ]
_cxxLine = [ generator.Var( "CXX" ), generator.Var( "CFLAGS" ), 
             generator.Var( "CXXFLAGS" ), generator.Var( "DEFS" ),
             generator.Var( "WARN" ), generator.Var( "INC" ),
             generator.Var( "SYSINC" ),
             '-MMD', '-MF', _outVar + '.d',
             '-o', _outVar,
             '-c', _inVar ]
_exeLine = [ generator.Var( "LD" ), generator.Var( "RPATH" ),
             generator.Var( "LDFLAGS" ), _inVar,
             '-o', _outVar ]

if sys.platform == 'win32':
    _object_file_ext = '.obj'
    _exe_ext = '.exe'
    _static_lib_ext = '.lib'
    _shared_lib_ext = '.dll'
elif sys.platform.startswith( 'darwin' ):
    _shared_lib_ext = '.dylib'
elif not sys.platform.startswith( 'linux' ):
    raise RuntimeError( "Unhandled platform" )

generator.DefineEnvVar( "CC", True )
generator.DefineEnvVar( "CXX", True )
generator.DefineEnvVar( "CFLAGS", False )
generator.DefineEnvVar( "CXXFLAGS", False )
generator.DefineEnvVar( "DEFS", False )
generator.DefineEnvVar( "WARN", False )
generator.DefineEnvVar( "INC", False )
generator.DefineEnvVar( "SYSINC", False )
generator.DefineEnvVar( "AR", True )
generator.DefineEnvVar( "ARFLAGS", False )
generator.DefineEnvVar( "LD", True )
generator.DefineEnvVar( "LDFLAGS", False )

_cppRule = generator.AddRule( tag='cpp', cmd=_cxxLine, desc="CXX", depfile=(_outVar + ".d"), descUseIn=True )
_staticLibRule = generator.AddRule( tag='lib', cmd=_statLibLine, desc="AR", descUseIn=False )
_exeRule = generator.AddRule( tag='exe', cmd=_exeLine, desc="LINK", descUseIn=False )

def CFlags( *args ):
    return

def CXXFlags( *args ):
    return

def Libs( *args ):
    return

def SysLibs( *args ):
    return

def Include( *args ):
    for a in args:
        if os.path.isabs( a ):
            generator.AddToVar( "INC", "-I" + a )
        else:
            generator.AddToVar( "INC", "-I" + os.path.join( GetCurrentSourceDir(), a ) )
    return

def _handleC( f, base, ext ):
    if os.path.isabs( f ):
        fullPath = f
    else:
        fullPath = os.path.join( GetCurrentSourceDir(), f )
    objPath = os.path.join( GetCurrentBinaryDir(), base + _object_file_ext )
    shortTarget = os.path.join( GetCurrentSourceRelDir(), base + _object_file_ext )
    t = generator.AddTarget( out=objPath, gen_type='object', short=shortTarget, dep=[fullPath], rule=_cppRule )
    return [ t ]

def _handleObject( f, base, ext ):
    t = generator.FindTarget( f )
    if t is not None:
        return [ t ]

    if os.path.isabs( f ):
        fullPath = f
    else:
        fullPath = os.path.join( GetCurrentSourceDir(), f )
        
    t = generator.AddTarget( out=fullPath, gen_type='object', short=f, rule=None )
    return [ t ]

_extDispatch = {}
_extDispatch['.c'] = _handleC
_extDispatch['.cpp'] = _handleC
_extDispatch[_object_file_ext] = _handleObject

def _handleName( f ):
    (base, ext) = os.path.splitext( f )
    if len(ext) == 0:
        return generator.FindTarget ( f )
    elif _extDispatch.get( ext ) is not None:
        return _extDispatch[ext]( f, base, ext )
    else:
        raise NameError( "Unhandled name attempting to compile object '%s'" %f )

_built_libs = {}

def _createLibDependencies( *args ):
    explicit = []
    shared_only = []
    for f in args:
        try:
            gt = generator.GetTargetType( f )
            if gt == "object":
                explicit.append( f )
            elif gt == "lib":
                shared_only.append( f )
        except AttributeError:
            if isinstance( f, (str, basestring) ):
                tmpl = _built_libs.get( f )
                if tmpl is not None:
                    shared_only.append( tmpl )
                else:
                    x = _handleName( f )
                    e, i = _createLibDependencies( *x )
                    explicit = explicit + e
                    shared_only = shared_only + i
            elif isinstance( f, list ):
                e, i = _createLibDependencies( *f )
                explicit = explicit + e
                shared_only = shared_only + i
            elif isinstance( f, tuple ):
                for x in f:
                    e, i = _createLibDependencies( x )
                    explicit = explicit + e
                    shared_only = shared_only + i
            else:
                raise
    return explicit, shared_only

def Library( *args ):
    global _built_libs

    name = args[0]

    out = 'lib' + name + _static_lib_ext
    generator.AddOutputDir( 'lib' )
    outPath = os.path.join( GetBuildRootDir(), 'lib', out )
    exp, shared = _createLibDependencies( args[1:] )
    l = generator.AddTarget( out=outPath, gen_type='lib', short=out, dep=exp,
                             impl_dep=shared, default=True, rule=_staticLibRule )
    _built_libs[name] = l
    return l

def Executable( *args ):
    name = args[0]
    out = name + _exe_ext
    generator.AddOutputDir( 'bin' )
    outPath = os.path.join( GetBuildRootDir(), 'bin', out )
    exp, shared = _createLibDependencies( args[1:] )
    exp = exp + shared
    e = generator.AddTarget( out=outPath, gen_type='exe', short=out, dep=exp,
                             default=True, rule=_exeRule )
    return e

def OptExecutable( *args ):
    name = args[0]
    out = name + _exe_ext
    generator.AddOutputDir( 'bin' )
    outPath = os.path.join( GetBuildRootDir(), 'bin', out )
    exp, shared = _createLibDependencies( args[1:] )
    exp = exp + shared
    e = generator.AddTarget( out=outPath, gen_type='exe', short=out, dep=exp,
                             default=False, rule=_exeRule )
    return e

def Compile( *args ):
    ret = []
    for f in args:
        if isinstance( f, generator.Target ):
            ret.append( f )
        if isinstance( f, (str, basestring) ):
            ret = ret + _handleName( f )
        elif isinstance( f, list ):
            ret = ret + Compile( *f )
        elif isinstance( f, tuple ):
            for x in f:
                ret = ret + Compile( x )
    return ret


