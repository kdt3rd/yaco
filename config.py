
BuildConfig( "build", "Build", default=True )
BuildConfig( "debug", "Debug" )
BuildConfig( "release", "Release" )

EnableModule( "cc" )
EnableModule( "unit_test" )

UseCXX11()

