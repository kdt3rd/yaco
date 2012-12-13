
from generators import ninja

SetGenerator( ninja.Ninja() )

BuildConfig( "build", "Build", default=True )
BuildConfig( "debug", "Debug" )
BuildConfig( "release", "Release" )

#from modules import cpp
#cpp.SetCompiler( "clang" )

SubDir( "src" )
