/*
 * vdi_raster.c - Blitting routines
 *
 * Copyright 2002 Joachim Hoenig (blitter)
 * Copyright 2003-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "vdi_defs.h"
#include "blitter.h"
#include "../bios/lineavars.h"
#include "../bios/tosvars.h"
#include "../bios/machine.h"    /* for blitter-related items */
#include "../bios/processor.h"  /* for cache control routines */
#include "kprint.h"

#ifdef __mcoldfire__
#define ASM_BLIT_IS_AVAILABLE   0   /* assembler routine does not support ColdFire */
#else
#define ASM_BLIT_IS_AVAILABLE   1   /* may use m68k assembler fast_bit_blt routine */
#endif


#if CONF_WITH_BLITTER || !ASM_BLIT_IS_AVAILABLE
#define FXSR    0x80
#define NFSR    0x40
#define SKEW    0x0f
#define BUSY    0x80
#define HOG     0x40
#define SMUDGE  0x20
#define LINENO  0x0f

#define GetMemW(addr) ((ULONG)*(UWORD*)(addr))
#define SetMemW(addr, val) *(UWORD*)(addr) = val

/* blitter registers */
typedef struct blit blit;
struct blit {
    UWORD          halftone[16];
    WORD           src_x_inc, src_y_inc;
    ULONG          src_addr;
    WORD           end_1, end_2, end_3;
    WORD           dst_x_inc, dst_y_inc;
    ULONG          dst_addr;
    UWORD          x_cnt, y_cnt;
    BYTE           hop, op, status, skew;
};

/* setting of skew flags */

/* ---QUALIFIERS--- -ACTIONS-
 * dirn equal Sx&F>
 * L->R spans Dx&F  FXSR NFSR
 *  0     0     0     0    1  |..ssssssssssssss|ssssssssssssss..|
 *                            |......dddddddddd|dddddddddddddddd|dd..............|
 *
 *  0     0     1     1    0  |......ssssssssss|ssssssssssssssss|ss..............|
 *                            |..dddddddddddddd|dddddddddddddd..|
 *
 *  0     1     0     1    1  |..ssssssssssssss|ssssssssssssss..|
 *                            |...ddddddddddddd|ddddddddddddddd.|
 *
 *  0     1     1     0    0  |...sssssssssssss|sssssssssssssss.|
 *                            |..dddddddddddddd|dddddddddddddd..|
 *
 *  1     0     0     0    1  |..ssssssssssssss|ssssssssssssss..|
 *                            |......dddddddddd|dddddddddddddddd|dd..............|
 *
 *  1     0     1     1    0  |......ssssssssss|ssssssssssssssss|ss..............|
 *                            |..dddddddddddddd|dddddddddddddd..|
 *
 *  1     1     0     0    0  |..ssssssssssssss|ssssssssssssss..|
 *                            |...ddddddddddddd|ddddddddddddddd.|
 *
 *  1     1     1     1    1  |...sssssssssssss|sssssssssssssss.|
 *                            |..dddddddddddddd|dddddddddddddd..|
 */

#define mSkewFXSR    0x80
#define mSkewNFSR    0x40

static const UBYTE skew_flags[8] = {
                        /* for blit direction Right->Left */
    mSkewNFSR,              /* Source span < Destination span */
    mSkewFXSR,              /* Source span > Destination span */
    mSkewNFSR+mSkewFXSR,    /* Spans equal, Shift Source right */
    0,                      /* Spans equal, Shift Source left */
                        /* for blit direction Left->Right */
    mSkewNFSR,              /* Source span < Destination span */
    mSkewFXSR,              /* Source span > Destination span */
    0,                      /* Spans equal, Shift Source right */
    mSkewNFSR+mSkewFXSR     /* Spans equal, Shift Source left */
};
#endif


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

/* flag:1 SOURCE and PATTERN   flag:0 SOURCE only */
#define PAT_FLAG        16

/* PTSIN ARRAY OFFSETs */
#define XMIN_S  0       /* x of upper left of source rectangle */
#define YMIN_S  1       /* y of upper left of source rectangle */
#define XMAX_S  2       /* x of lower right of source rectangle */
#define YMAX_S  3       /* y of lower right of source rectangle */

#define XMIN_D  4       /* x of upper left of destination rectangle */
#define YMIN_D  5       /* y of upper left of destination rectangle */
#define XMAX_D  6       /* x of lower right of destination rectangle */
#define YMAX_D  7       /* y of lower right of destination rectangle */


