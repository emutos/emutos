/*
 * kpgmld.c - program load
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2013-2016 The EmuTOS development team
 *
 * Authors:
 *  SCC  Steven C. Cavender
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "fs.h"
#include "proc.h"
#include "gemerror.h"
#include "pghdr.h"
#include "string.h"
#include "kprint.h"



/*
 * private macros
 */

/* this used to be in portab.h, but since it is *only* used here... */
#define SUCCESS 0

/*
 * forward prototypes
 */

static LONG     pgmld01(FH h, PD *pdptr, PGMHDR01 *hd);
static LONG     pgfix01(void *lastcp, LONG nrelbytes , PGMINFO *pi);

/*
 * kpgmhdrld - load program header
 *
 * contrary to what was before, we load the prg header first,
 * then allocate the basepage, choosing the memory pool according to
 * the flags and the amount of memory needed. Then, we actually load
 * the program.
 */

LONG kpgmhdrld(char *s, PGMHDR01 *hd, FH *h)
{
    LONG r;
    WORD magic;

    r = xopen( s , 0 );         /* open file for read */
    if( r < 0L  )
        return( r ) ;

    *h = (FH) r ;                /* get file handle */

    r = xread( *h, 2L, &magic);  /* read magic number */
    if( r < 0L )
        return( r ) ;
    if( r != 2 )
        return EPLFMT;

    /* alternate executable formats will not be handled */
    if( magic != 0x601a ) {
        KDEBUG(("BDOS xpgmld: Unknown executable format!\n"));
        return EPLFMT;
    }
    /* read in the program header */

    r = xread( *h, (LONG)sizeof(PGMHDR01), hd);
    if( r < 0L )
        return( r ) ;
    if( r != (LONG)sizeof(PGMHDR01) )
        return EPLFMT;

    return 0;
}


/*
 *  kpgmld - load program except the header (which has already been read)
 *
 *  The program space follows PD
 *
 * Arguments:
 * p - ptr to PD
 * h - file handle opened by kpgmhdrld
 * hd - the program header read by kpgmhdrld
 */

LONG   kpgmld(PD *p, FH h, PGMHDR01 *hd )
{
    LONG r;

    r = pgmld01(h, p, hd);

    KDEBUG(("BDOS pgmld01: return code=0x%lx\n",r));

    xclose(h);
    return r;
}




/*
 * pgmld01 - oldest known gemdos load format
 * It is very similar to cp/m 68k load in the (open) program file with
 * handle 'h' using load file strategy like cp/m 68k.  Specifically:
 *
 * - read in program header and determine format parameters
 * - seek past the symbol table to the start of the relo info
 * - read in the first offset (it's different than the rest in that
 *   it is a longword instead of a byte).
 * - make the first adjustment until we run out of relocation info or
 *   we have an error
 * - read in relocation info into the bss area
 * - call pgfix01() to fix up the code using that info
 * - zero out the bss
 */

