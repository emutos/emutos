/*	COMPICON.H	10/05/84 - 10/10/84 	Gregg Morris		*/

#include <portab.h>
#include <machine.h>
#include <obdefs.h>

WORD		beg_file;

#include <iconlist.h>

WORD		end_file;

main()
{
	WORD		handle, length, ret;

	beg_file = &gl_ilist;
	handle = dos_create( ADDR("DESKTOP.IMG"), 0);
	length = ( ((BYTE *)&end_file) - ((BYTE *)&beg_file) );
	ret = dos_write(handle, length-2, ADDR(&beg_file+1) );
	dos_close(handle);

} /* compicon */
