/*
 * blit.c - Blitting routines
 *
 * Copyright 1999 Christer Gustavsson <cg@nocrew.org>
 * Copyright 1999 Caldera, Inc.
 * Copyright 2003 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "vdidef.h"
#include "gsxextrn.h"
#include "tosvars.h"
#include "mouse.h"
#include "kprint.h"


#define DBG_BLIT 0



/* bitblt modes */

#define BM_ALL_WHITE   0
#define BM_S_AND_D     1
#define BM_S_AND_NOTD  2
#define BM_S_ONLY      3
#define BM_NOTS_AND_D  4
#define BM_D_ONLY      5
#define BM_S_XOR_D     6
#define BM_S_OR_D      7
#define BM_NOT_SORD    8
#define BM_NOT_SXORD   9
#define BM_NOT_D      10
#define BM_S_OR_NOTD  11
#define BM_NOT_S      12
#define BM_NOTS_OR_D  13
#define BM_NOT_SANDD  14
#define BM_ALL_BLACK  15

/* bitblt declarations */
typedef struct  bbpb
{
    UWORD   b_wd;	/* width of block to blit (pixels) */
    UWORD   b_ht;       /* height of block to blit (pixels) */
    UWORD   plane_ct;   /* number of planes to blit */
    UWORD   fg_col;     /* foreground colour */
    UWORD   bg_col;     /* background colour */
    UBYTE   op_tab[4];  /* logic op table */
    UWORD   s_xmin;     /* minimum X source */
    UWORD   s_ymin;     /* minimum Y source */
    UWORD * s_form;     /* source form base addr */
    UWORD   s_nxwd;     /* offset to next word in line (bytes) */
    UWORD   s_nxln;     /* offset to next line in plane (bytes) */
    UWORD   s_nxpl;     /* offset from start of current plane to next */
    UWORD   d_xmin;     /* minimum X destination */
    UWORD   d_ymin;     /* minimum Y destination */
    UWORD * d_form;     /* destination form base addr */
    UWORD   d_nxwd;     /* offset to next word in line (bytes) */
    UWORD   d_nxln;     /* offset to next line in plane (bytes) */
    UWORD   d_nxpl;     /* offset from start of current plane to next */
    UWORD * p_addr;     /* address of pattern buffer */
    UWORD   p_nxln;     /* offset to next line in pattern (bytes) */
    UWORD   p_nxpl;     /* offset to next plane in pattern (bytes) */
    UWORD   p_mask;     /* pattern index mask */

    /* For internal use */
    UWORD   p_indx;     /* initial pattern index */
    UWORD * s_addr;     /* initial source address */
    UWORD   s_xmax;     /* maximum X: source */
    UWORD   s_ymax;     /* maximum Y: source */
    UWORD * d_addr;     /* initial destination address */
    UWORD   d_xmax;     /* maximum X: destination */
    UWORD   d_ymax;     /* maximum Y: destination */
    UWORD   inner_ct;   /* blt inner loop initial count */
    UWORD   dst_wr;     /* destination form wrap (in bytes) */
    UWORD   src_wr;     /* source form wrap (in bytes) */
} FBBLTPBLK;



FBBLTPBLK pblk;


#define LSHIFT(s) s=((ULONG)s<<16)
#define RSHIFT(s) s=((ULONG)s>>16)
#define UPPER(s) (0xFFFF0000 & ((s)<<16))

#define EVENWORD(s) (((s)+15) >> 4)

/* just to reduce possible mistakes */
#define ALL_WHITE(s,d)	(0x0000)
#define S_AND_D(s,d)	((s) & (d))
#define S_AND_NOTD(s,d)	((s) & ~(d))
#define S_ONLY(s,d)	(s)
#define NOTS_AND_D(s,d)	(~(s) & (d))
#define D_ONLY(s,d)     (d)
#define S_XOR_D(s,d)	((s) ^ (d))
#define S_OR_D(s,d)	((s) | (d))
#define NOT_SORD(s,d)	(~((s) | (d)))
#define NOT_SXORD(s,d)	(~((s) ^ (d)))
#define NOT_D(s,d)	(~(d))
#define S_OR_NOTD(s,d)	((s) | ~(d))
#define NOT_S(s,d)	(~(s))
#define NOTS_OR_D(s,d)	(~(s) | (d))
#define NOT_SANDD(s,d)	(~((s) & (d)))
#define ALL_BLACK(s,d)	(0xffff)


