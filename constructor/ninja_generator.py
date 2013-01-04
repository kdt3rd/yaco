
#from . import ninja_syntax
import errno
import string

_generatorFiles = []
_envVars = {}
_envVarIsExe = {}
_globalVars = {}
_rules = []
_defaultTargets = {}
_subdirTargets = {}
_allTargets = []
_extraDirs = []

def _escape( s ):
    assert '\n' not in s, 'Ninja syntax does not allow newlines'
    # Ninja only has one special metacharacter: '$'.
    return s.replace('$', '$$')

def _escape_path( f ):
    return f.replace('$ ','$$ ').replace(' ','$ ').replace(':', '$:')

class _RuleDef:
    """Class representing a rule definition"""
    def __init__( self, rargs ):
        self.name = rargs['tag']
        rulespec = "rule " + self.name + "\n  command ="
        cmd = rargs['cmd']
        if isinstance( cmd, list ):
            for c in cmd:
                rulespec += ' ' + c
        else:
            rulespec += ' ' + cmd
        desc = rargs.get( 'desc' )
        descUse = rargs.get( 'descUseIn' )
        if desc is not None:
            rulespec += "\n  description = " + desc
            if descUse:
                rulespec += " ($in)"
            elif descUse is not None:
                rulespec += " ($out)"
        dep = rargs.get( 'depfile' )
        if dep is not None:
            rulespec += "\n  depfile = " + dep
        rulespec += '\n'
        self.rulestr = rulespec

    def GetTag( self ):
        return self.name

    def write( self, out ):
        out.write( self.rulestr )
        return

class Target(object):
    """Class representing a target"""
    def __init__( self, targs ):
        global _defaultTargets

        t = targs['out']
        self.gen_type = targs['gen_type']
        self.short_name = targs['short']
        self.rule = targs['rule']
        self.deps = targs.get( 'dep' )
        self.impl_deps = targs.get( 'impl_dep' )
        self.over_vars = targs.get( 'vars' )
        if isinstance( t, list ):
            self.outputs = t
        else:
            self.outputs = [ t ]

        self.doPhony = False
        d = targs.get( 'default' )
        if d is not None and d:
            _defaultTargets[self.short_name] = t
            self.doPhony = True

    def GetName( self ):
        return self.short_name

    def GetType( self ):
        return self.gen_type

    def addOutputs( self, out ):
        for o in self.outputs:
            out.write( ' ' )
            out.write( _escape_path( o ) )

    def write( self, out ):
        if self.rule is None:
            return
        out.write( "build" )
        self.addOutputs( out )
        out.write( ': ' )
        out.write( self.rule.name )
        if self.deps is not None:
            for d in self.deps:
                if isinstance( d, Target ):
                    d.addOutputs( out )
                else:
                    out.write( ' ' )
                    out.write( d )
        if self.impl_deps is not None and len(self.impl_deps) > 0:
            out.write( ' |' )
            for d in self.impl_deps:
                if isinstance( d, Target ):
                    d.addOutputs( out )
                else:
                    out.write( ' ' )
                    out.write( d )
        if self.over_vars is not None:
            for k, v in self.over_vars.iteritems():
                out.write( '\n  ' )
                out.write( k )
                out.write( ' = ' )
                out.write( _escape( v ) )
        out.write( '\n' )
        if self.doPhony:
            out.write( "build " )
            out.write( self.short_name )
            out.write( ": phony" )
            self.addOutputs( out )
            out.write( '\n' )

    def Outputs( self ):
        return self.outputs

    pass

def Var( n ):
    return '$' + n

def GetOutputVarName():
    return "$out"

def GetInputListVarName():
    return "$in"

def DefineEnvVar( n, isExe ):
    global _envVars
    global _envVarIsExe
    _envVars[n] = None
    _envVarIsExe[n] = isExe

def GetEnvVar( n ):
    global _envVars
    return _envVars.get( n )
    
def DefineGlobal( v, x ):
    global _globalVars
    _globalVars[v] = x

_subdirVars = {}
def AddToVar( v, x ):
    global _subdirVars
    global _globalVars
    cd = GetCurrentSourceRelDir()
    if len(cd) == 0:
        gl = _globalVars.get( v )
        if gl is None:
            if isinstance( x, list ):
                _globalVars[v] = x.copy()
            else:
                _globalVars[v] = [ x ]
        elif isinstance( gl, list ):
            if isinstance( x, list ):
                gl = gl + x
            else:
                gl.append( x )
        else:
            _globalVars[v] = [ gl, x ]
    else:
        sl = _subdirVars.get( cd )
        if sl is None:
            sl = {}
            sl[v] = [ x ]
            _subdirVars[cd] = sl
        else:
            vl = sl.get( v )
            if vl is None:
                if isinstance( x, list ):
                    sl[v] = x.copy()
                else:
                    sl[v] = [ x ]
            elif isinstance( x, list ):
                vl = vl + x
            else:
                vl.append( x )

def PopulateVarsFromEnv():
    global _envVars
    global _envVarIsExe
    for k, v in _envVars.iteritems():
        t = os.environ.get( k )
        if t is not None:
            if _envVarIsExe[k]:
                _envVars[k] = FindExecutable( t )
            else:
                _envVars[k] = t