/* 76-byte line-A BITBLT struct passing parameters to bitblt */
struct blit_frame {
    WORD b_wd;          /* +00 width of block in pixels */
    WORD b_ht;          /* +02 height of block in pixels */
    WORD plane_ct;      /* +04 number of consecutive planes to blt */
    UWORD fg_col;       /* +06 foreground color (logic op table index:hi bit) */
    UWORD bg_col;       /* +08 background color (logic op table index:lo bit) */
    UBYTE op_tab[4];    /* +10 logic ops for all fore and background combos */
    WORD s_xmin;        /* +14 minimum X: source */
    WORD s_ymin;        /* +16 minimum Y: source */
    UWORD * s_form;     /* +18 source form base address */
    WORD s_nxwd;        /* +22 offset to next word in line  (in bytes) */
    WORD s_nxln;        /* +24 offset to next line in plane (in bytes) */
    WORD s_nxpl;        /* +26 offset to next plane from start of current plane */
    WORD d_xmin;        /* +28 minimum X: destination */
    WORD d_ymin;        /* +30 minimum Y: destination */
    UWORD * d_form;     /* +32 destination form base address */
    WORD d_nxwd;        /* +36 offset to next word in line  (in bytes) */
    WORD d_nxln;        /* +38 offset to next line in plane (in bytes) */
    WORD d_nxpl;        /* +40 offset to next plane from start of current plane */
    UWORD * p_addr;     /* +42 address of pattern buffer   (0:no pattern) */
    WORD p_nxln;        /* +46 offset to next line in pattern  (in bytes) */
    WORD p_nxpl;        /* +48 offset to next plane in pattern (in bytes) */
    WORD p_mask;        /* +50 pattern index mask */

    /* these frame parameters are internally set */
    WORD p_indx;        /* +52 initial pattern index */
    UWORD * s_addr;     /* +54 initial source address */
    WORD s_xmax;        /* +58 maximum X: source */
    WORD s_ymax;        /* +60 maximum Y: source */
    UWORD * d_addr;     /* +62 initial destination address */
    WORD d_xmax;        /* +66 maximum X: destination */
    WORD d_ymax;        /* +68 maximum Y: destination */
    WORD inner_ct;      /* +70 blt inner loop initial count */
    WORD dst_wr;        /* +72 destination form wrap (in bytes) */
    WORD src_wr;        /* +74 source form wrap (in bytes) */
};

/* Raster definitions */
typedef struct {
    void *fd_addr;
    WORD fd_w;
    WORD fd_h;
    WORD fd_wdwidth;
    WORD fd_stand;
    WORD fd_nplanes;
    WORD fd_r1;
    WORD fd_r2;
    WORD fd_r3;
} MFDB;


extern void linea_blit(struct blit_frame *info); /* called only from linea.S */
extern void linea_raster(void); /* called only from linea.S */
#if ASM_BLIT_IS_AVAILABLE
extern void fast_bit_blt(void); /* in vdi_blit.S */
#endif

/* holds VDI internal info for bit_blt() */
static struct blit_frame vdi_info;

/* which blit information to use, should be set before calling bit_blt() */
struct blit_frame *blit_info;

/*
 * vdi_vr_trnfm - transform screen bitmaps
 *
 * Convert device-independent bitmaps to device-dependent and vice versa
 *
 * The major difference between the two formats is that, in the device-
 * independent ("standard") form, the planes are consecutive, while on
 * the Atari screen they are interleaved.
 */
void vdi_vr_trnfm(Vwk * vwk)
{
    MFDB *src_mfdb, *dst_mfdb;
    WORD *src, *dst, *work;
    WORD planes;
    BOOL inplace;
    LONG size, inner, outer, i, j;

    /* Get the pointers to the MFDBs */
    src_mfdb = *(MFDB **)&CONTRL[7];
    dst_mfdb = *(MFDB **)&CONTRL[9];

    src = src_mfdb->fd_addr;
    dst = dst_mfdb->fd_addr;
    planes = src_mfdb->fd_nplanes;
    size = (LONG)src_mfdb->fd_h * src_mfdb->fd_wdwidth; /* size of plane in words */
    inplace = (src==dst);

    if (src_mfdb->fd_stand)     /* source is standard format */
    {
        dst_mfdb->fd_stand = 0;     /* force dest to device-dependent */
        outer = planes;             /* set outer & inner loop counts */
        inner = size;
    }
    else                        /* source is device-dependent format */
    {
        dst_mfdb->fd_stand = 1;     /* force dest to standard */
        outer = size;               /* set loop counts */
        inner = planes;
    }

    if (!inplace)               /* the simple option */
    {
        for (i = 0; i < outer; i++, dst++)
        {
            for (j = 0, work = dst; j < inner; j++)
            {
                *work = *src++;
                work += outer;
            }
        }
        return;
    }

    /* handle in-place transform - can be slow (on Atari TOS too) */
    if (planes == 1)            /* for mono, there is no difference    */
        return;                 /* between standard & device-dependent */

    if (--outer <= 0)
        return;

    for (--inner; inner >= 0; inner--)
    {
        LONG count;
        for (i = 0, count = 0L; i < outer; i++)
        {
            WORD temp;
            src += inner + 1;
            temp = *src;
            dst = src;
            work = src;
            count += inner;
            for (j = 0; j < count; j++)
            {
                work = dst--;
                *work = *dst;
            }
            *dst = temp;
        }
        src = work;
    }
}


#if CONF_WITH_BLITTER
/*
 * blitter_do_blit()
 *
 * Interface to hardware blitter for raster functions
 */
