/*
 * blit.c - Blitting routines
 *
 * Copyright 2003 The EmuTOS development team
 * Copyright 2002 Joachim Hoenig (blitter)
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "vdidef.h"
#include "lineavars.h"
#include "gsxextrn.h"
#include "tosvars.h"
#include "mouse.h"

#define DBG_BLIT 0
#define C_BLIT 1        // cpy_fm routine in C
#define BLITTER_IN_C 1  // bit_blt routine in C

#if DBG_BLIT
#include "kprint.h"
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
#define PAT_FLAG	4



/* prototypes */
static void do_blit(void);

/* passes parameters to bitblt */
struct blit_frame {
    UWORD b_wd;		// +00 width of block in pixels
    UWORD b_ht;		// +02 height of block in pixels
    UWORD plane_ct;     // +04 number of consequitive planes to blt
    UWORD fg_col;       // +06 foreground color (logic op table index:hi bit)
    UWORD bg_col;       // +08 background color (logic op table index:lo bit)
    UBYTE op_tab[4];    // +10 logic ops for all fore and background combos
    UWORD s_xmin;       // +14 minimum X: source
    UWORD s_ymin;       // +16 minimum Y: source
    UWORD * s_form;     // +18 source form base address
    UWORD s_nxwd;       // +22 offset to next word in line  (in bytes)
    UWORD s_nxln;       // +24 offset to next line in plane (in bytes)
    UWORD s_nxpl;       // +26 offset to next plane from start of current plane
    UWORD d_xmin;       // +28 minimum X: destination
    UWORD d_ymin;       // +30 minimum Y: destination
    UWORD * d_form;     // +32 destination form base address
    UWORD d_nxwd;       // +36 offset to next word in line  (in bytes)
    UWORD d_nxln;       // +38 offset to next line in plane (in bytes)
    UWORD d_nxpl;       // +40 offset to next plane from start of current plane
    UWORD * p_addr;     // +42 address of pattern buffer   (0:no pattern)
    UWORD p_nxln;       // +46 offset to next line in pattern  (in bytes)
    UWORD p_nxpl;       // +48 offset to next plane in pattern (in bytes)
    UWORD p_mask;       // +50 pattern index mask

    /* these frame parameters are internally set */
    UWORD p_indx;       // +52 initial pattern index
    UWORD * s_addr;     // +54 initial source address
    UWORD s_xmax;       // +58 maximum X: source
    UWORD s_ymax;       // +60 maximum Y: source
    UWORD * d_addr;     // +62 initial destination address
    UWORD d_xmax;       // +66 maximum X: destination
    UWORD d_ymax;       // +68 maximum Y: destination
    UWORD inner_ct;     // +70 blt inner loop initial count
    UWORD dst_wr;       // +72 destination form wrap (in bytes)
    UWORD src_wr;       // +74 source form wrap (in bytes)
};

/*
 * This struct has a global scope. It should just be put on a stack frame by the
 * cpyfm function and then passed to the bit_blt routine, but I did not manage
 * to get it that way going. Maybe, the optimizer plays a role?!?
 */
struct blit_frame info;     /* holds some internal info for bit_blt */

/*
 * cpy_fm - general bitblt operation, using 1 of 16 logical operations.
 *
 * device dependent format only.
 *
 * copytran - Flag for Copy-raster-form (<>0 = Transparent)
 */
