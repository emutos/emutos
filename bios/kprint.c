/*
 *  dbgbios.c - bios debug routines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include	"portab.h"
#include "bios.h"
#if 0
#include	"kprint.h"
#endif

extern void printout(char *);
extern void cons_out(char);

#define	COMMENT	0
#define MAXDMP 1024
/*
 *  globals
 */

static  char	buffer[MAXDMP] ;

GLOBAL	char	*kcrlf = "\n\r" ;


/*==== kputs - output a null terminated string direct to the console ======*/

void cputs(char * s )
{
	while( *s )
		cons_out( *s++ ) ;
}

void cstatus(ERROR status)
{
    switch (status) {
    case SUCCESS:
        cputs("[ OK ]\n\r");
        break;
    default:
        cputs("[FAIL]\n\r");
    }
}


/*
 *  kprint - prints natively to emulator
 */

void	kprint(char *s)
{
    printout(s);
}



/*
 *  kputcrlf - output a cr/lf sequence
 *	used mostly for the convenience of asm routines
 */

void kputcrlf()
{
	kprint( kcrlf ) ;
}

/*
 *  kpress - display a message, then wait for a key to be input
 */

void kpress(char *s)
{
#if 0
    char	ch ;
#endif

	if( s )		/*  if there is a message to display		*/
  	    kprint(s) ;	/*  display it				*/

	kprint(" ...PRESS") ;
#if 0
        ch = cgetc() ;
#endif
	kputcrlf() ;
}

#if	COMMENT
/*
**  kputs - output a null terminated string direct to the console.
*/

