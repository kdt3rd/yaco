
EnableModule( "lang_cpp" )

BuildConfig( "build", "Build", default=True )
BuildConfig( "debug", "Debug" )
BuildConfig( "release", "Release" )

if sys.platform.startswith( 'linux' ):
    sys.stdout.write( "Configuring for linux...\n" )

if sys.platform.startswith( 'darwin' ):
    sys.stdout.write( "Configuring for OS/X...\n" )
    generator.DefineGlobal( 'CC', FindExecutable( "clang" ) )
    generator.DefineGlobal( 'CXX', FindExecutable( "clang++" ) )
    generator.DefineGlobal( 'LD', FindExecutable( "clang++" ) )
    generator.DefineGlobal( 'CXXFLAGS', [ "-std=c++11" ] )
    generator.DefineGlobal( 'AR', FindExecutable( "ar" ) )
    generator.DefineGlobal( 'WARN', [ "-Weverything" ] )
