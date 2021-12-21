#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "ps.h"

const char	*get_note( const char **src );

typedef int PITCH ;

#define	BUFSIZE	1024
#define SHARP   '^'
#define FLAT    '_'
#define NATURAL '='

#define PAGE_WIDTH	612
#define PAGE_HEIGHT	792
#define PAGE_COLUMNS	 72
#define PAGE_ROWS	 72

FILE *psout = NULL;

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
int	tuning[] = { D4, B3, G3, D3 } ;

int	divisions   = 24 ;        // subdivions of one beat 
int	unit_length =  3 ;        // eighth notes by default 
int	nstrings = 4 ;

//	Initialize default page width.
float	page_width = PAGE_WIDTH ;        // Letter size paper
float	page_height = PAGE_HEIGHT ;
float	page_columns = PAGE_COLUMNS ;
float	column_width = PAGE_WIDTH / PAGE_COLUMNS ;
float	row_height = PAGE_HEIGHT / PAGE_ROWS ;
float	margin_top = 16 ;
float	margin_left = 16 ;
float	margin_right = 16 ;
float	margin_bottom = 16 ;

int	lines_per_page ;
int	measures_per_line ;
float	divisions_per_measure ;

const char	*font_name = "Palatino-Roman" ;
const float	font_size = 11 ;

char	output_buffer[BUFSIZE] ;
int	output_position = 0 ;
float	output_width = 0 ;

float	x ;
float	y ;

const int	line_width = 1 ;

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



/*  is_chord - returns true if object is a chord  */
int	is_chord( const char *s )
{
	return s && *s == '[' ;
}


/* Parse an integer. */
int	get_number( const char **src )
{
	const char	*pos = *src ;
	int	n = 0 ;

	while ( isdigit( *pos ) )
		{
			n = n * 10 + ( *pos++ - '0' ) ;
		}

	*src = pos ;

	return n ;
}



const char *	/*  Get whitespace.  */
get_space( const char **src )
{
	const char *start ;
	const char *pos ;

	start = pos = *src ;

	while ( isspace( *pos ) || *pos == '|' || *pos == '\\' )
		++pos ;

	*src = pos ;

	return start ;
}



PITCH	/*  Get next pitch from input  */
get_pitch( const char **src )
{
	const char *pos = *src ;
	PITCH	p ;

	if ( ! ispitch( *pos ) )
		{
			return -1;
		}

	switch ( *pos++ )
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

	*src = pos ;

	return p ;
}


/*  Parse accidentals  */
int	get_alter( const char **src )
{
	const char *pos = *src ;
	int	a = 0 ;

	while ( isacc( *pos ) )
		{
			switch ( *pos++ )
				{
				case SHARP:
					++a;
					break;
				case FLAT:
					--a;
					break;
				}
		}

	*src = pos ;

	return a ;
}


/*  Get duration.  */
int	get_duration( const char **src )
{
	const char *pos = *src ;
	int n = 1, d = 1, w ;

	/* Test for numerator of note duration. */
	if ( isdigit( *pos ) )
		{
			/* Parse numerator. */
			n = get_number( &pos ) ;
		}

	/*  Test for slash character indicating denominator.  */
	if ( *pos == '/' )
		{
			if ( isdigit( *++pos ) )
				{
					d = get_number( &pos ) ;
				}
			else
				{
					d = 2 ;
					while ( *pos == '/' )
						{
							d *= 2;
							++pos;
						}
				}
		}

	/*  Return width of note in divisions.  */
	w = ( unit_length * n / d ) ;

	/*  Advance input pointer.  */
	*src = pos ;

	return w ;
}


/*  Measure the width of a note (in divisions.)  */
float	note_width( const char *buf )
{
	/*  Skip over accidentals (which do not affect duration.)  */
	( void ) get_alter( &buf ) ;

	/*  Skip over pitch.  */
	( void ) get_pitch( &buf ) ;

	/*  Get duration of note.  */
	return get_duration( &buf ) ;
}