void cpyfm(UWORD copytran)
{
    WORD mode;
    MFDB *src,*dst;

    mode = INTIN[0];
    if ( mode < 0 )
        return;                 /* mode is invalid */

    /* check the pattern flag and revert to log op # */
    if ( mode & PAT_FLAG )   	/* and set bit to 0! */ {
        mode &= ~PAT_FLAG;      /* split pattern flag from mode */

        /* multi-plane pattern? */
        info.p_nxpl = 0;	/* next plane pattern offset default. */
        if ( multifill ) {
            info.p_nxpl = 32;	/* yes, next plane pat offset = 32. */
        }
        info.p_nxln = 2;        /* offset to next line in pattern */
        info.p_mask = 0xf;      /* pattern index mask */
    }
    /* check user write (write mode #5 (D' = D) => do nothing */
    if ( mode == 5 || mode > 15)
        return;                 /* mode is invalid */

    info.p_addr = patptr;	/* get pattern pointer*/
    info.s_nxpl = 2;		/* d4 <- next plane offset (source) */
    info.d_nxpl = 2;		/* d5 <- next plane offset (destination) */

    /* Get the pointers to the MFDBs */
    src = *(MFDB **)&CONTRL[7]; /* a5, source MFDB */
    dst = *(MFDB **)&CONTRL[9]; /* a4, destination MFDB */

    /* setup planes for source MFDB */
    if ( src->fd_addr ) {
        /* for a positive source address */
        info.s_form = src->fd_addr;
        info.s_nxwd = src->fd_nplanes * 2;
        info.s_nxln = src->fd_wdwidth * 2 * src->fd_nplanes;
    }
    else {
        /* source address == NULL */
        info.s_form = (UWORD*) v_bas_ad;		/* source form is screen */
        info.s_nxwd = v_planes * 2;
        info.s_nxln = v_lin_wr;
    }

    /* setup planes for destination MFDB */
    if ( dst->fd_addr ) {
        /* for a positive address */
        info.d_form = dst->fd_addr;
        info.plane_ct = dst->fd_nplanes;
        info.d_nxwd = dst->fd_nplanes * 2;
        info.d_nxln = dst->fd_wdwidth * 2 * dst->fd_nplanes;
    }
    else {
        /* source address == NULL */
        info.d_form = (UWORD*) v_bas_ad;		/* source form is screen */
        info.plane_ct = v_planes;
        info.d_nxwd = v_planes * 2;
        info.d_nxln = v_lin_wr;
    }

    /* only 4,2, and 1 planes are valid (destination) */
    if ( info.plane_ct & ~0x0007 )
        return;

    if ( copytran ) {
        /*
         * COPY RASTER TRANSPARENT - copies a monochrome raster area
         * from source form to a color area. A writing mode and color
         * indices for both 0's and 1's are specified in the INTIN array.
         */
        WORD fg, bg;

        /* is source area one plane? */
        if ( info.s_nxwd != 2 )
            return;    /* source must be mono plane */
        info.s_nxpl = 0;		/* use only the first plane for the source */

        /* d6 <- background color */
        fg = INTIN[1];
        if ((fg >= DEV_TAB[13]) || (fg < 0))
            fg = 1;
        fg = MAP_COL[fg];

        /* d7 <- foreground color */
        bg = INTIN[2];
        if ((bg >= DEV_TAB[13]) || (bg < 0))
            bg = 1;
        bg = MAP_COL[bg];

        /* mode == d2 */
        switch(mode) {
        case MD_TRANS:
            info.op_tab[0] = 04;	/* fg:0 bg:0  D' <- [not S] and D */
            info.op_tab[2] = 07;	/* fg:1 bg:0  D' <- S or D */
            info.fg_col = fg;		/* were only interested in one color */
            info.bg_col = 0;		/* save the color of interest */
            break;

        case MD_REPLACE:
            /* CHECK: bug, that colors are reversed? */
            info.op_tab[0] = 00;	/* fg:0 bg:0  D' <- 0 */
            info.op_tab[1] = 12;	/* fg:0 bg:1  D' <- not S */
            info.op_tab[2] = 03;	/* fg:1 bg:0  D' <- S */
            info.op_tab[3] = 15;	/* fg:1 bg:1  D' <- 1 */
            info.bg_col = bg;		/* save fore and background colors */
            info.fg_col = fg;
            break;

        case MD_XOR:
            info.op_tab[0] = 06;	/* fg:0 bg:0  D' <- S xor D */
            info.bg_col = 0;
            info.fg_col = 0;
            break;

        case MD_ERASE:
            info.op_tab[0] = 01;	/* fg:0 bg:0  D' <- S and D */
            info.op_tab[1] = 13;	/* fg:0 bg:1  D' <- [not S] or D */
            info.fg_col = 0;		/* were only interested in one color */
            info.bg_col = bg;		/* save the color of interest */
            break;

        default:
            return;                     /* unsupported mode */
        }
    }
    else {
        /*
         * COPY RASTER OPAQUE - copies a rectangular raster area
         * from source form to destination form using the logic operation
         * specified by the application.
         */

        /* do the standard logic operations */

        /* planes of source and destination must be equal in number */
        if ( info.s_nxwd != info.d_nxwd )
            return;

        info.op_tab[0] = mode;		/* fg:0 bg:0 */
        info.bg_col = 0;                /* bg:0 & fg:0 => only first OP_TAB */
        info.fg_col = 0;                /* entry will be referenced */
    }


    /* PTSIN ARRAY OFFSETs */

#define XMIN_S	0	// x of upper left of source rectangle
#define YMIN_S  1       // y of upper left of source rectangle
#define XMAX_S  2       // x of lower right of source rectangle
#define YMAX_S  3       // y of lower right of source rectangle

#define XMIN_D  4       // x of upper left of destination rectangle
#define YMIN_D  5       // y of upper left of destination rectangle
#define XMAX_D  6       // x of lower right of destination rectangle
#define YMAX_D  7       // y of lower right of destination rectangle

    /* check if clipping is enabled and destination is the screen */
    if (CLIP && !dst->fd_addr) {
        int s_xmin, s_ymin;
        int d_xmin, d_ymin;
        int d_xmax, d_ymax;
        int clip, deltax, deltay;

        /*
         * clip Xmin source and destination to window
         */
        s_xmin = PTSIN[XMIN_S];
        d_xmin = PTSIN[XMIN_D];
        clip = XMN_CLIP;

        /* Xmin dest < Xmin clip */
        if ( d_xmin < clip ) {    /* Xmin dest > Xmin clip => branch */
            /* clip Xmin dest */
            int tmp = clip;
            clip = d_xmin;
            d_xmin = tmp;		/* d2 <- Xmin dest = Xmin clip */

            clip -= d_xmin;		/* d4 <- -(amount clipped in x) */
            s_xmin -= clip;		/* d0 <- adjusted Xmin src */
        }
        info.s_xmin = s_xmin;	/* d0 <- clipped Xmin source */
        info.d_xmin = d_xmin;	/* d2 <- clipped Xmin destination */


        /*
         * clip Xmax destination to window
         */
        d_xmax = PTSIN[XMAX_S] - s_xmin + d_xmin;
        clip = XMX_CLIP;

        /* Xmax dest > Xmax clip */
        if ( d_xmax > clip ) {
            /* clip Xmax dest */
            int tmp = clip;
            clip = d_xmax;
            d_xmax = tmp;		/* d6 <- Xmax dest = Xmax clip */
        }
        info.d_xmax = d_xmax;

        /* match source and destination rectangles */
        deltax = d_xmax - d_xmin;
        if ( deltax < 0 )
            return;             /* block entirely clipped */
        info.b_wd = deltax + 1;
        info.s_xmax = s_xmin + deltax;		/* d4 <- Xmax Source */


        /*
         * clip Ymin source and destination to window
         */
        s_ymin = PTSIN[YMIN_S];
        d_ymin = PTSIN[YMIN_D];
        clip = YMN_CLIP;

        /* Ymin dest < Ymin clip => clip Ymin */
        if ( d_ymin < clip ) {
            int tmp = clip;
            clip = d_ymin;
            d_ymin = tmp;		/* d1 <- Ymin dest = Ymin clip */

            clip -= d_ymin;		/* d4 <- -(amount clipped in Y) */
            s_ymin -= clip;		/* d0 <- adjusted source Ymin  */
        }
        info.s_ymin = s_ymin;       /* d1, Dy Source */
        info.d_ymin = d_ymin;       /* d3, Ymax destination */


        /*
         * clip Ymax destination to window
         */
        d_ymax = PTSIN[YMAX_S] - s_ymin + d_ymin;
        clip = YMX_CLIP;

        /* if Ymax dest > Ymax clip */
        if ( d_ymax > clip ) {
            /* clip Ymax dest */
            int tmp = clip;
            clip = d_ymax;
            d_ymax = tmp;		/* d7 <- Xmax dest = Xmax clip */
        }
        info.d_ymax = d_ymax;

        /* match source and destination rectangles */
        deltay = d_ymax - d_ymin;
        if ( deltay < 0 )
            return;             /* block entirely clipped */
        info.b_ht = deltay + 1;
        info.s_ymax = s_ymin + deltay;		/* d5 <- Ymax Source */
    }
    else {
        /* don't clip */

        /* source */
        info.s_xmin = PTSIN[XMIN_S];	/* d0 x of upper left of source */
        info.s_ymin = PTSIN[YMIN_S]; 	/* d1 y of upper left of source */
        info.s_xmax = PTSIN[XMAX_S];	/* d4 x of lower right of source */
        info.s_ymax = PTSIN[YMAX_S]; 	/* d5 y of lower right of source */

        /* width and height of block in pixels */
        info.b_wd = info.s_xmax - info.s_xmin + 1;
        info.b_ht = info.s_ymax - info.s_ymin + 1;

        /* destination */
        info.d_xmin = PTSIN[XMIN_D];	/* d2 x of upper left of dest. */
        info.d_ymin = PTSIN[YMIN_D]; 	/* d3 y of upper left of dest. */
        info.d_xmax = PTSIN[XMAX_D];	/* d6 x of lower right of dest. */
        info.d_ymax = PTSIN[YMAX_D]; 	/* d7 y of lower right of dest. */
    }

#if DBG_BLIT
    kprintf("\n");
    kprintf("s_xmin = %d\n", info.s_xmin);
    kprintf("s_xmax = %d\n", info.s_xmax);
    kprintf("s_ymin = %d\n", info.s_ymin);
    kprintf("s_ymax = %d\n", info.s_ymax);
    kprintf("\n");
    kprintf("d_xmin = %d\n", info.d_xmin);
    kprintf("d_xmax = %d\n", info.d_xmax);
    kprintf("d_ymin = %d\n", info.d_ymin);
    kprintf("d_ymax = %d\n", info.d_ymax);
    kprintf("\n");
    kprintf("b_wd = %d\n", info.b_wd);
    kprintf("b_ht = %d\n", info.b_ht);
    kprintf("s_form = %p\n", info.s_form);
    kprintf("d_form = %p\n", info.d_form);

    kprintf("\n");
    kprintf("plane_ct = %d\n", info.plane_ct);
    kprintf("s_nxwd = %d\n", info.s_nxwd);
    kprintf("s_nxln = %d\n", info.s_nxln);
    kprintf("d_nxwd = %d\n", info.d_nxwd);
    kprintf("d_nxln = %d\n", info.d_nxln);
#endif
    /* for now call assembly routine */
    bit_blt();
}