void kputs(char * s )
{
    while(*s != '\0') {
                    *out++=*str++;
	while( *s )
		CONOUT( *s++ ) ;
}
#endif



/*
 *  kputp - output the address of a pointer (in hex)
 */

void kputp(VOID *p )
{
    char	*s ;
    char	*slhex() ;

    s = slhex( p , buffer ) ;
    kprint( buffer ) ;
}



/*
 *  kputl - output the value of a long (in hex)
 */

void kputl(LONG *p)
{
    char	*s ;
    char	*slhex() ;

    s = slhex(*p  , buffer ) ;
    kprint( buffer ) ;
}



/*
 *  kputw - output the value of a word (in hex)
 */

void kputw(WORD *p )
{
	char	*s ;
	char	*swhex() ;

	s = swhex(*p , buffer ) ;
	kprint( buffer ) ;
}

/*
 * kputb - output the value of a byte (in hex)
 */

void kputb(BYTE *p)
{
	char	*s ;
	char	*sbhex() ;


	s = sbhex(*p , buffer ) ;
	kprint( buffer ) ;
}

/**
 * kpanic - throw out a panic message and halt
 */

VOID kpanic(char * s)
{
    kprint( "BIOS: Panic: \n");
    kprint (s);
    kprint( "\n");
    while(1);
}



/*
 * ntoa - nibble to ascii
 *
 * convert a nibble to an ascii character and return the character.
 * only the low order 4 bits of the character are used.
 */

char	ntoa( BYTE n )
{
    n = (n & (BYTE)(0x0f)) + '0' ;
    return(  n > '9'  ?  n + 7  :  n  ) ;
}


/*
**  swhex - string word to hexascii - word format
**	convert a word into hex-ascii, and place it in the string, placing a
**	space and a null after it.  (e.g. if w = 0x1234, s will contain 
**	"1234 " followed by a null).
**	NOTE:  the 'loop' is 'unrolled' to make it a little quicker.
**
**	returns a pointer to the null.
*/

char	*swhex(WORD w , char *s )
{
	*s++ = ntoa( (BYTE)(  w >> 12  ) ) ;
	*s++ = ntoa( (BYTE)(  w >>  8  ) ) ;
	*s++ = ntoa( (BYTE)(  w >>  4  ) ) ;
	*s++ = ntoa( (BYTE)(    w      ) ) ;
	*s++ = ' ' ;
	*s = '\0' ;
	return( s ) ;

}


/*
**  slhex - string long to hexascii - longword format
**	convert a longword into ascii and put it in the string, followed by a
**	space and a null. (e.g. if l = 0x12345678, the string will contain
**	"12345678 " followed by a null.
**
**	return a pointer to the null.
*/

char	*slhex(LONG l, char *s)
{
	s = swhex( (WORD)( l >> 16 ) , s ) ;
	return(  swhex( (WORD)( l ) , s-1 )  ) ;
} 



/*
**  slwhex - string long to hexascii - word format
**	convert a long word into hex-ascii, and place it in the string, placing
**	a null ater it.  it is in the form of swhex.
**	(e.g., if l = 0x12345678, s will contain "1234 5678 " followed by a 
**	null).
**
**	returns a pointer to the null.
*/

char	*slwhex(LONG l, char *s )
{
	s = swhex(  (WORD)( l >> 16) , s ) ;
	return( swhex(  (WORD)( l ) , s ) ) ;
}

/*
**  swbhex - string word to hexascii in byte format
**	convert a word into hex-ascii, in sbhex format.
**	(e.g., a word with the value 0x1234 will appear in the string s as
**	"12 34 ", with a terminating null.  
**
**	returns a pointer to the null.
*/

char	*swbhex(WORD w , char *s )
{
	BYTE	*sbhex() ;

	s = sbhex( (BYTE)((w>>8) & 0x00ff) , s ) ;
	return(   sbhex((BYTE)( w & 0x00ff ) , s)    ) ;
}

/*
**  slbhex - string long to hexascii in byte format
**	convert a long word into hex-ascii, in sbhex format.
**	(e.g., a long with the value 0x12345678 will appear in the string as
**	"12 34 56 78 " followed by a null.)
**
**	returns a pointer to the null.
*/

char	*slbhex(LONG l , char *s )
{
	s = swbhex( (WORD)( l >> 16 ) , s ) ;
	return(  swbhex( (WORD)( l ) , s ) ) ; 
}

/*
**  sbhex - string byte to hexascii
**	convert a byte into hex-ascii, and place it in the string, followed
**	by a space and a null.  return pointer to the null.
*/

char	*sbhex(BYTE c , char *s )
{
	*s++ = ntoa(  c >> 4  ) ;
	*s++ = ntoa( c ) ;
	*s++ = ' '  ;
	*s = '\0' ;
	return( s ) ;
}


#if 0
/*
**  kdump - dump memory
**	dump the specified memory locations direct to the console
*/

void kdump( start , cnt )
	char	*start ;
	int	cnt ;
{
	if( cnt > MAXDMP )
	{
		kprint( "Trying to dump more than MAXDUMP\n" ) ;
		return ;
	}

	bdump( buffer , start , start , cnt ) ;

	kprint( buffer ) ;
}

/*
 *  bdump - byte dump
 *	do a byte dump of the memory buffer into the string.
 *	format: "hhhhhhhh:  12 34 56 ... nn " with null at end.
 *
 *	returns ptr to null
 *
 * s  - string buffer for converted values
 * h  - header value
 * b  - pointer to buffer to convert
 * n  - number of bytes to dump
 */

char	*bdump( char *s , LONG h , BYTE *b , int n )
{
    int	i ;

    s = DMPHDR( h , s ) ;	/*  store the header into the string	*/

    for( i = n ; i-- ; )
        s = sbhex( *b++ , s ) ;

    return( s ) ;
}

/*
 * dmphdr - dump header
 *
 * take a header value (longword) and store it in header format:
 *    	"hhhhhhhh:  "
 *  	          ^^^-- 2 spaces and a null
 * return a pointer to the null
 */

char	*dmphdr(LONG h , char *s )
{
	s = slhex( h , s ) ;	/* convert and store header value	*/
	*s++ = ':' ;		/* pad and terminate it			*/
	*s++ = ' ' ;
	*s++ = ' ' ;
	*s  = '\0' ;
	return( s ) ;
}


#endif