/*  chord_width - returns width of chord (in divisions)  */
float	chord_width( const char *s )
{
	/*  Skip over start of chord marker.  */
	if ( *s++ != '[' )
		return -1 ;

	/*  Skip over notes within chord.  */
	while ( *s != '\0' && *s != ']' )
		{
			( void ) get_note( &s ) ;
		}

	/*  Skip over end of chord marker.  */
	if ( *s == ']' )
		++s ;

	return get_duration( &s ) ;
}



float	/*  width - returns the width of a musical object (in divisions)  */
width( const char *object )
{
	float w ;

	if ( is_chord( object ) )
		w = chord_width( object ) ;
	else
		w = note_width( object ) ;

	return w * column_width ;
}


int	note_pitch( const char *note )
{
	int	p ;
	int	a ;

	a = get_alter( &note ) ;
	p = get_pitch( &note ) ;

	return ( p > 0 ? p + a : -1 ) ;
}



/*  get_note - note parser  */

const char *get_note( const char **src )
{
	const char	*start = *src ;
	const char	*end ;

	/*  Skip over leading whitespace.  */
	( void ) get_space( &start ) ;

	/*  If no more notes, return 0.  */
	if ( *start == 0 )
		{
			*src = start ;
			return 0 ;
		}

	/*  Parse accidentals, pitch, and duration.  */
	end = start ;
	( void ) get_alter( &end ) ;
	( void ) get_pitch( &end ) ;
	( void ) get_duration( &end ) ;

	/*  Advance pointer.  */
	*src = end ;

	return start ;
}


/*  get_chord - parse a chord  */
const char	*get_chord( const char **src )
{
	const char	*start = *src ;
	const char	*end = start ;

	/*  Skip over start of chord marker.  */
	if ( *end == '[' )
		++end ;
	else
		{
			fprintf( stderr, "error: expected '['\n" );
			return 0 ;
		}

	/*  Parse one or more notes.  */
	while ( *end && *end != ']' )
		( void ) get_note( &end ) ;

	/*  Skip over end of chord marker.  */
	if ( *end == ']' )
		++end ;

	/*  Skip over duration.  */
	( void ) get_duration( &end ) ;

	/*  Advance input pointer.  */
	*src = end ;

	return start ;
}


/*  get_next - get next element */

const char	*get_next( const char **src )
{
	const char	*start = *src ;
	const char	*end ;

	/*  Skip over leading whitespace.  */
	( void ) get_space( &start ) ;

	/*  If no more elements, return 0.  */
	if ( *start == 0 )
		{
			*src = start ;
			return 0 ;
		}

	/*  Parse next element.  */
	end = start ;
	if ( *start == '[' )
		get_chord( &end ) ;
	else
		get_note( &end ) ;

	/*  Advance input.  */
	*src = end ;

	/*  Return pointer to start of object.  */
	return start ;
}


int	pitch_string( int p )
{
	int	s ;

	for ( s = 0; s < nstrings; s++ )
		if ( p >= tuning[s] )
			return s ;

	return -1 ;
}


int	put_tab_note( const char *buffer, int n, char *b )
{
	int	i ;
	int	p ;
	int	s ;
	int	f ;

	i = 0 ;
	p = note_pitch( buffer );
	s = pitch_string( p ) ;

	if ( s == n )
		{
			f = p - tuning[n] ;
			sprintf( b, "%d", f ) ;
			while ( b[i] != '\0' )
				putchar( b[i++] ) ;
		}

	return i ;
}


int	put_tab_chord( const char *s, int n, char *b )
{
	int	i ;
	int	imax = 0 ;

	if ( *s == '[' )
		++s ;

	while ( *s != '\0' && *s != ']' )
		{
			i = put_tab_note( get_note( &s ), n, b );
			if ( i > imax )
				imax = i ;
		}

	if ( *s == ']' )
		++s ;

	return  imax ;
}