/*
 * vdi_vro_cpyfm - copy raster opaque
 *
 * This function copies a rectangular raster area from source form to
 * destination form using the logic operation specified by the application.
 */
void vdi_vro_cpyfm()
{
    arb_corner(PTSIN, ULLR);
    arb_corner((PTSIN + 4), ULLR);
#if C_BLIT
    cpyfm(0);
#else
    /* this needs copyrfm.S in Makefile */
    COPYTRAN = 0;
    COPY_RFM();
#endif
}



/*
 * vdi_vrt_cpyfm - copy raster transparent
 *
 * This function copies a monochrome raster area from source form to a
 * color area. A writing mode and color indices for both 0's and 1's
 * are specified in the INTIN array.
 */

void vdi_vrt_cpyfm()
{
    /* transparent blit */
    arb_corner(PTSIN, ULLR);
    arb_corner((PTSIN + 4), ULLR);
#if C_BLIT
    cpyfm(1);
#else
    /* this needs copyrfm.S in Makefile */
    COPYTRAN = 0xFFFF;
    COPY_RFM();
#endif
}



/*
 * vr_trnfm - Convert bitmaps
 *
 * Convert device independant bitmaps into device dependants and vice versa.
 *
 * The major difference is, that in the device independant format the planes
 * are consecutive, while on the screen they are interleaved.
 */
