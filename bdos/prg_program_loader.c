#include "program_loader.h"
#include "gemerror.h"
#include "config.h"
#include "portab.h"
#include "fs.h"
#include "kprint.h"
#include "pghdr.h"
#include "proc.h"
#include "string.h"

#define TOS_PRG_MAGIC 0x601a
#if 0
#define EXT_LENGTH 3
static const char extensions[][EXT_LENGTH+1] = { "TOS", "TTP", "PRG", "PGX", "APP", "GTP" };

static char *loadable_extensions(void) {
    return extensions;
}
#endif


static LONG pgfix01(UBYTE *lastcp, LONG nrelbytes, PGMINFO *pi);


/* Tells whether this loader can manage that executable format.
 * Returns < 0 if error, >= is the number of bytes to skip the magic number. */
static WORD can_load(const UBYTE *first_bytes) {
    return *((WORD*)first_bytes) == TOS_PRG_MAGIC ? 2 : -1;
}


static WORD prg_get_program_header(FH h, PGMHDR01 *hd) {
    WORD r;

    r = xread(h, (LONG)sizeof(PGMHDR01), hd);
    if (r < 0L)
        return r;
    if (r != (LONG)sizeof(PGMHDR01))
        return EPLFMT;
    
    return 0;
}

/*
 * prg_load_program_into_memory - oldest known gemdos load format
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
static LONG prg_load_program_into_memory(FH h, PD *pdptr, PGMHDR01 *hd)
{
    PGMINFO *pi;
    PD      *p;
    PGMINFO pinfo;
    UBYTE   *cp;
    LONG    relst;
    LONG    flen;
    LONG    r;

    pi = &pinfo;
    p = pdptr;
    relst = 0;

    /* calculate program load info */

    pi->pi_tlen=hd->h01_tlen;
    pi->pi_dlen=hd->h01_dlen;
    flen = pi->pi_tlen + pi->pi_dlen;

    pi->pi_blen = hd->h01_blen;
    pi->pi_slen = hd->h01_slen;
    pi->pi_tpalen = p->p_hitpa - p->p_lowtpa - sizeof(PD);
    pi->pi_tbase = (UBYTE *) (p+1);     /*  1st byte after PD   */
    pi->pi_bbase = pi->pi_tbase + flen;
    pi->pi_dbase = pi->pi_tbase + pi->pi_tlen;


    /*
     * see if there is enough room to load in the file, then see if
     * the requested bss space is larger than the space we have to offer
     */

    if ((flen > pi->pi_tpalen) || (pi->pi_tpalen-flen < pi->pi_blen))
        return ENSMEM;

    /* initialize PD fields */

    memcpy(&p->p_tbase, &pi->pi_tbase, 6 * sizeof(long));

    /*
     * read in the program file (text and data)
     */

    r = xread(h,flen,pi->pi_tbase);
    if (r < 0)
        return r;

    if (!hd->h01_abs)
    {

        /*
         * if not an absolute format, position past the symbols and start
         * the reloc pointer (flen is tlen + dlen).
         */

        /**********  should change hard coded 0x1c  ******************/

        KDEBUG(("BDOS load_program_into_memory: flen=0x%lx, pi_slen=0x%lx\n",flen,pi->pi_slen));

        r = xlseek(flen+pi->pi_slen+0x1c,h,0);
        if (r < 0L)
            return r;

        r = xread(h,(long)sizeof(relst),&relst);

        KDEBUG(("BDOS load_program_into_memory: relst=0x%lx\n",relst));

        if (r < 0L)
            return r;

        if (relst != 0)
        {
            cp = pi->pi_tbase + relst;

            /*  make sure we didn't wrap memory or overrun the bss  */

            if ((cp < pi->pi_tbase) || (cp >= pi->pi_bbase))
                return EPLFMT;

            *((long *)(cp)) += (long)pi->pi_tbase ; /*  1st fixup     */

            flen = (long)p->p_hitpa - (long)pi->pi_bbase;   /* M01.01.0925.01 */

            for ( ; ; )
            {
                /*  read in more relocation info  */
                r = xread(h,flen,pi->pi_bbase);
                if (r <= 0)
                    break;

                /*  do fixups using that info  */
                r = pgfix01(cp, r, pi);
                if (r <= 0)
                    break;
            }

            if (r < 0)                      /* M01.01.1023.01 */
                return r;
        }

    }

    /* clear the bss or the whole heap */

    if (hd->h01_flags & PF_FASTLOAD)
    {
        flen =  pi->pi_blen;                            /* clear only the bss */
    }
    else
    {
        flen = (long)p->p_hitpa - (long)pi->pi_bbase;   /* clear the whole heap */
    }
    if (flen > 0)
        bzero(pi->pi_bbase, flen);

    return 0;
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

