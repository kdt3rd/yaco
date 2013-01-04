
EnableModule( "lang_cpp" )
EnableModule( "unittest" )

BuildConfig( "build", "Build", default=True )
BuildConfig( "debug", "Debug" )
BuildConfig( "release", "Release" )

if sys.platform.startswith( 'linux' ):
    sys.stdout.write( "Configuring for linux...\n" )
    generator.DefineGlobal( 'AR', FindExecutable( "ar" ) )
    gpp = FindExecutable( "g++" )
    cpp = FindExecutable( "clang++" )
    if cpp is not None and generator.GetEnvVar( 'CXX' ) is None:
        sys.stdout.write( " Using compiler: %s\n" % cpp )
        generator.DefineGlobal( 'CC', FindExecutable( "clang" ) )
        generator.DefineGlobal( 'CXX', cpp )
        generator.DefineGlobal( 'LD', cpp )
        generator.DefineGlobal( 'CXXFLAGS', [ "-std=c++11", "-stdlib=libc++" ] )
        generator.DefineGlobal( 'WARN', [ "-Weverything", "-Wno-c++98-compat" ] )
        generator.DefineGlobal( 'LDFLAGS', [ "-lc++" ] )
    else:
        sys.stdout.write( " Using compiler: %s\n" % gpp )
        generator.DefineGlobal( 'CC', FindExecutable( "gcc" ) )
        generator.DefineGlobal( 'CXX', gpp )
        generator.DefineGlobal( 'LD', gpp )
        generator.DefineGlobal( 'CXXFLAGS', [ "-std=c++11" ] )
        generator.DefineGlobal( 'WARN', [ "-Wall" ] )

if sys.platform.startswith( 'darwin' ):
    sys.stdout.write( "Configuring for OS/X...\n" )
    generator.DefineGlobal( 'CC', FindExecutable( "clang" ) )
    generator.DefineGlobal( 'CXX', FindExecutable( "clang++" ) )
    generator.DefineGlobal( 'LD', FindExecutable( "clang++" ) )
    generator.DefineGlobal( 'CXXFLAGS', [ "-std=c++11", "-stdlib=libc++" ] )
    generator.DefineGlobal( 'AR', FindExecutable( "ar" ) )
    generator.DefineGlobal( 'WARN', [ "-Weverything", "-Wno-c++98-compat" ] )
    generator.DefineGlobal( 'LDFLAGS', [ "-lc++" ] )

Include( "include" )