void vr_trnfm()
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






#if BLITTER_IN_C

/* BLiTTER BASE ADDRESS */
//#define BLiTTER 0xFF8A00:

    /* BLiTTER REGISTER FLAGS */
#define fHOP_Source    1
#define fHOP_Halftone  0

#define fSkewFXSR  7
#define fSkewNFSR  6

#define fLineBusy  7
#define fLineHog  6
#define fLineSmudge  5

#define mLineBusy    0x80
#define mLineHog     0x40
#define mLineSmudge  0x20

/*
 * endmask data
 *
 * a bit means:
 *
 *   0: Destination
 *   1: Source <<< Invert right end mask data >>>
 */

/* TiTLE: BLiT_iT */

/* PuRPoSE: */
/* Transfer a rectangular block of pixels located at an */
/* arbitrary X,Y position in the source memory form to */
/* another arbitrary X,Y position in the destination memory */
/* form using replace mode (boolean operator 3). */
/* The source and destination rectangles should not overlap. */

/* iN: */
/* a4 pointer to 34 byte input parameter block */

/* Note: This routine must be executed in supervisor mode as */
/* access is made to hardware registers in the protected region */
/* of the memory map. */


/* I n p u t p a r a m e t e r b l o c k o f f s e t s */

#define SRC_FORM  0 // Base address of source memory form .l:
#define SRC_NXWD  4 // Offset between words in source plane .w:
#define SRC_NXLN  6 // Source form width .w:
#define SRC_NXPL  8 // Offset between source planes .w:
#define SRC_XMIN 10 // Source blt rectangle minimum X .w:
#define SRC_YMIN 12 // Source blt rectangle minimum Y .w:

#define DST_FORM 14 // Base address of destination memory form .l:
#define DST_NXWD 18 // Offset between words in destination plane.w:
#define DST_NXLN 20 // Destination form width .w:
#define DST_NXPL 22 // Offset between destination planes .w:
#define DST_XMIN 24 // Destination blt rectangle minimum X .w:
#define DST_YMIN 26 // Destination blt rectangle minimum Y .w:

#define WIDTH    28 // Width of blt rectangle .w:
#define HEIGHT   30 // Height of blt rectangle .w:
#define PLANES   32 // Number of planes to blt .w:

/* BLiTTER REGISTER OFFSETS - not neede here */
#if 0
#define Halftone  0:
#define Src_Xinc 32:
#define Src_Yinc 34:
#define Src_Addr 36:
#define Endmask1 40:
#define Endmask2 42:
#define Endmask3 44:
#define Dst_Xinc 46:
#define Dst_Yinc 48:
#define Dst_Addr 50:
#define X_Count  54:
#define Y_Count  56:
#define HOP      58:
#define OP       59:
#define Line_Num 60:
#define Skew     61:
#endif

/* blitter registers */
UWORD          blt_halftone[16];
WORD           blt_src_x_inc, blt_src_y_inc;
ULONG          blt_src_addr;
WORD           blt_end_1, blt_end_2, blt_end_3;
WORD           blt_dst_x_inc, blt_dst_y_inc;
ULONG          blt_dst_addr;
UWORD          blt_x_cnt, blt_y_cnt;
BYTE           blt_hop, blt_op, blt_status, blt_skew;
BYTE           blt_ready;