static void
blitter_do_blit(blit *blt)
{
    LONG length;
    /*
     * since the blitter doesn't see the data cache, and we may be in
     * copyback mode (e.g. the FireBee), we must flush the data cache
     * first to ensure that the memory is current.  we strictly do not
     * need to calculate the length, since the current cache control
     * routines ignore it & act on the whole cache anyway.
     */
    length = (blt->y_cnt * blt->dst_y_inc) + (blt->x_cnt * blt->dst_x_inc);
    flush_data_cache((void *)blt->dst_addr,length);

    BLITTER->src_x_incr = blt->src_x_inc;
    BLITTER->src_y_incr = blt->src_y_inc;
    BLITTER->src_addr = (UWORD *)blt->src_addr;
    BLITTER->endmask_1 = blt->end_1;
    BLITTER->endmask_2 = blt->end_2;
    BLITTER->endmask_3 = blt->end_3;
    BLITTER->dst_x_incr = blt->dst_x_inc;
    BLITTER->dst_y_incr = blt->dst_y_inc;
    BLITTER->dst_addr = (UWORD *)blt->dst_addr;
    BLITTER->x_count = blt->x_cnt;
    BLITTER->y_count = blt->y_cnt;
    BLITTER->op = blt->op;
    BLITTER->hop = blt->hop;
    BLITTER->skew = blt->skew;

    /*
     * we run the blitter in the Atari-recommended way: use no-HOG mode,
     * and manually restart the blitter until it's done.
     */
    BLITTER->status = BUSY;     /* no-HOG mode */
    __asm__ __volatile__(
    "lea    0xFFFF8A3C,a0\n\t"
    "0:\n\t"
    "tas    (a0)\n\t"
    "nop\n\t"
    "jbmi   0b\n\t"
    :
    :
    : "a0", "memory", "cc"
    );

    /*
     * we've modified data behind the cpu's back, so we must
     * invalidate any cached data.
     */
    invalidate_data_cache((void *)blt->dst_addr,length);
}
#endif


#if !ASM_BLIT_IS_AVAILABLE
/*
 * the following is a modified version of a blitter emulator, with the HOP
 * processing removed since it is always called with a HOP value of 2 (source)
 */
static void
do_blit(blit * blt)
{
    ULONG   blt_src_in;
    UWORD   blt_src_out, blt_dst_in, blt_dst_out, mask_out;
    int     last, first;
    UWORD   xc;

    KDEBUG(("do_blit(): Start\n"));
    /*
     * note: because HOP is always set to source, the halftone RAM
     * and the starting halftone line number (status&0x0f) are not
     * used and so are not dumped at the moment ...
     */
    KDEBUG(("X COUNT %u\n",blt->x_cnt));
    KDEBUG(("Y COUNT %u\n",blt->y_cnt));
    KDEBUG(("X S INC %d\n",blt->src_x_inc));
    KDEBUG(("Y S INC %d\n",blt->src_y_inc));
    KDEBUG(("X D INC %d\n",blt->dst_x_inc));
    KDEBUG(("Y D INC %d\n",blt->dst_y_inc));
    KDEBUG(("ENDMASK 0x%04x-%04x-%04x\n",(UWORD)blt->end_1,(UWORD)blt->end_2,(UWORD)blt->end_3));
    KDEBUG(("S_ADDR  %p\n",(UWORD *)blt->src_addr));
    KDEBUG(("D_ADDR  %p\n",(UWORD *)blt->dst_addr));
    KDEBUG(("HOP %d, OP %d\n",blt->hop&0x03,blt->op&0x0f));
    KDEBUG(("NFSR=%d,FXSR=%d,SKEW=%d\n",
            (blt->skew&NFSR)!=0,(blt->skew&FXSR)!=0,(blt->skew & SKEW)));

    do {
        xc = blt->x_cnt;
        first = 1;
        blt_src_in = 0;
        do {
            last = (xc == 1);
            /* read source into blt_src_in */
            if (blt->src_x_inc >= 0) {
                if (first && (blt->skew & FXSR)) {
                    blt_src_in = GetMemW (blt->src_addr);
                    blt->src_addr += blt->src_x_inc;
                }
                blt_src_in <<= 16;

                if (last && (blt->skew & NFSR)) {
                    blt->src_addr -= blt->src_x_inc;
                } else {
                    blt_src_in |= GetMemW (blt->src_addr);
                    if (!last) {
                        blt->src_addr += blt->src_x_inc;
                    }
                }
            } else {
                if (first &&  (blt->skew & FXSR)) {
                    blt_src_in = GetMemW (blt->src_addr);
                    blt->src_addr +=blt->src_x_inc;
                } else {
                    blt_src_in >>= 16;
                }
                if (last && (blt->skew & NFSR)) {
                    blt->src_addr -= blt->src_x_inc;
                } else {
                    blt_src_in |= (GetMemW (blt->src_addr) << 16);
                    if (!last) {
                        blt->src_addr += blt->src_x_inc;
                    }
                }
            }
            /* shift blt->skew times into blt_src_out */
            blt_src_out = blt_src_in >> (blt->skew & SKEW);

            /* read destination into blt_dst_in */
            blt_dst_in = GetMemW (blt->dst_addr);
            /* op into blt_dst_out */
            switch (blt->op & 0xf) {
            case BM_ALL_WHITE:
                blt_dst_out = 0;
                break;
            case BM_S_AND_D:
                blt_dst_out = blt_src_out & blt_dst_in;
                break;
            case BM_S_AND_NOTD:
                blt_dst_out = blt_src_out & ~blt_dst_in;
                break;
            case BM_S_ONLY:
                blt_dst_out = blt_src_out;
                break;
            case BM_NOTS_AND_D:
                blt_dst_out = ~blt_src_out & blt_dst_in;
                break;
            case BM_D_ONLY:
                blt_dst_out = blt_dst_in;
                break;
            case BM_S_XOR_D:
                blt_dst_out = blt_src_out ^ blt_dst_in;
                break;
            case BM_S_OR_D:
                blt_dst_out = blt_src_out | blt_dst_in;
                break;
            case BM_NOT_SORD:
                blt_dst_out = ~blt_src_out & ~blt_dst_in;
                break;
            case BM_NOT_SXORD:
                blt_dst_out = ~blt_src_out ^ blt_dst_in;
                break;
            case BM_NOT_D:
                blt_dst_out = ~blt_dst_in;
                break;
            case BM_S_OR_NOTD:
                blt_dst_out = blt_src_out | ~blt_dst_in;
                break;
            case BM_NOT_S:
                blt_dst_out = ~blt_src_out;
                break;
            case BM_NOTS_OR_D:
                blt_dst_out = ~blt_src_out | blt_dst_in;
                break;
            case BM_NOT_SANDD:
                blt_dst_out = ~blt_src_out | ~blt_dst_in;
                break;
            case BM_ALL_BLACK:
                blt_dst_out = 0xffff;
                break;
            }

            /* and endmask */
            if (first) {
                mask_out = (blt_dst_out & blt->end_1) | (blt_dst_in & ~blt->end_1);
            } else if (last) {
                mask_out = (blt_dst_out & blt->end_3) | (blt_dst_in & ~blt->end_3);
            } else {
                mask_out = (blt_dst_out & blt->end_2) | (blt_dst_in & ~blt->end_2);
            }
            SetMemW (blt->dst_addr, mask_out);
            if (!last) {
                blt->dst_addr += blt->dst_x_inc;
            }
            first = 0;
        } while(--xc != 0);
        blt->status = (blt->status + ((blt->dst_y_inc >= 0) ? 1 : 15)) & 0xef;
        blt->src_addr += blt->src_y_inc;
        blt->dst_addr += blt->dst_y_inc;
    } while(--blt->y_cnt != 0);
    /* blt->status &= ~BUSY; */
}
#endif