int	put_tab_next( const char *buffer, int n, char *b )
{
	if ( *buffer == '[' )
		return put_tab_chord( buffer, n, b ) ;
	else
		return put_tab_note( buffer, n, b );
}


void	put_tab( const char *buffer, int n, float *wp )
{
	int	i = 0 ;
	float	d ;
	float	w = *wp ;
	char	b[128] ;

	d = width( buffer ) ;

	if ( w > 0 && (int)(w / column_width) % 24 == 0 )
		{
			if ( n == 0 || n + 1 == nstrings )
				printf( "-+-" ) ;
			else
				printf( "-|-" ) ;
		}

	*wp = w + d ;

	bzero( b, sizeof(b) ) ;
	i = put_tab_next( buffer, n, b ) ;
	ps_moveto( x + w, y ) ;
	ps_show( b ) ;
	ps_stroke() ;

	for ( ; i < (int)(d/column_width); i++ )
		putchar( '-' ) ;
}


void	put_line( const char *buffer, int line )
{
	float	w ;
	const char	*note ;

	w = 0 ;

	if ( line == 0 || line + 1 == nstrings )
		printf( "+-" ) ;
	else
		printf( "|-" ) ;

	ps_setlinewidth( line_width ) ;
	ps_moveto( x, y ) ;
	ps_lineto( x + page_width - margin_left - margin_right, y ) ;
	ps_stroke() ;

	while ( ( note = get_next( &buffer ) ) )
		{
			put_tab( note, line, &w );
		}

	if ( line == 0 || line + 1 == nstrings )
		printf( "-+\n" ) ;
	else
		printf( "-|\n" ) ;

	y -= row_height ;
}


void	put( const char *buffer )
{
	int	i ;

	putchar( '\n' ) ;

	for ( i = 0; i < nstrings; i++ )
		{
			put_line( buffer, i ) ;
		}

	x = margin_left ;
	y -= margin_top ;
}


//	Output completed line.  
void	brk( void )
{
	if ( output_position > 0 )
		{
			output_buffer[output_position] = '\0' ;
			output_position = 0 ;
			output_width = 0 ;
			put( output_buffer ) ;
		}
}


//	Add note to output buffer.  
void	put_note( const char *note_buffer, int n )
{
	int w, last;

	w = width( note_buffer ) ;
	last = output_position + n ;

	if ( output_position > 0 && output_width + w > page_width )
		{
			last = n ;
			brk() ; // output completed line 
		}
	strncpy( &output_buffer[output_position], note_buffer, n );
	output_position = last;
	output_width = output_width + w;
}


void	mystrncpy( char *dst, const char *src, size_t n )
{
	strncpy( dst, src, n ) ;
	dst[n] = 0 ;
}


//	Process music code.  
void	music( const char *src )
{
	const char	*note ;

	while ( ( note = get_next( &src ) ) )
		{
			put_note( note, src - note ) ;
		}
}


//	Process a header line.  
void	info( const char *buffer )
{
	//puts( buffer );
}


//	Convert ABC format to tab.  
void	abc2tab( void )
{
	char buf[BUFSIZE];

	if ( psout ) fputs( "%!PS\n", psout ) ;

	ps_selectfont( font_name, font_size ) ;

	x = margin_left ;
	y = page_height - margin_top ;

	while ( fgets( buf, BUFSIZE, stdin ) != NULL )
		if ( isalpha( buf[0] ) && buf[1] == ':' )
			info( buf );
		else
			music( buf );
	brk() ;

	if ( psout ) fputs( "showpage", psout ) ;
}


int 
main( int argc, char *argv[] ) 
{
	char *s;

	while ( --argc > 0 && (*++argv)[0] == '-' )
		for ( s = argv[0]+1; *s != '\0'; s++ )
			switch ( *s ) {
			case 'o':
				psout = fopen( *++argv, "w" );
				if ( psout == NULL ) {
					fprintf( stderr, "error opening %s", *argv );
					exit( EXIT_FAILURE );
				}
				--argc;
				break;
			}

	abc2tab() ;
}


