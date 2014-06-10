
YACO = Library( 'yaco', Compile( 'yaco.cpp', 'region.cpp' ) )

#SubDir( 'test' )
Executable( 'unit_str_format', Compile( 'test/strFormat.cpp' ) )
Executable( 'unit_arg_parse', Compile( 'test/argParser.cpp' ) )