static LONG pgfix01(UBYTE *lastcp, LONG nrelbytes, PGMINFO *pi)
{
    UBYTE *cp;              /*  code pointer                */
    UBYTE *rp;              /*  relocation info pointer     */
    LONG  n;                /*  nbr of relocation bytes     */
    UBYTE *bbase;           /*  base addr of bss segment    */
    LONG  tbase;            /*  base addr of text segment   */

    cp = lastcp;
    rp = pi->pi_bbase;
    n = nrelbytes;
    tbase = (LONG)pi->pi_tbase;
    bbase = pi->pi_bbase;

    while(n-- && (*rp != 0))
    {
        if (*rp == 1)
            cp += 0xfe;
        else
        {
            cp += *rp;  /* add the byte at rp to cp, don't sign ext */

            if (cp >= bbase)
                return EPLFMT;
            if (((LONG)cp) & 1)
                return EPLFMT;
            *((LONG *)cp) += tbase;
        }
        ++rp;
    }

    return (++n == 0) ? 1 : 0;
}



#if DETECT_NATIVE_FEATURES
LONG kpgm_relocate(PD *p, long length)
{
    UBYTE   *cp;
    LONG    *rp;
    LONG    flen;
    PGMINFO pinfo;
    PGMINFO *pi;
    PGMHDR01 *hd;
    UWORD   abs_flag;

    KDEBUG(("BDOS kpgm_relocate: lotpa=%p hitpa=%p len=0x%lx\n",p->p_lowtpa,p->p_hitpa,length));

    hd = (PGMHDR01*)(((char*)(p+1)) + 2);
    pi = &pinfo;
    abs_flag = hd->h01_abs;

    pi->pi_tlen=hd->h01_tlen;
    pi->pi_dlen=hd->h01_dlen;
    flen = pi->pi_tlen + pi->pi_dlen;

    pi->pi_blen = hd->h01_blen;
    pi->pi_slen = hd->h01_slen;
    pi->pi_tpalen = p->p_hitpa - p->p_lowtpa - sizeof(PD);
    pi->pi_tbase = (UBYTE *)(p+1);      /*  1st byte after PD   */
    pi->pi_bbase = pi->pi_tbase + flen;
    pi->pi_dbase = pi->pi_tbase + pi->pi_tlen;

    /* move the code to the right position (after the PD - basepage) */
    memmove(p+1, (char*)(p+1) +2+sizeof(PGMHDR01), flen);

    KDEBUG(("BDOS kpgm_relocate: tlen=0x%lx, dlen=0x%lx, slen=0x%lx\n",
            pi->pi_tlen,pi->pi_dlen,pi->pi_slen));
    KDEBUG(("BDOS kpgm_relocate: flen=0x%lx, tpalen=0x%lx\n",flen,pi->pi_tpalen));

    /*
     * see if there is enough room to load in the file, then see if
     * the requested bss space is larger than the space we have to offer
     */

    if ((flen > pi->pi_tpalen) || (pi->pi_tpalen-flen < pi->pi_blen))
    {
        KDEBUG(("BDOS kpgm_relocate: ENSMEM\n"));
        return ENSMEM;
    }

    KDEBUG(("BDOS kpgm_relocate: abs=0x%x\n",abs_flag));

    /* initialize PD fields */

    memcpy(&p->p_tbase, &pi->pi_tbase, 6*sizeof(long));

    if (!abs_flag)
    {
        /* relocation information present */
        rp = (LONG*) (pi->pi_tbase+2+sizeof(PGMHDR01)+flen+pi->pi_slen);
        if (*rp)
        {
            cp = pi->pi_tbase + *rp++;

            /*  make sure we didn't wrap memory or overrun the bss  */
            if ((cp < pi->pi_tbase) || (cp >= pi->pi_bbase))
                return EPLFMT;

            *((long *)(cp)) += (long)pi->pi_tbase;  /*  1st fixup     */

            /* move the relocation info to the pi_bbase */
            length -= ((long)rp) - (long)pi->pi_tbase;
            memmove(pi->pi_bbase, rp, length);

            /* fixup with the reloc information available */
            pgfix01(cp, length, pi);
        }
    }

    /* clear the whole heap */
    flen = (long)p->p_hitpa - (long)pi->pi_bbase;
    if (flen > 0)
        bzero(pi->pi_bbase, flen);

    return 0;
}
#endif


PROGRAM_LOADER prg_program_loader = {
    can_load,
    prg_get_program_header,
    prg_load_program_into_memory
};