static UWORD
all_white(UWORD s, UWORD d)
{
    return ALL_WHITE(s,d);
}

static UWORD
s_and_d(UWORD s, UWORD d)
{
    return S_AND_D(s,d);
}

static UWORD
s_and_notd(UWORD s, UWORD d)
{
    return S_AND_NOTD(s,d);
}

static UWORD
s_only(UWORD s, UWORD d)
{
    return S_ONLY(s,d);
}

static UWORD
nots_and_d(UWORD s, UWORD d)
{
    return NOTS_AND_D(s,d);
}

static UWORD
d_only(UWORD s, UWORD d)
{
    return D_ONLY(s,d);
}

static UWORD
s_xor_d(UWORD s, UWORD d)
{
    return S_XOR_D(s,d);
}

static UWORD
s_or_d(UWORD s, UWORD d)
{
    return S_OR_D(s,d);
}

static UWORD
not_sord(UWORD s, UWORD d)
{
    return NOT_SORD(s,d);
}

static  UWORD
not_sxord(UWORD s, UWORD d)
{
    return NOT_SXORD(s,d);
}

static  UWORD
not_d(UWORD s, UWORD d)
{
    return NOT_D(s,d);
}

static  UWORD
s_or_notd(UWORD s, UWORD d)
{
    return S_OR_NOTD(s,d);
}

static  UWORD
not_s(UWORD s, UWORD d)
{
    return NOT_S(s,d);
}

static  UWORD
nots_or_d(UWORD s, UWORD d)
{
    return NOTS_OR_D(s,d);
}

static  UWORD
not_sandd(UWORD s, UWORD d)
{
    return NOT_SANDD(s,d);
}

static UWORD
all_black(UWORD s, UWORD d)
{
    return ALL_BLACK(s,d);
}

UWORD (*logop_tab[16])(UWORD s, UWORD d) = {
    all_white,
    s_and_d,
    s_and_notd,
    s_only,
    nots_and_d,
    d_only,
    s_xor_d,
    s_or_d,
    not_sord,
    not_sxord,
    not_d,
    s_or_notd,
    not_s,
    nots_or_d,
    not_sandd,
    all_black
};



/*
 * bitblt - Generic bitblt for all interleaved resolutions
 */
