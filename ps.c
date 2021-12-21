#include <stdio.h>

extern FILE *psout ;

void
ps_moveto( float x, float y )
{
	if ( !psout )
		return ;

	fprintf( psout, "%f %f moveto\n", x, y ) ;
}

void
ps_rmoveto( float x, float y )
{
	if ( !psout )
		return ;

	fprintf( psout, "%f %f rmoveto\n", x, y ) ;
}

void
ps_lineto( float x, float y )
{
	if ( !psout )
		return ;

	fprintf( psout, "%f %f lineto\n", x, y ) ;
}

void
ps_rlineto( float x, float y )
{
	if ( !psout )
		return ;

	fprintf( psout, "%f %f rlineto\n", x, y ) ;
}

void
ps_show( const char *string )
{
	if ( !psout )
		return ;

	if ( string != NULL && string[0] != '\0' )
		fprintf( psout, "(%s) show\n", string ) ;
}

void
ps_stroke( )
{
	if ( !psout )
		return ;

	fprintf( psout, "stroke\n" ) ;
}

void
ps_newpath( )
{
	if ( !psout )
		return ;

	fprintf( psout, "newpath\n" ) ;
}

void
ps_setlinewidth( float w )
{
	if ( !psout )
		return ;

	fprintf( psout, "%f setlinewidth\n", w ) ;
}

void
ps_selectfont( const char *name, float size )
{
	if ( !psout )
		return ;

	fprintf( psout, "/%s %f selectfont\n", name, size ) ;
}