#if CONF_WITH_BLITTER || !ASM_BLIT_IS_AVAILABLE
/*
 * bit_blt()
 *
 * Purpose:
 * Transfer a rectangular block of pixels located at an arbitrary X,Y
 * position in the source memory form to another arbitrary X,Y position
 * in the destination memory form, using replace mode (boolean operator 3).
 * This is used on ColdFire (where fast_bit_blt() is not available) or if
 * configuring with the blitter on a 68K system, since fast_bit_blt() does
 * not provide an interface to the hardware.
 *
 * In:
 *  blit_info   pointer to 34 byte input parameter block
 *
 * Note: This is a translation of the original assembler code in the Atari
 * blitter document, with the addition that source and destination are
 * allowed to overlap.  Original source code comments are mostly preserved.
 */

/* I n p u t   p a r a m e t e r   b l o c k   o f f s e t s */

#define SRC_FORM  0 /* Base address of source memory form .l: */
#define SRC_NXWD  4 /* Offset between words in source plane .w: */
#define SRC_NXLN  6 /* Source form width .w: */
#define SRC_NXPL  8 /* Offset between source planes .w: */
#define SRC_XMIN 10 /* Source blt rectangle minimum X .w: */
#define SRC_YMIN 12 /* Source blt rectangle minimum Y .w: */

#define DST_FORM 14 /* Base address of destination memory form .l: */
#define DST_NXWD 18 /* Offset between words in destination plane.w: */
#define DST_NXLN 20 /* Destination form width .w: */
#define DST_NXPL 22 /* Offset between destination planes .w: */
#define DST_XMIN 24 /* Destination blt rectangle minimum X .w: */
#define DST_YMIN 26 /* Destination blt rectangle minimum Y .w: */

#define WIDTH    28 /* Width of blt rectangle .w: */
#define HEIGHT   30 /* Height of blt rectangle .w: */
#define PLANES   32 /* Number of planes to blt .w: */