void 
bit_blt(FBBLTPBLK * fbb )
{
    WORD  pc;   /* plane counter */
    UWORD *dbase;
    UWORD *sbase;
    WORD  snxline,dnxline;
    WORD  snxword,dnxword;
    UWORD endmask1, endmask3;
    UWORD wordcnt, s_xmin16, s_xmax16, d_xmin16, d_xmax16;
    UWORD lendmsk=0xffff>>(fbb->d_xmin%16);
    UWORD rendmsk=~(0x7fff>>(fbb->d_xmax%16));
    WORD skew = (fbb->d_xmin - fbb->s_xmin) & 0x000f;
    UWORD prefetch = ((fbb->d_xmin%16) < (fbb->s_xmin%16));

    /* precalculate some values for speed */
    s_xmin16 = fbb->s_xmin / 16;
    s_xmax16 = fbb->s_xmax / 16;
    d_xmin16 = fbb->d_xmin / 16;
    d_xmax16 = fbb->d_xmax / 16;
    wordcnt = ((d_xmax16 - d_xmin16) - 1 );

    /* Normally blit top to bottom, left to right */
    /* Adjust sbase and dbase */
    sbase = (UWORD *)((UBYTE *)fbb->s_form + (( (LONG)fbb->s_ymin * (LONG)fbb->s_nxln ) + ( ((LONG)fbb->s_xmin/16) * (LONG)fbb->s_nxwd )));
    dbase = (UWORD *)((UBYTE *)fbb->d_form + (( (LONG)fbb->d_ymin * (LONG)fbb->d_nxln ) + ( ((LONG)fbb->d_xmin/16) * (LONG)fbb->d_nxwd )));

    /* Set next line offsets in number of WORDs */
    snxline = fbb->s_nxln>>1;
    dnxline = fbb->d_nxln>>1;

    /* Set next word offsets in number of WORDs  */
    snxword = fbb->s_nxwd>>1;
    dnxword = fbb->d_nxwd>>1;

    /* Set endmasks */
    endmask1 = lendmsk;
    endmask3 = rendmsk;

    if ( (fbb->b_wd <= 16) && (d_xmin16 == d_xmax16) )  {
        /* Blit left to right */
#if DBG_BLIT > 0
        kprintf("fbbitblt: top to bottom, left to right, wd<16, same dst word\n");
#endif
        /* Set next word offsets */
        snxword = ( s_xmin16 != s_xmax16 ) ? fbb->s_nxwd>>1 : 0;
        dnxword = 0;

        /* Set endmask */
        endmask1 = lendmsk & rendmsk;
        endmask3 = 0;
    }
    else if ( ( fbb->b_wd <= 16 ) && ( s_xmin16 == s_xmax16 ) ) {
        /* Blit left to right */
#if DBG_BLIT > 0
       kprintf("fbbitblt: top to bottom, left to right, wd<16, same src word\n");
#endif
        /* Set next word offsets */
        snxword = 0;
        dnxword = ( d_xmin16 != d_xmax16 ) ? fbb->d_nxwd>>1 : 0;
    }
    else if ( fbb->s_ymin > fbb->d_ymin ) {
        /* Blit top to bottom */
#if DBG_BLIT > 0
       kprintf("fbbitblt: top to bottom\n");
#endif
    }
    else if ( fbb->s_ymin < fbb->d_ymin ) {
        /* Blit bottom to top */
#if DBG_BLIT > 0
       kprintf("fbbitblt: bottom to top\n");
#endif
        /* Adjust sbase and dbase to bottom of block */
        (UBYTE *) sbase += ( (LONG)fbb->s_nxln * ( (LONG)fbb->b_ht - 1 ) );
        (UBYTE *) dbase += ( (LONG)fbb->d_nxln * ( (LONG)fbb->b_ht - 1 ) );

        /* Set next line offsets */
        snxline = -snxline;
        dnxline = -dnxline;
    }
    else if ( fbb->s_xmin < fbb->d_xmin ) {
#if DBG_BLIT > 0
       kprintf("fbbitblt: top to bottom, right to left\n");
#endif
        /* Blit top to bottom, right to left */
        /* Adjust sbase and dbase to top right of block */
        (UBYTE *)dbase += (((((LONG)fbb->b_wd - 1) / 16) + (((LONG)fbb->d_xmin % 16) != 0)) * (LONG)fbb->d_nxwd);
        (UBYTE *)sbase += (((((LONG)fbb->b_wd - 1) / 16) + (((LONG)fbb->s_xmin % 16) != 0)) * (LONG)fbb->s_nxwd);

        /* Set next word offsets */
        snxword = -snxword;
        dnxword = -dnxword;

        /* Set endmasks */
        endmask1 = rendmsk;
        endmask3 = lendmsk;
    }

#if DBG_BLIT > 0
    kprintf("s_form: %p d_form: %p\n",fbb->s_form,fbb->d_form);
    kprintf("sbase: %p dbase: %p\n",sbase,dbase);
    kprintf("skew: %d\n",skew);
    kprintf("prefetch: %d\n",prefetch);
    kprintf("s_xmin: %d\n", fbb->s_xmin);
    kprintf("d_xmin: %d\n", fbb->d_xmin);
    kprintf("skew  : %d\n", skew);
    kprintf("s_nxpl: %d\n", fbb->s_nxpl);
    kprintf("d_nxpl: %d\n", fbb->d_nxpl);
    kprintf("endmask1 is %x, endmask3 is %x\n", endmask1, endmask3);
#endif

    /* Blit each plane starting from the last */
    for (pc=fbb->plane_ct-1, sbase+=pc*(fbb->s_nxpl>>1), dbase+=pc*(fbb->d_nxpl>>1);
         pc >= 0 ;
         pc--, sbase-=fbb->s_nxpl>>1, dbase-=fbb->d_nxpl>>1) {

        UWORD (*logop)(UWORD s, UWORD d);
        int op_tabidx;

        /* Calculate index into op_tab */
        op_tabidx = ((fbb->fg_col>>pc) & 0x0001 ) <<1;
        op_tabidx |= (fbb->bg_col>>pc) & 0x0001;

        /* Get logic op function pointer for this plane from op_tab */
        logop = logop_tab[fbb->op_tab[op_tabidx] & 0x000f];

        if (snxword>=0) {
            /* left to right blit entered */

            if (logop == s_only) {
                /* Just for replace mode, most often used! */
                /* This is extra simplyfied code for speed */
                int j;
                UWORD *lsbase = sbase;
                UWORD *ldbase = dbase;

                /* Do for each line in the blit */
                for (j=fbb->b_ht-1; j>= 0; j--,
                lsbase+=snxline, ldbase+=dnxline) {

                    ULONG sbuf=0x00000000;
                    UWORD *s=lsbase;
                    UWORD *d=ldbase;
                    int i;

                    /* If source pixel on word boundary, don't prefetch */
                    if (prefetch) {
                        sbuf=*s;
                        sbuf <<= 16;
                        s += snxword;
                    }

                    /* Do first word */
                    sbuf |= *s;
                    *d = *d & ~endmask1;
                    *d |= (UWORD)(sbuf>>skew) & endmask1;

                    /* One word blit */
                    if (dnxword==0)
                        continue;

                    /* Do middle words */
                    for (i=wordcnt-1 ; i >= 0; i--) {
                        s+=snxword;
                        sbuf <<= 16;
                        sbuf |= *s;
                        d+=dnxword;
                        *d = (UWORD)(sbuf>>skew);
                    }

                    /* Do last word */
                    s += snxword;
                    sbuf <<= 16;
                    sbuf |= *s;
                    d += dnxword;
                    *d = *d & ~endmask3;
                    *d |= (UWORD)(sbuf>>skew) & endmask3;
                }
            }
            else {
                /* not replace mode, for all the not so often used modes */
                int j;
                UWORD *lsbase = sbase;
                UWORD *ldbase = dbase;

                /* Do for each line in the blit */
                for (j=fbb->b_ht-1; j>= 0; j--,
                lsbase+=snxline, ldbase+=dnxline) {

                    ULONG sbuf=0x00000000;
                    UWORD *s=lsbase;
                    UWORD *d=ldbase;
                    int i;

                    /* If source pixel on word boundary, don't prefetch */
                    if (prefetch) {
                        sbuf = *s;
                        sbuf <<= 16;
                        s += snxword;
                    }

                    /* Do first word */
                    sbuf |= *s;
                    *d &= ~endmask1;
                    *d |= (*logop)((UWORD)(sbuf>>skew), *d) & endmask1;

                    /* One word blit */
                    if (dnxword==0)
                        continue;

                    /* Do middle words */
                    for (i=wordcnt-1 ; i >= 0; i--) {
                        s += snxword;
                        d += dnxword;
                        sbuf <<= 16;
                        sbuf |=  *s;
                        *d = (*logop)((UWORD)(sbuf>>skew), *d);
                    }

                    /* Do last word */
                    s += snxword;
                    d += dnxword;
                    sbuf <<= 16;
                    sbuf |= *s;
                    *d &= ~endmask3;
                    *d |= (*logop)((UWORD)(sbuf>>skew), *d) & endmask3;
                }
            }
        }
        else {
            /* right to left blit entered */
            int j;
            UWORD *lsbase = sbase;
            UWORD *ldbase = dbase;

            /* Do for each line in the blit */
            for (j=fbb->b_ht-1; j>= 0; j--,
            lsbase+=snxline, ldbase+=dnxline) {

                ULONG sbuf=0x00000000;
                UWORD *s=lsbase;
                UWORD *d=ldbase;
                int i;

                /* Do first word */
                sbuf |= UPPER((ULONG)*s);
                RSHIFT(sbuf);
                s += snxword;
                sbuf |= UPPER((ULONG)*s);
                *d &= ~endmask1;
                *d |= (*logop)((UWORD)(sbuf>>skew), *d) & endmask1;

                /* Do middle words */
                for (i=wordcnt-1 ; i >= 0; i--) {
                    s += snxword;
                    d += dnxword;
                    sbuf = (sbuf>>16) | ((ULONG)*s<<16);
                    *d = (*logop)((UWORD)(sbuf>>skew), *d);
                }

                /* Do last word */
                s += snxword;
                d += dnxword;
                sbuf = (sbuf>>16) | ((ULONG)*s<<16);
                *d &= ~endmask3;
                *d |= (*logop)((UWORD)(sbuf>>skew), *d) & endmask3;
            }
        }
    }
}