void bit_blt ()
{
    WORD plane;
    WORD s_xmin, s_xmax;
    WORD d_xmin, d_xmax;
    UWORD lendmask, rendmask;
    WORD skew, skew_idx;
    WORD s_span, s_xmin_off, s_xmax_off;
    WORD d_span, d_xmin_off, d_xmax_off;
    ULONG s_addr, d_addr;

    /* setting of skew flags */

    /* QUALIFIERS   ACTIONS   BITBLT DIRECTION: LEFT -> RIGHT */

    /* equal Sx&F> */
    /* spans Dx&F FXSR NFSR */

    /* 0     0     0    1 |..ssssssssssssss|ssssssssssssss..|   */
    /*   |......dddddddddd|dddddddddddddddd|dd..............|   */

    /* 0     1      1  0 */
    /*   |......ssssssssss|ssssssssssssssss|ss..............|   */
    /*   |..dddddddddddddd|dddddddddddddd..|   */

    /* 1     0     0    0 |..ssssssssssssss|ssssssssssssss..|   */
    /*   |...ddddddddddddd|ddddddddddddddd.|   */

    /* 1     1     1    1 |...sssssssssssss|sssssssssssssss.|   */
    /*   |..dddddddddddddd|dddddddddddddd..|   */

#define mSkewFXSR    0x80
#define mSkewNFSR    0x40

    const UBYTE skew_flags [8] = {
        mSkewNFSR,		/* Source span < Destination span */
        mSkewFXSR,		/* Source span > Destination span */
        0,			/* Spans equal Shift Source right */
	mSkewNFSR+mSkewFXSR,    /* Spans equal Shift Source left */

        /* When Destination span is but a single word ... */
        0,			/* Implies a Source span of no words */
        mSkewFXSR,		/* Source span of two words */
        0,			/* Skew flags aren't set if Source and */
        0			/* Destination spans are both one word */
    };

    //a5 = BLiTTER;   /* word */    /* a5-> BLiTTER register block */

    /* Calculate Xmax coordinates from Xmin coordinates and width */
    s_xmin = info.s_xmin;		/* d0<- src Xmin */
    s_xmax = s_xmin + info.b_wd - 1;	/* d1<- src Xmax=src Xmin+width-1 */
    d_xmin = info.d_xmin;		/* d2<- dst Xmin */
    d_xmax = d_xmin + info.b_wd - 1;	/* d3<- dst Xmax=dstXmin+width-1 */

    /* Endmasks derived from source Xmin mod 16 and source Xmax mod 16 */
    lendmask=0xffff>>(d_xmin%16);
    rendmask=~(0x7fff>>(d_xmax%16));

    /* Skew value is (destination Xmin mod 16 - source Xmin mod 16) */
    /* && 0x000F.  Three discriminators are used to determine the */
    /* states of FXSR and NFSR flags: */

    /* bit 0     0: Source Xmin mod 16 =< Destination Xmin mod 16 */
    /* 1: Source Xmin mod 16 >  Destination Xmin mod 16 */

    /* bit 1     0: SrcXmax/16-SrcXmin/16 <> DstXmax/16-DstXmin/16 */
    /* Source span      Destination span */
    /* 1: SrcXmax/16-SrcXmin/16 == DstXmax/16-DstXmin/16 */

    /* bit 2     0: multiple word Destination span */
    /* 1: single word Destination span */

    /* These flags form an offset into a skew flag table yielding */
    /* correct FXSR and NFSR flag states for the given source and */
    /* destination alignments */

    /* d7<- Dst Xmin mod16 - Src Xmin mod16 */
    skew = (d_xmin & 0x0f) - (s_xmin & 0x0f);

    /* if Sx&F > Dx&F then cy:1 else cy:0 */
    skew_idx = (skew < 0) ? 0x0001 : 0x0000; /* d6[bit0]<- alignment flag */

    s_xmin_off = s_xmin >> 4;		/* d0<- word offset to src Xmin */
    s_xmax_off = s_xmax >> 4;		/* d1<- word offset to src Xmax */
    s_span = s_xmax_off - s_xmin_off;   /* d1<- Src span - 1 */

    d_xmin_off = d_xmin >> 4;		/* d2<- word offset to dst Xmin */
    d_xmax_off = d_xmax >> 4;		/* d3<- word offset to dst Xmax */
    d_span = d_xmax_off - d_xmin_off;   /* d3<- dst span - 1 */

    /* does destination just span a single word? */
    if ( !d_span ) {
        /* merge both end masks into Endmask1. */
        lendmask &= rendmask;		/* d4<- single word end mask */
        skew_idx |= 0x0004;			/* d6[bit2]:1 => single word dst */
        /* The other end masks will be ignored by the BLiTTER */
    }

    blt_end_1 = lendmask;		/* left end mask */
    blt_end_2 = 0xFFFF;			/* center end mask */
    blt_end_3 = rendmask;		/* right end mask */

    /* the last discriminator is the */
    if ( d_span == s_span ) {    	/* equality of src and dst spans */
        skew_idx |= 0x0002;   		/* d6[bit1]:1 => equal spans */
    }

    /* d4<- number of words in dst line */
    blt_x_cnt = d_span + 1;		/* set value in BLiTTER */

    /* Calculate Source starting address */
    s_addr = (ULONG)info.s_form
        + info.s_ymin * info.s_nxln
        + s_xmin_off * info.s_nxwd;

    /* d4<- offset between consecutive words in Src plane */
    blt_src_x_inc = info.s_nxwd;

    /* Src_Yinc is the offset in bytes from the last word of one Source */
    /* line to the first word of the next Source line */
    blt_src_y_inc = info.s_nxln - info.s_nxwd * s_span;


    /* Calculate Destination starting address */
    d_addr = (ULONG)info.d_form
        + info.d_ymin * info.d_nxln
        + d_xmin_off * info.d_nxwd;

    /* d4<- offset between consecutive words in Src plane */
    blt_dst_x_inc = info.d_nxwd;

    /* Dst_Yinc is the offset in bytes from the last word of one
     * Destination line to the first word of the next Dest. line */
    blt_dst_y_inc = info.d_nxln - info.d_nxwd * d_span;

    /*
     * The low nibble of the difference in Source and Destination alignment
     * is the skew value.  Use the skew flag index to reference FXSR and
     * NFSR states in skew flag table.
     */
    skew &= 0x0f;			/* d7<- isolated skew count */
    skew |= skew_flags[skew_idx];	/* d7<- necessary flags and skew */
    blt_skew = skew;			/* load Skew register   */

    /* BLiTTER REGISTER MASKS */
#define mHOP_Source  0x02
#define mHOP_Halftone 0x01
    blt_hop = mHOP_Source;   /* word */    /* set HOP to source only */

    for (plane = info.plane_ct-1; plane >= 0; plane--) {
        int op_tabidx;

        blt_src_addr = s_addr;		/* load Source pointer to this plane */
        blt_dst_addr = d_addr;		/* load Dest ptr to this plane   */
        blt_y_cnt = info.b_ht;		/* load the line count   */

        /* calculate operation for actual plane */
        op_tabidx = ((info.fg_col>>plane) & 0x0001 ) <<1;
        op_tabidx |= (info.bg_col>>plane) & 0x0001;
        blt_op = info.op_tab[op_tabidx] & 0x000f;

        do_blit();

        s_addr += info.s_nxpl;		/* a0-> start of next src plane   */
        d_addr += info.d_nxpl;		/* a1-> start of next dst plane   */
    }

}