static void
bit_blt (void)
{
    WORD plane;
    UWORD s_xmin, s_xmax;
    UWORD d_xmin, d_xmax;
    UWORD lendmask, rendmask;
    WORD skew, skew_idx;
    WORD s_span, s_xmin_off, s_xmax_off;
    WORD d_span, d_xmin_off, d_xmax_off;
    ULONG s_addr, d_addr;
    blit blitter;

    /* a5-> BLiTTER register block */
    blit * blt = &blitter;

    /* Calculate Xmax coordinates from Xmin coordinates and width */
    s_xmin = blit_info->s_xmin;               /* d0<- src Xmin */
    s_xmax = s_xmin + blit_info->b_wd - 1;    /* d1<- src Xmax=src Xmin+width-1 */
    d_xmin = blit_info->d_xmin;               /* d2<- dst Xmin */
    d_xmax = d_xmin + blit_info->b_wd - 1;    /* d3<- dst Xmax=dstXmin+width-1 */

    /*
     * Skew value is (destination Xmin mod 16 - source Xmin mod 16) && 0x000F.
     * Three main discriminators are used to determine the states of the skew
     * flags (FXSR and NFSR):
     *
     * bit 0     0: Source Xmin mod 16 =< Destination Xmin mod 16
     *           1: Source Xmin mod 16 >  Destination Xmin mod 16
     *
     * bit 1     0: SrcXmax/16-SrcXmin/16 <> DstXmax/16-DstXmin/16
     *                       Source span      Destination span
     *           1: SrcXmax/16-SrcXmin/16 == DstXmax/16-DstXmin/16
     *
     * bit 2     0: Blit direction is from Right to Left
     *           1: Blit direction is from Left to Right
     *
     * These form an offset into a skew flag table yielding FXSR and NFSR flag
     * states for the given source and destination alignments.
     *
     * NOTE: this table lookup is overridden for the special case when both
     * the source & destination widths are one, and the skew is 0.  For this
     * case, the FXSR flag alone is always set.
     */

    skew_idx = 0x0000;                  /* default */

    s_xmin_off = s_xmin >> 4;           /* d0<- word offset to src Xmin */
    s_xmax_off = s_xmax >> 4;           /* d1<- word offset to src Xmax */
    s_span = s_xmax_off - s_xmin_off;   /* d1<- Src span - 1 */

    d_xmin_off = d_xmin >> 4;           /* d2<- word offset to dst Xmin */
    d_xmax_off = d_xmax >> 4;           /* d3<- word offset to dst Xmax */
    d_span = d_xmax_off - d_xmin_off;   /* d3<- dst span - 1 */

                                        /* the last discriminator is the */
    if ( d_span == s_span ) {           /* equality of src and dst spans */
        skew_idx |= 0x0002;             /* d6[bit1]:1 => equal spans */
    }

    /* d4<- number of words in dst line */
    blt->x_cnt = d_span + 1;            /* set value in BLiTTER */

    /* Endmasks derived from dst Xmin mod 16 and dst Xmax mod 16 */
    lendmask=0xffff>>(d_xmin%16);
    rendmask=~(0x7fff>>(d_xmax%16));

    /* d7<- Dst Xmin mod16 - Src Xmin mod16 */
    skew = (d_xmin & 0x0f) - (s_xmin & 0x0f);
    if (skew < 0 )
        skew_idx |= 0x0001;             /* d6[bit0]<- alignment flag */

    /* Calculate starting addresses */
    s_addr = (ULONG)blit_info->s_form
        + (ULONG)blit_info->s_ymin * (ULONG)blit_info->s_nxln
        + (ULONG)s_xmin_off * (ULONG)blit_info->s_nxwd;
    d_addr = (ULONG)blit_info->d_form
        + (ULONG)blit_info->d_ymin * (ULONG)blit_info->d_nxln
        + (ULONG)d_xmin_off * (ULONG)blit_info->d_nxwd;

    /* if (just_screen && (s_addr < d_addr)) { */
    if ((s_addr < d_addr)
     || ((s_addr == d_addr) && (skew >= 0))) {
        /* start from lower right corner, so add width+length */
        s_addr = (ULONG)blit_info->s_form
            + (ULONG)blit_info->s_ymax * (ULONG)blit_info->s_nxln
            + (ULONG)s_xmax_off * (ULONG)blit_info->s_nxwd;
        d_addr = (ULONG)blit_info->d_form
            + (ULONG)blit_info->d_ymax * (ULONG)blit_info->d_nxln
            + (ULONG)d_xmax_off * (ULONG)blit_info->d_nxwd;

        /* offset between consecutive words in planes */
        blt->src_x_inc = -blit_info->s_nxwd;
        blt->dst_x_inc = -blit_info->d_nxwd;

        /* offset from last word of a line to first word of next one */
        blt->src_y_inc = -(blit_info->s_nxln - blit_info->s_nxwd * s_span);
        blt->dst_y_inc = -(blit_info->d_nxln - blit_info->d_nxwd * d_span);

        blt->end_1 = rendmask;          /* first write mask */
        blt->end_2 = 0xFFFF;            /* center mask */
        blt->end_3 = lendmask;          /* last write mask */
    }
    else {
        /* offset between consecutive words in planes */
        blt->src_x_inc = blit_info->s_nxwd;
        blt->dst_x_inc = blit_info->d_nxwd;

        /* offset from last word of a line to first word of next one */
        blt->src_y_inc = blit_info->s_nxln - blit_info->s_nxwd * s_span;
        blt->dst_y_inc = blit_info->d_nxln - blit_info->d_nxwd * d_span;

        blt->end_1 = lendmask;          /* first write mask */
        blt->end_2 = 0xFFFF;            /* center mask */
        blt->end_3 = rendmask;          /* last write mask */

        skew_idx |= 0x0004;             /* blitting left->right */
    }

    /* does destination just span a single word? */
    if ( !d_span ) {
        /* merge both end masks into Endmask1. */
        blt->end_1 &= blt->end_3;       /* single word end mask */
        /* The other end masks will be ignored by the BLiTTER */
    }

    /*
     * Set up the skew byte, which contains the FXSR/NFSR flags and the
     * skew value.  The skew value is the low nybble of the difference
     * in Source and Destination alignment.
     *
     * The main complication is setting the FXSR/NFSR flags.  Normally
     * we use the calculated skew_idx to obtain them from the skew_flags[]
     * array.  However, when the source and destination widths are both 1,
     * we do not set either flag unless the skew value is zero, in which
     * case we set the FXSR flag only.  Additionally, we must set the skew
     * direction in source x incr.
     *
     * Thank you blitter hardware designers ...
     */
    if (!s_span && !d_span) {
        blt->src_x_inc = skew;          /* sets skew direction */
        blt->skew = skew ? (skew & 0x0f) : mSkewFXSR;
    } else {
        blt->skew = (skew & 0x0f) | skew_flags[skew_idx];
    }

    /* BLiTTER REGISTER MASKS */
#define mHOP_Source  0x02
#define mHOP_Halftone 0x01
    blt->hop = mHOP_Source;   /* word */    /* set HOP to source only */

    for (plane = blit_info->plane_ct-1; plane >= 0; plane--) {
        int op_tabidx;

        blt->src_addr = s_addr;         /* load Source pointer to this plane */
        blt->dst_addr = d_addr;         /* load Dest ptr to this plane   */
        blt->y_cnt = blit_info->b_ht;   /* load the line count   */

        /* calculate operation for actual plane */
        op_tabidx = ((blit_info->fg_col>>plane) & 0x0001 ) <<1;
        op_tabidx |= (blit_info->bg_col>>plane) & 0x0001;
        blt->op = blit_info->op_tab[op_tabidx] & 0x000f;

        /*
         * We can only be here if either:
         * (a) we are on ColdFire (ASM_BLIT_IS_AVAILABLE is 0): the
         *     hardware blitter may or may not be enabled, or
         * (b) we are on 68K (ASM_BLIT_IS_AVAILABLE is 1): the
         *     hardware blitter must be enabled to get here.
         */
#if !ASM_BLIT_IS_AVAILABLE
#if CONF_WITH_BLITTER
        if (blitter_is_enabled)
        {
            blitter_do_blit(blt);
        }
        else
#endif
        {
            do_blit(blt);
        }
#else
        blitter_do_blit(blt);
#endif

        s_addr += blit_info->s_nxpl;          /* a0-> start of next src plane   */
        d_addr += blit_info->d_nxpl;          /* a1-> start of next dst plane   */
    }
}
#endif