/*
 * getbltpblk - set defaultvalues for new parameter block
 */
static void
getbltpblk(FBBLTPBLK *fbb)
{
    fbb->b_wd = xres;
    fbb->b_ht = yres;
    fbb->plane_ct = INQ_TAB[4];     /* bytes per Pixel*/

    fbb->fg_col = fbb->bg_col = 0;
    fbb->op_tab[0] = 0x03; /* dest = src */
    fbb->op_tab[1] = 0x03; /* dest = src */
    fbb->op_tab[2] = 0x03; /* dest = src */
    fbb->op_tab[3] = 0x03; /* dest = src */
    fbb->s_xmin = 0;
    fbb->s_ymin = 0;
    fbb->d_xmin = 0;
    fbb->d_ymin = 0;

    /* these aren't used yet, but we could just as well set them up anyway */
    fbb->p_addr = NULL;
    fbb->p_nxln = 0;
    fbb->p_nxpl = 0;
    fbb->p_mask = 0;

    /* VDI driver internal usage - needed??? */
    fbb->p_indx = 0;
    fbb->s_addr = NULL;
    fbb->s_xmax = 0;
    fbb->s_ymax = 0;
    fbb->d_addr = NULL;
    fbb->d_xmax  = 0;
    fbb->d_ymax = 0;
    fbb->inner_ct = 0;
    fbb->dst_wr  = 0;
    fbb->src_wr = 0;

    /* setup for interleaved planes */

    /* source and destination form base addr */
    fbb->s_form = fbb->d_form = (UWORD*)v_bas_ad;
    /* offset to next word in line (bytes) */
    fbb->s_nxwd = fbb->d_nxwd = INQ_TAB[4] * 2;
    /* offset to next line in plane (bytes) */
    fbb->s_nxln = fbb->d_nxln = EVENWORD(xres) * 2 * INQ_TAB[4];     /* bytes per Pixel*/
    /* offset from start of current plane to next */
    fbb->s_nxpl = fbb->d_nxpl = 2;
}