static LONG     pgmld01( FH h , PD *pdptr, PGMHDR01 *hd)
{
    PGMINFO    *pi ;
    PD         *p ;
    PGMINFO             pinfo ;
    BYTE                *cp ;
    LONG                relst ;
    LONG                flen ;
    LONG                r ;

    pi = &pinfo ;
    p = pdptr ;
    relst = 0 ;

    /* calculate program load info */

    pi->pi_tlen=hd->h01_tlen;
    pi->pi_dlen=hd->h01_dlen;
    flen = pi->pi_tlen + pi->pi_dlen;

    pi->pi_blen = hd->h01_blen ;
    pi->pi_slen = hd->h01_slen ;
    pi->pi_tpalen = p->p_hitpa - p->p_lowtpa - sizeof(PD) ;
    pi->pi_tbase = (char *) (p+1) ;     /*  1st byte after PD   */
    pi->pi_bbase = pi->pi_tbase + flen ;
    pi->pi_dbase = pi->pi_tbase + pi->pi_tlen ;


    /*
     * see if there is enough room to load in the file, then see if
     * the requested bss space is larger than the space we have to offer
     */

    if( flen > pi->pi_tpalen  ||  pi->pi_tpalen-flen < pi->pi_blen )
        return( ENSMEM ) ;

    /* initialize PD fields */

    memcpy(&p->p_tbase, &pi->pi_tbase, 6 * sizeof(long));

    /*
     * read in the program file (text and data)
     * if there is an error reading in the file or if it is an abs
     * file, then we are finished
     */

    r = xread(h,flen,pi->pi_tbase);
    if( r < 0  )
        return( r ) ;

    if( hd->h01_abs )
        return( SUCCESS ) ;     /*  do we need to clr bss here? */

    /*
     * if not an absolute format, position past the symbols and start the
     * reloc pointer  (flen is tlen + dlen).  NOTE that relst is
     * init'd to 0, so if the format is absolute, we will not drop
     * into the fixup code.
     */

    if( !hd->h01_abs )
    {
        /**********  should change hard coded 0x1c  ******************/

        KDEBUG(("BDOS pgmld01: flen=0x%lx, pi_slen=0x%lx\n",flen,pi->pi_slen));

        r = xlseek(flen+pi->pi_slen+0x1c,h,0);
        if( r < 0L  )
            return( r ) ;

        r = xread( h , (long)sizeof(relst) , &relst );

        KDEBUG(("BDOS pgmld01: relst=0x%lx\n",relst));

        if( r <  0L  )
            return( r ) ;
    }

    if( relst != 0 )
    {
        cp = pi->pi_tbase + relst ;

        /*  make sure we didn't wrap memory or overrun the bss  */

        if(  cp < pi->pi_tbase  ||  cp >= pi->pi_bbase  )
            return( EPLFMT ) ;

        *((long *)(cp)) += (long)pi->pi_tbase ; /*  1st fixup     */

        flen = (long)p->p_hitpa - (long)pi->pi_bbase;   /* M01.01.0925.01 */

        for(;;)
        {
            /*  read in more relocation info  */
            r = xread(h,flen,pi->pi_bbase);
            if( r <= 0 )
                break ;

            /*  do fixups using that info  */
            r = pgfix01( cp, r , pi );
            if( r <= 0 )
                break ;
        }

        if ( r < 0 )                    /* M01.01.1023.01 */
            return( r );
    }

    /* clear the bss or the whole heap */

    if( hd->h01_flags & PF_FASTLOAD ) {
        /* clear only the bss */
        flen =  pi->pi_blen;
    } else {
        /* clear the whole heap */
        flen = (long)p->p_hitpa - (long)pi->pi_bbase;
    }
    if(flen > 0) {
        bzero(pi->pi_bbase, flen);
    }

    return( SUCCESS ) ;
}



/*
 * pgfix01 - do the next set of fixups
 *
 *  returns:
 *      addr of last modified longword in code segment (cp)
 *      0 if error or done
 *      stat01:
 *              >0: all offsets in bss used up, read in more
 *              =0: offset of 0 encountered, no more fixups
 *              <0: EPLFMT (load file format error)
 *
 * Arguments:
 *  nrelbytes - number of avail rel values
 *  pi        - program info pointer
 */

static LONG     pgfix01( void *lastcp, LONG nrelbytes , PGMINFO *pi )
{
    UBYTE      *cp ;            /*  code pointer                */
    UBYTE      *rp ;            /*  relocation info pointer     */
    LONG       n ;              /*  nbr of relocation bytes     */
    UBYTE      *bbase ;         /*  base addr of bss segment    */
    LONG       tbase ;          /*  base addr of text segment   */

    cp = lastcp ;
    rp = (UBYTE *)pi->pi_bbase ;
    n = nrelbytes ;
    tbase = (LONG) pi->pi_tbase ;
    bbase = (UBYTE *)pi->pi_bbase ;

    while( n--  &&      *rp != 0 )
    {
        if( *rp == 1 )
            cp += 0xfe ;
        else
        {
            cp += *rp ; /* add the byte at rp to cp, don't sign ext */

            if(  cp >= bbase  )
            {
                return( EPLFMT ) ;
            }
            *( (long *)(cp) ) += tbase ;

        }
        ++rp ;
    }

    return(  ++n == 0  ? 1 : SUCCESS  ) ;
}