/* common settings needed both by VDI and line-A raster
 * operations, but being given through different means.
 */
struct raster_t {
    VwkClip *clipper;
    int clip;
    int multifill;
    int transparent;
};

/*
 * setup_pattern - if bit 5 of mode is set, use pattern with blit
 */
static void
setup_pattern (struct raster_t *raster, struct blit_frame *info)
{
    /* multi-plane pattern? */
    info->p_nxpl = 0;           /* next plane pattern offset default. */
    if (raster->multifill) {
        info->p_nxpl = 32;      /* yes, next plane pat offset = 32. */
    }
    info->p_nxln = 2;        /* offset to next line in pattern */
    info->p_mask = 0xf;      /* pattern index mask */
}

/*
 * do_clip - clip, if dest is screen and clipping is wanted
 *
 * return TRUE, if clipping took away everything
 */
/* not fully optimized yet*/
static BOOL
do_clip (VwkClip *clipper, struct blit_frame *info)
{
    WORD s_xmin, s_ymin;
    WORD d_xmin, d_ymin;
    WORD d_xmax, d_ymax;
    WORD deltax, deltay, clip;

    /* clip Xmin source and destination to window */
    s_xmin = PTSIN[XMIN_S];
    d_xmin = PTSIN[XMIN_D];
    clip = clipper->xmn_clip;

    /* Xmin dest < Xmin clip */
    if ( d_xmin < clip ) {    /* Xmin dest > Xmin clip => branch */
        s_xmin -= d_xmin - clip;    /* subtract amount clipped in x */
        d_xmin = clip;               /* clip Xmin dest */
    }
    info->s_xmin = s_xmin;      /* d0 <- clipped Xmin source */
    info->d_xmin = d_xmin;      /* d2 <- clipped Xmin destination */

    /* clip Xmax destination to window */
    d_xmax = PTSIN[XMAX_S] - s_xmin + d_xmin;
    clip = clipper->xmx_clip;

    /* Xmax dest > Xmax clip */
    if ( d_xmax > clip )
        d_xmax = clip;          /* clip Xmax dest */
    info->d_xmax = d_xmax;

    /* match source and destination rectangles */
    deltax = d_xmax - d_xmin;
    if ( deltax < 0 )
        return TRUE;                    /* block entirely clipped */
    info->b_wd = deltax + 1;
    info->s_xmax = s_xmin + deltax;     /* d4 <- Xmax Source */

    /* clip Ymin source and destination to window */
    s_ymin = PTSIN[YMIN_S];
    d_ymin = PTSIN[YMIN_D];
    clip = clipper->ymn_clip;

    /* Ymin dest < Ymin clip => clip Ymin */
    if ( d_ymin < clip ) {
        s_ymin -= d_ymin - clip;    /* subtract amount clipped in y */
        d_ymin = clip;               /* clip Ymin dest */
    }
    info->s_ymin = s_ymin;       /* d1, Dy Source */
    info->d_ymin = d_ymin;       /* d3, Ymax destination */

    /* clip Ymax destination to window */
    d_ymax = PTSIN[YMAX_S] - s_ymin + d_ymin;
    clip = clipper->ymx_clip;

    /* if Ymax dest > Ymax clip */
    if ( d_ymax > clip ) {
        /* clip Ymax dest */
        d_ymax = clip;          /* d7 <- Xmax dest = Xmax clip */
    }
    info->d_ymax = d_ymax;

    /* match source and destination rectangles */
    deltay = d_ymax - d_ymin;
    if ( deltay < 0 )
        return TRUE;             /* block entirely clipped */
    info->b_ht = deltay + 1;
    info->s_ymax = s_ymin + deltay;             /* d5 <- Ymax Source */

    return FALSE;
}

