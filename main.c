#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#define	BUFSIZE	1024
#define SHARP   '^'
#define FLAT    '_'
#define NATURAL '='

enum accidental { sharp = SHARP, natural = NATURAL, flat = FLAT };

enum pitch
{
	C3 = 48,
	D3 = 50,
	E3 = 52,
	F3 = 53,
	G3 = 55,
	A3 = 57,
	B3 = 59,
	C4 = 60,
	D4 = 62,
	E4 = 64,
	F4 = 65,
	G4 = 67,
	A4 = 69,
	B4 = 71
};

//	Standard G tuning
int	tuning[] = { 62, 59, 55, 50 } ;

int	divisions   = 24 ;        // subdivions of one beat 
int	unit_length =  3 ;        // eighth notes by default 
int	nstrings = 4 ;

//	Initialize default page width.
int	page_width = 72 ;

char	output_buffer[BUFSIZE] ;
int	output_position = 0 ;
int	output_width = 0 ;


//	Test for an accidental character (sharp, natural, or flat symbol.)  
int	isacc( const char c )
{
	return c == sharp || c == natural || c == flat;
}


//	Test for a pitch character (a-g or A-G.)  
int	ispitch( const char c )
{
	return ( 'a' <= c && c <= 'g' ) || ( 'A' <= c && c <= 'G' ) ;
}


//	Parse an integer.
int	get_number( const char *buffer, int *position )
{
	int i = *position ;
	int n = 0 ;

	while ( isdigit( buffer[i] ) )
		{
			n = n * 10 + ( buffer[i++] - '0' ) ;
		}

	*position = i ;

	return n ;
}


//	Measure the width of a note (in divisions.)
int	note_width( const char *buf )
{
//	Initialize current parse position.  
	int	i = 0;

//	Initialize numerator of fractional duration.  
	int	n = 1;

//	Initialize denominator of fractional duration.  
	int	d = 1;

//	Skip over accidentals (which do not affect duration.)  
	while ( isacc( buf[i] ) )
		++i;

//	Skip over pitch.  
	if ( ispitch( buf[i] ) )
		++i;

//	Test for numerator of note duration.  
	if ( isdigit( buf[i] ) )
		{
//			Parse numerator of note duration.  
			n = get_number( buf, &i );
		}

//	Test for slash character indicating denominator.  
	if ( buf[i] == '/' )
		{
//			Skip over slash character.  
			++i;
//			Parse denominator of note duration.  
			int rv = get_number( buf, &i );
			if ( rv > 0 ) d = rv ;
		}

//	Return width of note in divisions.  
	return (unit_length * n / d);
}


int	note_pitch( const char *buffer )
{
	int	i = 0 ;
	int	p = 0 ;
	int	a = 0 ;
	char	c ;

	for ( ; isacc( c = buffer[i] ); i++ )
		{
		switch ( c )
			{
			case SHARP:
				++a;
				break;
			case FLAT:
				--a;
				break;
			}
		}

	switch ( buffer[i] )
		{
		case 'C': p = C3; break ;
		case 'D': p = D3; break ;
		case 'E': p = E3; break ;
		case 'F': p = F3; break ;
		case 'G': p = G3; break ;
		case 'A': p = A3; break ;
		case 'B': p = B3; break ;
		case 'c': p = C4; break ;
		case 'd': p = D4; break ;
		case 'e': p = E4; break ;
		case 'f': p = F4; break ;
		case 'g': p = G4; break ;
		case 'a': p = A4; break ;
		case 'b': p = B4; break ;
		}

	return p + a ;
}


int	get_note( const char *src, int *pos, char *dst )
{
	int i = *pos ;
	int j = 0 ;

	while ( isspace( src[i] ) || src[i] == '|' || src[i] == '\\' )
		++i ;

	while ( isacc( src[i] ) )
		dst[j++] = src[i++] ;

	if ( ispitch( src[i] ) )
		dst[j++] = src[i++] ;

	while ( src[i] == '/' )
		dst[j++] = src[i++];

	while ( isdigit( src[i] ) )
		dst[j++] = src[i++];

	dst[j] = '\0';
	*pos = i;

	return j;
}


int	pitch_string( int p )
{
	int	s ;

	for ( s = 0; s < nstrings; s++ )
		if ( p >= tuning[s] )
			return s ;

	return -1 ;
}

void	put_tab( const char *buffer, int n, int *width )
{
	int	p ;
	int	f ;
	int	s ;
	int	d ;
	int	i = 0 ;
	int	w = *width ;
	char	b[128] ;

	p = note_pitch( buffer ) ;
	d = note_width( buffer ) ;

	if ( w > 0 && w % 24 == 0 )
		printf( "-|-" ) ;

	*width = w + d ;

	s = pitch_string( p ) ;
	if ( s == n )
		{
			f = p - tuning[n] ;
			sprintf( b, "%d", f ) ;
			while ( b[i] != '\0' )
				putchar( b[i++] ) ;
		}

	for ( ; i < d; i++ )
		putchar( '-' ) ;
}


void	put_line( const char *buffer, int line )
{
	int	i ;
	int	w ;
	char	note_buffer[BUFSIZE];

	w = 0 ;

	printf( "|-" ) ;

	for ( i = 0; get_note( buffer, &i, note_buffer ) > 0; )
		{
			put_tab( note_buffer, line, &w );
		}

	printf( "-|\n" ) ;
}


void	put( const char *buffer )
{
	int	x ;

	for ( x = 0; x < nstrings; x++ )
		{
			put_line( buffer, x ) ;
		}

	putchar( '\n' ) ;
}


//	Output completed line.  
void	brk( void )
{
	if ( output_position > 0 )
		put( output_buffer ) ;
	output_buffer[output_position] = '\0' ;
	output_position = 0 ;
	output_width = 0 ;
}


//	Add note to output buffer.  
void	put_note( const char *note_buffer )
{
	int w, last;

//	Declare note buffer length.
	int	n ;

	w = note_width( note_buffer ) ;
	n = strlen( note_buffer ) ;
	last = output_position + n ;
	if ( output_position > 0 && output_width + w > page_width )
		{
			last = n ;
			brk() ; // output completed line 
		}
	strcpy( &output_buffer[output_position], note_buffer );
	output_position = last;
	output_width = output_width + w;
}


//	Process music code.  
void	music( const char *buffer )
{
	int  i;
	char note_buffer[BUFSIZE];

	for ( i = 0; get_note( buffer, &i, note_buffer ) > 0; )
		{
			put_note( note_buffer );
		}

	brk() ;
}


//	Process a header line.  
void	info( const char *buffer )
{
	puts( buffer );
}


//	Convert ABC format to tab.  
void	abc2tab( void )
{
	char buf[BUFSIZE];

	while ( fgets( buf, BUFSIZE, stdin ) != NULL )
		if ( isalpha( buf[0] ) && buf[1] == ':' )
			info( buf );
		else
			music( buf );
}


#ifdef DEBUG
void	test( void )
{
	assert( ispitch( 'a' ) ) ;
	assert( !ispitch( 'h' ) ) ;
	assert( ispitch( 'A' ) ) ;
	assert( !ispitch( 'H' ) ) ;
}
#endif


int 
main( int argc, char *argv[] ) 
{
#ifdef DEBUG
	test() ;
#endif
	abc2tab() ;
}