LONG kpgm_relocate( PD *p, long length)
{
    BYTE      *cp;
    LONG      *rp;
    LONG      flen;
    PGMINFO   pinfo;
    PGMINFO   *pi;
    PGMHDR01  *hd;
    UWORD     abs_flag;

    KDEBUG(("BDOS kpgm_relocate: lotpa=0x%lx hitpa=0x%lx len=0x%lx\n",p->p_lowtpa,p->p_hitpa,length));

    hd = (PGMHDR01*)( ((char*)(p+1)) + 2 );
    pi = &pinfo;
    abs_flag = hd->h01_abs;

    pi->pi_tlen=hd->h01_tlen;
    pi->pi_dlen=hd->h01_dlen;
    flen = pi->pi_tlen + pi->pi_dlen;

    pi->pi_blen = hd->h01_blen ;
    pi->pi_slen = hd->h01_slen ;
    pi->pi_tpalen = p->p_hitpa - p->p_lowtpa - sizeof(PD) ;
    pi->pi_tbase = (char *) (p+1) ;     /*  1st byte after PD   */
    pi->pi_bbase = pi->pi_tbase + flen ;
    pi->pi_dbase = pi->pi_tbase + pi->pi_tlen ;

    /* move the code to the right position (after the PD - basepage) */
    memmove( p+1, (char*)(p+1) +2+sizeof(PGMHDR01), flen);

    KDEBUG(("BDOS kpgm_relocate: tlen=0x%lx, dlen=0x%lx, slen=0x%lx\n",
            pi->pi_tlen,pi->pi_dlen,pi->pi_slen));
    KDEBUG(("BDOS kpgm_relocate: flen=0x%lx, tpalen=0x%lx\n",flen,pi->pi_tpalen));

    /*
     * see if there is enough room to load in the file, then see if
     * the requested bss space is larger than the space we have to offer
     */

    if( flen > pi->pi_tpalen  ||  pi->pi_tpalen-flen < pi->pi_blen ) {
        KDEBUG(("BDOS kpgm_relocate: ENSMEM\n"));
        return( ENSMEM ) ;
    }

    KDEBUG(("BDOS kpgm_relocate: abs=0x%x\n",abs_flag));

    /* initialize PD fields */

    memcpy(&p->p_tbase, &pi->pi_tbase, 6 * sizeof(long));

    if( abs_flag )
        return( SUCCESS ) ;

    /* relocation information present */
    rp = (LONG*) (pi->pi_tbase+2+sizeof(PGMHDR01)+flen+pi->pi_slen);
    if ( *rp ) {
        cp = pi->pi_tbase + *rp++;

        /*  make sure we didn't wrap memory or overrun the bss  */
        if(  cp < pi->pi_tbase  ||  cp >= pi->pi_bbase  )
            return( EPLFMT ) ;

        *((long *)(cp)) += (long)pi->pi_tbase ; /*  1st fixup     */

        /* move the relocation info to the pi_bbase */
        length -= ((long)rp) - (long)pi->pi_tbase;
        memmove( pi->pi_bbase, rp, length );

        /* fixup with the reloc information available */
        pgfix01( cp, length, pi);
    }

    /* clear the whole heap */
    flen = (long)p->p_hitpa - (long)pi->pi_bbase;
    if(flen > 0) {
        bzero(pi->pi_bbase, flen);
    }

    return( SUCCESS ) ;
}