/*
 * min - return min of two values
 */
inline static WORD
min(WORD a, WORD b)
{
    return( (a < b) ? a : b );
}

/*
 * min - return max of two values
 */
inline static WORD
max(WORD a, WORD b)
{
    return( (a > b) ? a : b );
}



inline void
fix_rect(RECT *cor)
{
    RECT t;

    t.x1 = min(cor->x1, cor->x2);
    t.y1 = min(cor->y1, cor->y2);
    cor->x2 = max(cor->x1, cor->x2);
    cor->y2 = max(cor->y1, cor->y2);
    cor->x1 = t.x1;
    cor->y1 = t.y1;
}



inline WORD
do_rectclip(RECT *cor, RECT *clip)
{
    fix_rect(cor);

    if(((cor->x1 < clip->x1) && (cor->x2 < clip->x1)) ||
       ((cor->x1 > clip->x2) && (cor->x2 > clip->x2)) ||
       ((cor->y1 < clip->y1) && (cor->y2 < clip->y1)) ||
       ((cor->y1 > clip->y2) && (cor->y2 > clip->y2)))
        return 0;

    cor->x1 = max(cor->x1, clip->x1);
    cor->y1 = max(cor->y1, clip->y1);
    cor->x2 = min(cor->x2, clip->x2);
    cor->y2 = min(cor->y2, clip->y2);

    return 1;
}

/*
 * gen_cpyfm - general copy raster
 */