def AddRule( **rargs ):
    global _rules
    r = None
    try:
        r = _RuleDef( rargs )
        _rules.append( r )
    except KeyError:
        sys.stderr.write( "ERROR: Missing argument to Rule definition in '%s'\n" % GetCurrentBuildFile() )
        raise
    return r

def AddTarget( **targs ):
    global _subdirTargets
    global _allTargets

    t = None
    try:
        cd = GetCurrentSourceRelDir()
        t = Target( targs )
        st = _subdirTargets.get( cd )
        if st is None:
            _subdirTargets[cd] = [ t ]
        else:
            st.append( t )
        _allTargets.append( t )
    except KeyError:
        sys.stderr.write( "ERROR: Missing argument to AddTarget definition in '%s'\n" % GetCurrentBuildFile() )
        raise
    return t

def FindTarget( name ):
    for t in _allTargets:
        if t.short_name == name:
            return t
        for o in t.outputs:
            if o == name:
                return t
    return None

def GetTargetType( x ):
    return x.GetType()

_phonyTargets = {}
def AddToAliasTarget( name, item ):
    global _phonyTargets
    lt = _phonyTargets.get( name )
    if lt is None:
        _phonyTargets[name] = [ item ]
    else:
        lt.append( item )

def AddConfigureFile( fn ):
    global _generatorFiles
    if os.path.isabs( fn ):
        _generatorFiles.append( fn )
    else:
        _generatorFiles.append( os.path.join( GetCurrentSourceDir(), fn ) )

def AddOutputDir( d ):
    global _extraDirs
    n = os.path.join( GetBuildRootDir(), d )
    if n in _extraDirs:
        return
    _extraDirs.append( n )

def GetOutputDirs():
    dirs = []
    for k, v in _subdirTargets.iteritems():
        dirs.append( os.path.join( GetBuildRootDir(), k ) )
    return dirs + _extraDirs

def WriteBuildFiles( root, build_dir, build_config ):
    builddirs = GetOutputDirs()
    for d in builddirs:
        try:
            os.makedirs( d )
        except OSError as e:
            if e.errno == errno.EEXIST:
                pass
            else:
                raise

    filename = os.path.join( root, "build.ninja" )
    out = open( filename, 'wb' )

    for r in _rules:
        out.write( '\n' )
        out.write( r.rulestr )

    for g, v in _globalVars.iteritems():
        out.write( "\n" )
        out.write( _escape( g ) )
        out.write( " =" )
        if isinstance( v, list ):
            out.write( string.join( v ) )
        else:
            out.write( ' ' )
            out.write( v )

    for g, v in _envVars.iteritems():
        if v is not None:
            out.write( "\n" )
            out.write( _escape( g ) )
            out.write( " =" )
            if isinstance( v, list ):
                out.write( string.join( v ) )
            else:
                out.write( ' ' )
                out.write( v )

    out.write( '\n' )
    for k, v in _subdirTargets.iteritems():
        subfile = os.path.join( root, k, "build.ninja" )
        subout = open( subfile, 'wb' )
        sdvar = _subdirVars.get( k )
        if sdvar is not None:
            for v, x in sdvar.iteritems():
                out.write( v )
                out.write( ' = ' )
                out.write( string.join( x ) )
                out.write( '\n' )

        for t in v:
            t.write( subout )
        out.write( '\nsubninja ' )
        out.write( subfile )

    if len(_phonyTargets) > 0:
        out.write( '\n' )
        for k, l in _phonyTargets.iteritems():
            out.write( '\nbuild ' )
            out.write( k )
            out.write( ': phony' )
            for t in l:
                t.addOutputs( out )

    if len(_defaultTargets) > 0:
        out.write( "\ndefault" )
        for k, v in _defaultTargets.iteritems():
            out.write( ' ' )
            out.write( k )

    out.write( "\n\nrule regen_config\n" )
    out.write( "  command =" )
    for g, v in _envVars.iteritems():
        if v is not None:
            out.write( ' ' )
            out.write( g )
            out.write( '=' )
            out.write( _escape( v ) )

    if sys.platform == 'win32':
        raise NotImplementedError

    # python swallows real arg0 invocation of py script
    realArg0 = os.path.abspath( os.environ['_'] )
    ourArg0 = os.path.abspath( sys.argv[0] )
    if realArg0 != ourArg0:
        out.write( ' ' )
        out.write( realArg0 )
    out.write( ' ' )
    out.write( ourArg0 )
    out.write( ' --build=' )
    out.write( build_config )
    out.write( ' --build-dir=' )
    out.write( build_dir )
    out.write( ' --generator=ninja' )

    out.write( '\n' )
    out.write( '  description = Regenerating build\n' )
    out.write( '  generator = 1\n' )
    out.write( '\nbuild build.ninja: regen_config |' )
    for g in _generatorFiles:
        out.write( ' ' )
        out.write( _escape_path( g ) )
    out.write( '\n' )
    out.write( '\n' )
