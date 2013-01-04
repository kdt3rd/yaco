
EnableModule( "lang_cpp" )

_testExeStatus = [ generator.GetInputListVarName(), '>', generator.GetOutputVarName() ]

_testExeRule = generator.AddRule( tag='run_test', cmd=_testExeStatus, desc="TESTING", descUseIn=True )

def UnitTestExe( *args ):
    name, ext = os.path.splitext( args[0] )
    exename = name
    if len(ext) == 0:
        ut = OptExecutable( args )
    else:
        exename = 'test_' + name
        ut = OptExecutable( exename, Compile( args[0] ), args[1:] )

    cd = GetCurrentSourceRelDir()
    p = string.replace( cd, '/', '_' )
    p = string.replace( p, '\\', '_' )
    generator.AddOutputDir( 'test_output' )
    outPath = os.path.join( GetBuildRootDir(), 'test_output', p + '_' + name )
    t = generator.AddTarget( out=outPath, gen_type='test', short=name, dep=[ ut ],
                             rule=_testExeRule )
    generator.AddToAliasTarget( 'test/' + name, ut )
    generator.AddToAliasTarget( 'test', t )