static void
gen_cpyfm(int tran)
{
    struct attribute *work_ptr;
    FBBLTPBLK myfbb;
    FBBLTPBLK * fbb;
    MFDB *src,*dst;
    RECT *srccor, *dstcor;
    RECT tmpcor;
    int mode;

    work_ptr = cur_work;

    /* Check out the areas to copy */
    srccor = (RECT*)&PTSIN[0];
    dstcor = (RECT*)&PTSIN[4];

    /* Get the pointers to the MFDBs */
    src = *(MFDB **)&CONTRL[7];
    dst = *(MFDB **)&CONTRL[9];

    /* order the coordinates as I like them. */
    fix_rect(srccor);
    fix_rect(dstcor);

    /* Completely ignore what the user says what the destination
     width and height should be and recalculate them here.
     They must be recalculated after the fix. */
    dstcor->x2 = srccor->x2 - srccor->x1 + dstcor->x1;
    dstcor->y2 = srccor->y2 - srccor->y1 + dstcor->y1;
    tmpcor = *dstcor;

    /* check first if clipping takes away everything,
     if destination is the screen */
    if ((work_ptr->clip = *INTIN) != 0) {
        if(!dst->fd_addr && !do_rectclip(dstcor, (RECT *)&XMN_CLIP)) {
            return;
        }
    }

    /* setup fbb according to the screen format */
    fbb = &myfbb;
    getbltpblk(fbb);

    mode  = INTIN[0];

    /* choose, if transparent */
    if (tran) {
        /* foreground and background colour to put block in */
        fbb->fg_col = INTIN[1];
        fbb->bg_col = INTIN[2];

        /* emulate the four modes with different combinations
         of the ordinary 15 bitblt modes. */
        switch(mode) {
        case MD_TRANS:
            fbb->op_tab[0] = BM_NOTS_AND_D;
            fbb->op_tab[1] = BM_NOTS_AND_D;
            fbb->op_tab[2] = BM_S_OR_D;
            fbb->op_tab[3] = BM_S_OR_D;
            break;

        case MD_XOR:
            fbb->op_tab[0] = BM_S_XOR_D;
            fbb->op_tab[1] = BM_S_XOR_D;
            fbb->op_tab[2] = BM_S_XOR_D;
            fbb->op_tab[3] = BM_S_XOR_D;
            break;

        case MD_ERASE:
            fbb->op_tab[0] = BM_S_AND_D;
            fbb->op_tab[1] = BM_S_AND_D;
            fbb->op_tab[2] = BM_S_AND_D;
            fbb->op_tab[3] = BM_NOTS_OR_D;
            break;

        case MD_REPLACE:
        default: /* illegal mode specified, use replace mode */
            fbb->op_tab[0] = BM_ALL_WHITE;
            fbb->op_tab[1] = BM_NOT_S;
            fbb->op_tab[2] = BM_S_ONLY;
            fbb->op_tab[3] = BM_ALL_BLACK;
        }
    }
    else {
        fbb->op_tab[0] = (BYTE)mode;
        fbb->op_tab[1] = (BYTE)mode;
        fbb->op_tab[2] = (BYTE)mode;
        fbb->op_tab[3] = (BYTE)mode;
    }

    /* dst size is the same as src size here, only that dst
     might have been clipped earlier */
    fbb->b_wd = dstcor->x2 - dstcor->x1 + 1;
    fbb->b_ht = dstcor->y2 - dstcor->y1 + 1;

    /* adjust coordinates if destination is clipped */
    fbb->s_xmin = srccor->x1 + dstcor->x1-tmpcor.x1;
    fbb->s_ymin = srccor->y1 + dstcor->y1-tmpcor.y1;
    if(src->fd_addr) {
        fbb->plane_ct = src->fd_nplanes; /* use source no of bitplanes! */
        fbb->s_form = src->fd_addr;

        /* block is in VDI format == in machine dependent format  */
        fbb->s_nxwd = 2;
        fbb->s_nxln = src->fd_wdwidth * 2;
        fbb->s_nxpl = src->fd_wdwidth * 2 * src->fd_h;

        /*
         * "machine dependent" could be discussed.
         * On one hand, machine dependent _is_ machine dependent, but
         * on the other hand, machine dependent is most likely associated
         * with Atari interleaved format. For now we treat them exactly the
         * same
         * if(src->fd_stand) {
         *   block is in VDI format
         * }
         * else {
         *   block is in machine dependent format
         */

    } /* else source is the screen, i.e. already setup by getbltpblk() */

    fbb->d_xmin = dstcor->x1;
    fbb->d_ymin = dstcor->y1;
    if(dst->fd_addr) {
        fbb->d_form = dst->fd_addr;

        /* block is in VDI format == in machine dependent format  */
        fbb->d_nxwd = 2;
        fbb->d_nxln = dst->fd_wdwidth * 2;
        fbb->d_nxpl = dst->fd_wdwidth * 2 * dst->fd_h;

    } /* else destination is the screen, i.e. already setup by getbltpblk() */

    fbb->s_xmax = fbb->s_xmin + fbb->b_wd - 1;
    fbb->d_xmax = fbb->d_xmin + fbb->b_wd - 1;

    bit_blt (fbb);

    CONTRL[N_PTSOUT] = 0;
    CONTRL[N_INTOUT] = 0;

#if DBG_BLIT > 0
    kprintf("gen_cpyfm: %d,%d -> %d,%d  w: %d h: %d mode: %d\n",
            fbb->s_xmin, fbb->s_ymin, fbb->d_xmin, fbb->d_ymin,
            fbb->b_wd, fbb->b_ht, mode);
#endif

}