/*
 * dont_clip - clip, if dest is screen and clipping is wanted
 */
static void
dont_clip (struct blit_frame * info)
{
    /* source */
    info->s_xmin = PTSIN[XMIN_S];       /* d0 x of upper left of source */
    info->s_ymin = PTSIN[YMIN_S];       /* d1 y of upper left of source */
    info->s_xmax = PTSIN[XMAX_S];       /* d4 x of lower right of source */
    info->s_ymax = PTSIN[YMAX_S];       /* d5 y of lower right of source */

    /* width and height of block in pixels */
    info->b_wd = info->s_xmax - info->s_xmin + 1;
    info->b_ht = info->s_ymax - info->s_ymin + 1;

    /* destination */
    info->d_xmin = PTSIN[XMIN_D];       /* d2 x of upper left of dest. */
    info->d_ymin = PTSIN[YMIN_D];       /* d3 y of upper left of dest. */
    info->d_xmax = PTSIN[XMAX_D];       /* d6 x of lower right of dest. */
    info->d_ymax = PTSIN[YMAX_D];       /* d7 y of lower right of dest. */
}

/*
 * setup_info - fill the info structure with MFDB values
 */
static BOOL
setup_info (struct raster_t *raster, struct blit_frame * info)
{
    MFDB *src,*dst;
    BOOL use_clip = FALSE;

    /* Get the pointers to the MFDBs */
    src = *(MFDB **)&CONTRL[7]; /* a5, source MFDB */
    dst = *(MFDB **)&CONTRL[9]; /* a4, destination MFDB */

    /* setup plane info for source MFDB */
    if ( src->fd_addr ) {
        /* for a positive source address */
        info->s_form = src->fd_addr;
        info->s_nxwd = src->fd_nplanes * 2;
        info->s_nxln = src->fd_wdwidth * info->s_nxwd;
    }
    else {
        /* source form is screen */
        info->s_form = (UWORD*) v_bas_ad;
        info->s_nxwd = v_planes * 2;
        info->s_nxln = v_lin_wr;
    }

    /* setup plane info for destination MFDB */
    if ( dst->fd_addr ) {
        /* for a positive address */
        info->d_form = dst->fd_addr;
        info->plane_ct = dst->fd_nplanes;
        info->d_nxwd = dst->fd_nplanes * 2;
        info->d_nxln = dst->fd_wdwidth * info->d_nxwd;
    }
    else {
        /* destination form is screen */
        info->d_form = (UWORD*) v_bas_ad;
        info->plane_ct = v_planes;
        info->d_nxwd = v_planes * 2;
        info->d_nxln = v_lin_wr;

        /* check if clipping is enabled, when destination is screen */
        if (raster->clip)
            use_clip = TRUE;
    }

    if (use_clip) {
        if (do_clip(raster->clipper, info))
            return TRUE;        /* clipping took away everything */
    }
    else
        dont_clip(info);

    info->s_nxpl = 2;           /* next plane offset (source) */
    info->d_nxpl = 2;           /* next plane offset (destination) */

    /* only 8, 4, 2 and 1 planes are valid (destination) */
    return info->plane_ct & ~0x000f;
}

