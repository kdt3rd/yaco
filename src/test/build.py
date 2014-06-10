
#UnitTestExe( "varFuncDisp.cpp", "yaco" )
#UnitTestExe( "strSplit.cpp", "yaco" )
#UnitTestExe( "strFormat.cpp", "yaco" )
#UnitTestExe( "region.cpp", "yaco" )
#UnitTestExe( "scopeGuard.cpp", "yaco" )

Executable( 'unit_str_format', Compile( 'strFormat.cpp' ), YACO )
Executable( 'unit_arg_parser', Compile( 'argParser.cpp' ), YACO )