#define FXSR    0x80
#define NFSR    0x40
#define SKEW    0x0f
#define BUSY    0x80
#define HOG     0x40
#define SMUDGE  0x20
#define LINENO  0x0f

#define GetMemW(addr) ((ULONG)*(UWORD*)(addr))
#define SetMemW(addr, val) *(UWORD*)(addr) = val

static void do_blit(void)
{
    ULONG   blt_src_in;
    UWORD   blt_src_out, blt_hop_out, blt_dst_in, blt_dst_out, mask_out;
    int     xc, yc, lineno, last, first;

#if DBG_BLIT
    kprintf ("bitblt: Start\n");
    kprintf ("HALFT[] 0x%04x-%04x-%04x-%04x\n", (UWORD) blt_halftone[0], blt_halftone[1], blt_halftone[2], blt_halftone[3]);
    kprintf ("X COUNT 0x%04x\n", (UWORD) blt_x_cnt);
    kprintf ("Y COUNT 0x%04x\n", (UWORD) blt_y_cnt);
    kprintf ("X S INC 0x%04x\n", (UWORD) blt_src_x_inc);
    kprintf ("Y S INC 0x%04x\n", (UWORD) blt_src_y_inc);
    kprintf ("X D INC 0x%04x\n", (UWORD) blt_dst_x_inc);
    kprintf ("Y D INC 0x%04x\n", (UWORD) blt_dst_y_inc);
    kprintf ("ENDMASK 0x%04x-%04x-%04x\n", (UWORD) blt_end_1, (UWORD) blt_end_2, (UWORD) blt_end_3);
    kprintf ("S_ADDR  0x%08lx\n", blt_src_addr);
    kprintf ("D_ADDR  0x%08lx\n", blt_dst_addr);
    kprintf ("HOP=%01d, OP=%02d\n", blt_hop & 0x3, blt_op & 0xf);
    kprintf ("HOPline=%02d\n", blt_status & 0xf);
    kprintf ("NFSR=%d, FXSR=%d, SKEW=%02d\n", (blt_skew & NFSR) != 0,
                                              (blt_skew & FXSR) != 0,
                                              (blt_skew & SKEW));
#endif
    xc = 0;
    yc = (blt_y_cnt == 0) ? 65536 : blt_y_cnt;
    while (yc-- > 0) {
        xc = (blt_x_cnt == 0) ? 65536 : blt_x_cnt;
        first = 1;
        blt_src_in = 0;
        /* next line to get rid of obnoxious compiler warnings */
        blt_src_out = blt_hop_out = blt_dst_out = 0;
        while (xc-- > 0) {
            last = (xc == 0);
            if ((blt_hop & 0x03) >= 2) {
                /* read source into blt_src_in */
                if (blt_src_x_inc >= 0) {
                    if (first && (blt_skew & FXSR)) {
                        blt_src_in = GetMemW (blt_src_addr);
                        blt_src_addr += blt_src_x_inc;
                    }
                    blt_src_in <<= 16;

                    if (last && (blt_skew & NFSR)) {
                        blt_src_addr -= blt_src_x_inc;
                    } else {
                        blt_src_in |= GetMemW (blt_src_addr);
                        if (!last) {
                            blt_src_addr += blt_src_x_inc;
                        }
                    }
                } else {
                    if (first &&  (blt_skew & FXSR)) {
                        blt_src_in = GetMemW (blt_src_addr);
                        blt_src_addr +=blt_src_x_inc;
                    } else {
                        blt_src_in >>= 16;
                    }
                    if (last && (blt_skew & NFSR)) {
                        blt_src_addr -= blt_src_x_inc;
                    } else {
                        blt_src_in |= (GetMemW (blt_src_addr) << 16);
                        if (!last) {
                            blt_src_addr += blt_src_x_inc;
                        }
                    }
                }
                /* shift blt_skew times into blt_src_out */
                blt_src_out = blt_src_in >> (blt_skew & SKEW);
#if (VERBOSE & 0x8)
                kprintf ("%04x ", blt_src_out);
#endif
            }
            /* halftone OP */
            lineno = ((blt_status & SMUDGE) ? blt_src_out : blt_status) & LINENO;
            switch (blt_hop & 0x3) {
            case 0:
                blt_hop_out = 0xffff;
                break;
            case 1:
                blt_hop_out = blt_halftone[lineno];
                break;
            case 2:
                blt_hop_out = blt_src_out;
                break;
            case 3:
                blt_hop_out = blt_src_out & blt_halftone[lineno];
                break;
            }
            /* read destination into blt_dst_in */
            blt_dst_in = GetMemW (blt_dst_addr);
            /* op into blt_dst_out */
            switch (blt_op & 0xf) {
            case 0:
                blt_dst_out = 0;
                break;
            case 1:
                blt_dst_out = blt_hop_out & blt_dst_in;
                break;
            case 2:
                blt_dst_out = blt_hop_out & ~blt_dst_in;
                break;
            case 3:
                blt_dst_out = blt_hop_out;
                break;
            case 4:
                blt_dst_out = ~blt_hop_out & blt_dst_in;
                break;
            case 5:
                blt_dst_out = blt_dst_in;
                break;
            case 6:
                blt_dst_out = blt_hop_out ^ blt_dst_in;
                break;
            case 7:
                blt_dst_out = blt_hop_out | blt_dst_in;
                break;
            case 8:
                blt_dst_out = ~blt_hop_out & ~blt_dst_in;
                break;
            case 9:
                blt_dst_out = ~blt_hop_out ^ blt_dst_in;
                break;
            case 0xa:
                blt_dst_out = ~blt_dst_in;
                break;
            case 0xb:
                blt_dst_out = blt_hop_out | ~blt_dst_in;
                break;
            case 0xc:
                blt_dst_out = ~blt_hop_out;
                break;
            case 0xd:
                blt_dst_out = ~blt_hop_out | blt_dst_in;
                break;
            case 0xe:
                blt_dst_out = ~blt_hop_out | ~blt_dst_in;
                break;
            case 0xf:
                blt_dst_out = 0xffff;
                break;
            }

            /* and endmask */
            if (first) {
                mask_out = (blt_dst_out & blt_end_1) | (blt_dst_in & ~blt_end_1);
            } else if (last) {
                mask_out = (blt_dst_out & blt_end_3) | (blt_dst_in & ~blt_end_3);
            } else {
                mask_out = (blt_dst_out & blt_end_2) | (blt_dst_in & ~blt_end_2);
            }
            SetMemW (blt_dst_addr, mask_out);
            if (!last) {
                blt_dst_addr += blt_dst_x_inc;
            }
            first = 0;
        }
#if DBG_BLIT
        kprintf ("\n");
#endif
        blt_status = (blt_status + ((blt_dst_y_inc >= 0) ? 1 : 15)) & 0xef;
        blt_src_addr += blt_src_y_inc;
        blt_dst_addr += blt_dst_y_inc;
    }
    /* blt_status &= ~BUSY; */
    blt_y_cnt = 0;
#if DBG_BLIT
    kprintf ("bitblt: End\n");
    kprintf ("HALFT[] 0x%04x-%04x-%04x-%04x\n", (UWORD) blt_halftone[0], blt_halftone[1], blt_halftone[2], blt_halftone[3]);
    kprintf ("X COUNT 0x%04x\n", (UWORD) xc);
    kprintf ("Y COUNT 0x%04x\n", (UWORD) yc);
    kprintf ("X S INC 0x%04x\n", (UWORD) blt_src_x_inc);
    kprintf ("Y S INC 0x%04x\n", (UWORD) blt_src_y_inc);
    kprintf ("X D INC 0x%04x\n", (UWORD) blt_dst_x_inc);
    kprintf ("Y D INC 0x%04x\n", (UWORD) blt_dst_y_inc);
    kprintf ("ENDMASK 0x%04x-%04x-%04x\n", (UWORD) blt_end_1, (UWORD) blt_end_2, (UWORD) blt_end_3);
    kprintf ("S_ADDR  0x%08lx\n", blt_src_addr);
    kprintf ("D_ADDR  0x%08lx\n", blt_dst_addr);
    kprintf ("HOP=%01d, OP=%02d\n", blt_hop & 0x3, blt_op & 0xf);
    kprintf ("HOPline=%02d\n", blt_status & 0xf);
    kprintf ("NFSR=%d, FXSR=%d, SKEW=%02d\n", (blt_skew & NFSR) != 0,
                                              (blt_skew & FXSR) != 0,
                                              (blt_skew & SKEW));
#endif


}
#endif   //BLITTER_IN_C
