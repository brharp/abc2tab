#include <stdio.h>

void
ps_moveto( float x, float y )
{
	printf( "%f %f moveto\n", x, y ) ;
}

void
ps_rmoveto( float x, float y )
{
	printf( "%f %f rmoveto\n", x, y ) ;
}

void
ps_lineto( float x, float y )
{
	printf( "%f %f lineto\n", x, y ) ;
}

void
ps_rlineto( float x, float y )
{
	printf( "%f %f rlineto\n", x, y ) ;
}

void
ps_show( const char *string )
{
	if ( string != NULL && string[0] != '\0' )
		printf( "(%s) show\n", string ) ;
}

void
ps_stroke( )
{
	printf( "stroke\n" ) ;
}

void
ps_newpath( )
{
	printf( "newpath\n" ) ;
}

void
ps_setlinewidth( float w )
{
	printf( "%f setlinewidth\n", w ) ;
}

void
ps_selectfont( const char *name, float size )
{
	printf( "/%s %f selectfont\n", name, size ) ;
}