/*
 * dro_cpyfm - copy raster opaque
 *
 * This function copies a rectangular raster area from source form to
 * destination form using the logic operation specified by the application.
 */
void
dro_cpyfm()
{
    gen_cpyfm(0);
}



/*
 * drt_cpyfm - copy raster transparent
 *
 * This function copies a monochrome raster area from source form to a
 * color area. A writing mode and color indices for  both 0's and 1's
 * are specified in the INTIN array.
 */
void
drt_cpyfm()
{
    /* transparent blit */
    gen_cpyfm(1);
}



/*
 * vr_trnfm - Convert bitmaps
 *
 * Convert device independant bitmaps into device dependants and vice versa.
 *
 * The major difference is, that in the device independant format the planes
 * are consecutive, while on the screen they are interleaved.
 */
void
vr_trnfm()
{
    MFDB *src_mfdb, *dst_mfdb;
    WORD *src;
    WORD *dst;
    WORD height, wdwidth, planes, standard;

    /* Get the pointers to the MFDBs */
    src_mfdb = *(MFDB **)&CONTRL[7];
    dst_mfdb = *(MFDB **)&CONTRL[9];

    src = src_mfdb->fd_addr;
    dst = dst_mfdb->fd_addr;
    height = src_mfdb->fd_h;
    wdwidth = src_mfdb->fd_wdwidth;            /* Pixels / 16 */
    planes = src_mfdb->fd_nplanes;
    standard = src_mfdb->fd_stand;

    /* Is the transformation in or out of place? */
    if (src != dst) {
        /* transformation is completely non-overlapped (out of place) */
         if (standard) {
            /* Source is in standard format and device independent (raster area) */
            LONG plane_total;
            WORD h;

            plane_total = (LONG)wdwidth * height;
            for(h = height - 1; h >= 0; h--) {
                WORD w;

                for(w = wdwidth - 1; w >= 0; w--) {
                    WORD *tmp;
                    WORD p;

                    tmp = src;
                    for(p = planes - 1; p >= 0; p--) {
                        *dst++ = *src;
                        src += plane_total;
                    }
                    src = tmp + 1;
                }
            }
        } else {
            /* Source is device dependent (physical device) */
            WORD p;

            for(p = planes - 1; p >= 0; p--) {
                WORD *tmp;
                WORD h;

                tmp = src;
                for(h = height - 1; h >= 0; h--) {
                    WORD w;

                    for(w = wdwidth - 1; w >= 0; w--) {
                        *dst++ = *src;
                        src += planes;
                    }
                }
                src = tmp + 1;
            }
        }
    } else {
        /* transformation is completely overlapped (in place ).*/
        if (!standard) {
            /* Source is device dependent (physical device) */
            WORD p;

            for(p = planes - 1; p >= 0; p--) {
                WORD shift;
                WORD h;

                shift = p + 1;
                src = dst + (p + 1);
                for(h = height - 1; h >= 0; h--) {
                    WORD w;

                    for(w = wdwidth - 1; w >= 0; w--) {
                        WORD first;
                        WORD *lower;
                        WORD *higher;
                        WORD s;

                        first = *src;
                        lower = src;
                        higher = (src + 1);
                        for(s = shift - 1; s >= 0; s--) {
                            *--higher = *--lower;
                        }
                        *dst++ = first;
                        src += (p + 1);
                        shift += p;
                    }
                }
            }
        }
    }
}


