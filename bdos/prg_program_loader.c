/*
 * prg_program_loader - loader for TOS PRG program format.
 *
 * Copyright (C) 2001 Lineo, Inc.
 * Copyright (C) 2015-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "program_loader.h"
#include "gemerror.h"
#include "config.h"
#include "portab.h"
#include "fs.h"
#include "kprint.h"
#include "pghdr.h"
#include "proc.h"
#include "string.h"
#include "mem.h"

#define TOS_PRG_MAGIC 0x601a
#define SIZEOF_MAGIC 2L


/* Information from the loaded program */ 
typedef struct
{
    LONG    pi_tpalen;    /*  length of tpa area          */
    UBYTE   *pi_tbase;    /*  start addr of text seg      */
    UBYTE   *pi_dbase;    /*  start addr of data seg      */
    UBYTE   *pi_bbase;    /*  start addr of bss  seg      */
} PGMINFO;

static LONG pgfix01(UBYTE *lastcp, LONG nrelbytes, const PGMINFO *pi);


static WORD can_load(FH fh)
{
    WORD r;
    WORD magic_maybe;

    r = xread(fh, SIZEOF_MAGIC, &magic_maybe);
    if (r < 0L)
        return r;
    if (r != SIZEOF_MAGIC)
        return EPLFMT;

    return magic_maybe == TOS_PRG_MAGIC ? 1 : 0;
}


static WORD get_program_info(FH fh, LOAD_STATE *lstate)
{
    WORD r;
    PGMHDR01 *header;
    
    lstate->data = header = xmalloc(sizeof(PGMHDR01));
    if (!lstate->data)
        return ENSMEM;

    /* load program header (can_load skipped the magic) */
    r = xread(fh, (LONG)sizeof(PGMHDR01), header);
    if (r < 0L)
        return r;
    if (r != (LONG)sizeof(PGMHDR01))
        return EPLFMT;
    
    lstate->flags = header->h01_flags;
    lstate->relocatable_size = header->h01_tlen + header->h01_dlen + header->h01_blen;
    return E_OK;
}

/*
 * load_program_into_memory - oldest known gemdos load format
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
static LONG prg_load_program_into_memory(FH fh, PD *p, LOAD_STATE *lstate)
{
    /* Aliases */
    PGMHDR01 *hd = (PGMHDR01*)lstate->data;
    PGMINFO  pi;
    /* Relocation stuff */
    UBYTE   *cp;
    LONG    relst;
    LONG    flen;
    LONG    r;

    relst = 0;

    /* calculate program load info */

    flen = hd->h01_tlen + hd->h01_dlen;                     /* TEXT + DATA size  */
    pi.pi_tpalen = p->p_hitpa - p->p_lowtpa;                /* TPA length        */
    pi.pi_tbase = lstate->prg_entry_point = (UBYTE *)(p+1); /* 1st byte after PD */
    pi.pi_bbase = pi.pi_tbase + flen;
    pi.pi_dbase = pi.pi_tbase + hd->h01_tlen;

    /*
     * see if there is enough room to load in the file, then see if
     * the requested bss space is larger than the space we have to offer
     */

    if ((flen > pi.pi_tpalen) || (pi.pi_tpalen - flen < hd->h01_blen))
        return ENSMEM;

    /* initialize PD fields */

    memcpy(&p->p_tbase, &(pi.pi_tbase), 6 * sizeof(long));

    /* read TEXT and DATA section into memory */
    r = xread(fh, flen, pi.pi_tbase);
    if (r < 0)
        return r;

    if (!hd->h01_abs)
    {
        /* relocation required */

        /*
         * if not an absolute format, position past the symbols and start
         * the reloc pointer (flen is tlen + dlen).
         */

        KDEBUG(("BDOS load_program_into_memory: flen=0x%lx, hd->h01_slen=0x%lx\n",flen,hd->h01_slen));

        /* the 0x1c comes from the fact that the text segment image doesn't come immediately
         * after the program header, but one word after: there is a 2-byte padding after ABSFLAG
         * and before the TEXT section's content that we need to skip. */ 
        r = xlseek(flen + hd->h01_slen + sizeof(PGMHDR01) + sizeof(UWORD), fh, 0);
        if (r < 0L)
            return r;

        r = xread(fh,(long)sizeof(relst),&relst);

        KDEBUG(("BDOS load_program_into_memory: relst=0x%lx\n",relst));

        if (r < 0L)
            return r;

        if (relst != 0)
        {
            cp = pi.pi_tbase + relst;

            /*  make sure we didn't wrap memory or overrun the bss  */

            if ((cp < pi.pi_tbase) || (cp >= pi.pi_bbase))
                return EPLFMT;

            *((long *)(cp)) += (long)pi.pi_tbase ; /*  1st fixup     */

            flen = (long)p->p_hitpa - (long)pi.pi_bbase;   /* M01.01.0925.01 */

            for ( ; ; )
            {
                /*  read in more relocation info  */
                r = xread(fh, flen, pi.pi_bbase);
                if (r <= 0)
                    break;

                /*  do fixups using that info  */
                r = pgfix01(cp, r, &pi);
                if (r <= 0)
                    break;
            }

            if (r < 0)                      /* M01.01.1023.01 */
                return r;
        }

    }

    /* clear the bss or the whole heap */
    flen = hd->h01_flags & PF_FASTLOAD
        ? hd->h01_blen                            /* clear only the bss */
        : (long)p->p_hitpa - (long)pi.pi_bbase;   /* clear the whole heap */
    
    if (flen > 0)
        bzero(pi.pi_bbase, flen);

    return E_OK;
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

static LONG pgfix01(UBYTE *lastcp, LONG nrelbytes, const PGMINFO *pi)
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

            if (cp >= bbase) /* we can apply relocation offset to TEXT and DATA only */
                return EPLFMT;
            if (((LONG)cp) & 1) /* only legit to relocate on word boundaries */
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

    flen = hd->h01_tlen + hd->h01_dlen;
    pi->pi_tpalen = p->p_hitpa - p->p_lowtpa;
    pi->pi_tbase = (UBYTE *)(p+1);      /*  1st byte after PD   */
    pi->pi_bbase = pi->pi_tbase + flen;
    pi->pi_dbase = pi->pi_tbase + hd->h01_tlen;

    /* move the code to the right position (after the PD - basepage) */
    memmove(p+1, (char*)(p+1) +2+sizeof(PGMHDR01), flen);

    KDEBUG(("BDOS kpgm_relocate: tlen=0x%lx, dlen=0x%lx, slen=0x%lx\n",
            hd->h01_tlen, hd->h01_dlen, hd->h01_slen));
    KDEBUG(("BDOS kpgm_relocate: flen=0x%lx, tpalen=0x%lx\n",flen,pi->pi_tpalen));

    /*
     * see if there is enough room to load in the file, then see if
     * the requested bss space is larger than the space we have to offer
     */

    if ((flen > pi->pi_tpalen) || (pi->pi_tpalen-flen < hd->h01_blen))
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
        rp = (LONG*) (pi->pi_tbase + 2 + sizeof(PGMHDR01) + flen + hd->h01_slen);
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

    return E_OK;
}
#endif


PROGRAM_LOADER prg_program_loader = {
    can_load,
    get_program_info,
    prg_load_program_into_memory
};
