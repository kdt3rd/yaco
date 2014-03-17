
YACO = Library( 'yaco', Compile( 'yaco.cpp', 'region.cpp' ) )

#SubDir( 'test' )
Executable( 'unit_str_format', Compile( 'test/strFormat.cpp' ), YACO )