/* common functionality for vdi_vro_cpyfm, vdi_vrt_cpyfm, linea_raster */
static void
cpy_raster(struct raster_t *raster, struct blit_frame *info)
{
    WORD mode;
    WORD fg_col, bg_col;

    arb_corner((Rect*)PTSIN);
    arb_corner((Rect*)(PTSIN+4));
    mode = INTIN[0];

    /* if mode is made up of more than the first 5 bits */
    if (mode & ~0x001f)
        return;                 /* mode is invalid */

    /* check the pattern flag (bit 5) and revert to log op # */
    info->p_addr = NULL;        /* get pattern pointer */
    if (mode & PAT_FLAG) {
        mode &= ~PAT_FLAG;      /* set bit to 0! */
        setup_pattern(raster, info);   /* fill in pattern related stuff */
    }

    /* if true, the plane count is invalid or clipping took all! */
    if (setup_info(raster, info))
        return;

    if (!raster->transparent) {

        /* COPY RASTER OPAQUE */

        /* planes of source and destination equal in number? */
        if (info->s_nxwd != info->d_nxwd)
            return;

        info->op_tab[0] = mode; /* fg:0 bg:0 */
        info->bg_col = 0;       /* bg:0 & fg:0 => only first OP_TAB */
        info->fg_col = 0;       /* entry will be referenced */

    } else {

        /*
         * COPY RASTER TRANSPARENT - copies a monochrome raster area
         * from source form to a color area. A writing mode and color
         * indices for both 0's and 1's are specified in the INTIN array.
         */

        /* is source area one plane? */
        if (info->s_nxwd != 2)
                return;         /* source must be mono plane */

        info->s_nxpl = 0;       /* use only one plane of source */

        /* d6 <- background color */
        fg_col = INTIN[1];
        if ((fg_col >= DEV_TAB[13]) || (fg_col < 0))
            fg_col = 1;
        fg_col = MAP_COL[fg_col];

        /* d7 <- foreground color */
        bg_col = INTIN[2];
        if ((bg_col >= DEV_TAB[13]) || (bg_col < 0))
            bg_col = 1;
        bg_col = MAP_COL[bg_col];

        switch(mode) {
        case MD_TRANS:
            info->op_tab[0] = 04;    /* fg:0 bg:0  D' <- [not S] and D */
            info->op_tab[2] = 07;    /* fg:1 bg:0  D' <- S or D */
            info->fg_col = fg_col;   /* were only interested in one color */
            info->bg_col = 0;        /* save the color of interest */
            break;

        case MD_REPLACE:
            /* CHECK: bug, that colors are reversed? */
            info->op_tab[0] = 00;    /* fg:0 bg:0  D' <- 0 */
            info->op_tab[1] = 12;    /* fg:0 bg:1  D' <- not S */
            info->op_tab[2] = 03;    /* fg:1 bg:0  D' <- S */
            info->op_tab[3] = 15;    /* fg:1 bg:1  D' <- 1 */
            info->bg_col = bg_col;   /* save fore and background colors */
            info->fg_col = fg_col;
            break;

        case MD_XOR:
            info->op_tab[0] = 06;    /* fg:0 bg:0  D' <- S xor D */
            info->bg_col = 0;
            info->fg_col = 0;
            break;

        case MD_ERASE:
            info->op_tab[0] = 01;    /* fg:0 bg:0  D' <- S and D */
            info->op_tab[1] = 13;    /* fg:0 bg:1  D' <- [not S] or D */
            info->fg_col = 0;        /* were only interested in one color */
            info->bg_col = bg_col;   /* save the color of interest */
            break;

        default:
            return;                     /* unsupported mode */
        }
    }

    /*
     * call assembler blit routine or C-implementation.  we call the
     * assembler version if we're not on ColdFire and either
     * (a) the blitter isn't configured, or
     * (b) it's configured but not available.
     */
    blit_info = info;

#if ASM_BLIT_IS_AVAILABLE
#if CONF_WITH_BLITTER
    if (blitter_is_enabled)
    {
        bit_blt();
    }
    else
#endif
    {
        fast_bit_blt();
    }
#else
    bit_blt();
#endif
}

/*
 * vdi_vro_cpyfm - copy raster opaque
 *
 * This function copies a rectangular raster area from source form to
 * destination form using the logic operation specified by the application.
 */
void
vdi_vro_cpyfm(Vwk * vwk)
{
    struct raster_t raster;

    vdi_info.p_addr = vwk->patptr;

    raster.clipper = VDI_CLIP(vwk);
    raster.clip = vwk->clip;
    raster.multifill = vwk->multifill;
    raster.transparent = 0;

    cpy_raster(&raster, &vdi_info);
}

/*
 * vdi_vrt_cpyfm - copy raster transparent
 *
 * This function copies a monochrome raster area from source form to a
 * color area. A writing mode and color indices for both 0's and 1's
 * are specified in the INTIN array.
 */
void
vdi_vrt_cpyfm(Vwk * vwk)
{
    struct raster_t raster;

    vdi_info.p_addr = vwk->patptr;

    raster.clipper = VDI_CLIP(vwk);
    raster.clip = vwk->clip;
    raster.multifill = vwk->multifill;
    raster.transparent = 1;

    cpy_raster(&raster, &vdi_info);
}

/* line-A wrapper for Copy raster form */
void linea_raster(void)
{
    struct raster_t raster;

    raster.clipper = NULL;
    raster.clip = 0;
    raster.multifill = MFILL;
    raster.transparent = COPYTRAN;

    cpy_raster(&raster, &vdi_info);
}

/* line-A wrapper for blitting */
void linea_blit(struct blit_frame *info)
{
    /* with line-A, need to calculate these for bit_blt()
     * (whereas VDI needs to calculate wd & ht)
     */
    info->s_xmax = info->s_xmin + info->b_wd - 1;
    info->s_ymax = info->s_ymin + info->b_ht - 1;
    info->d_xmax = info->d_xmin + info->b_wd - 1;
    info->d_ymax = info->d_ymin + info->b_ht - 1;
    blit_info = info;

    /*
     * call assembler blit routine or C-implementation.  we call the
     * assembler version if we're not on ColdFire and either
     * (a) the blitter isn't configured, or
     * (b) it's configured but not available.
     */
#if ASM_BLIT_IS_AVAILABLE
#if CONF_WITH_BLITTER
    if (blitter_is_enabled)
    {
        bit_blt();
    }
    else
#endif
    {
        fast_bit_blt();
    }
#else
    bit_blt();
#endif
}
