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
#include "lineavars.h"
#include "gsxextrn.h"
#include "tosvars.h"
#include "mouse.h"

#define DBG_BLIT 0
#define C_BLIT 1        // decide, which implementation

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
    WORD mode, bits;
    MFDB *src,*dst;

    /* check user write (write mode #5 (D' = D) => do nothing */
    mode  = INTIN[0];
    if ( mode < 0 || mode == 5 || mode > 15)
        return;                 /* mode is invalid */

    /* check the pattern flag and revert to log op # */
    info.p_addr = NULL;		/* No pattern selected */
    bits = (1<<PAT_FLAG);
    if ( mode & bits )   	/* and set bit to 0! */ {
        info.p_addr = patptr;	/* get pattern */

        /* multi-plane pattern? */
        info.p_nxpl = 0;	/* next plane pattern offset default. */
        if ( multifill ) {
            info.p_nxpl = 32;	/* yes, next plane pat offset = 32. */
        }
        info.p_nxln = 2;        /* offset to next line in pattern */
        info.p_mask = 0xf;      /* pattern index mask */
    }
    info.s_nxpl = 2;		/* d4 <- next plane offset (source) */
    info.d_nxpl = 2;		/* d5 <- next plane offset (destination) */

    /* Get the pointers to the MFDBs */
    src = *(MFDB **)&CONTRL[7]; /* a5, source MFDB */
    dst = *(MFDB **)&CONTRL[9]; /* a4, destination MFDB */

    /* setup planes for source MFDB */
    if ( src->fd_addr < 0 )     /* d6 */
        return;			/* negative source address invalid */
    else if ( src->fd_addr > 0 ) {
        /* for a positive source address */
        info.s_form = src->fd_addr;
        info.s_nxwd = src->fd_nplanes * 2;
        info.s_nxln = src->fd_wdwidth * 2 * src->fd_h;
    }
    else {
        /* source address == NULL */
        info.s_form = (UWORD*) v_bas_ad;		/* source form is screen */
        info.s_nxwd = v_planes * 2;
        info.s_nxln = v_lin_wr;
    }

    /* setup planes for destination MFDB */
    if ( dst->fd_addr < 0 )     /* d7 */
        return;			/* negative source address invalid */
    else if ( dst->fd_addr > 0 ) {
        /* for a positive address */
        info.d_form = dst->fd_addr;
        info.plane_ct = dst->fd_nplanes;
        info.d_nxwd = dst->fd_nplanes * 2;
        info.d_nxln = dst->fd_wdwidth * 2 * dst->fd_h;
    }
    else {
        /* source address == NULL */
        info.d_form = (UWORD*) v_bas_ad;		/* source form is screen */
        info.plane_ct = v_planes;
        info.d_nxwd = v_planes * 2;
        info.d_nxln = v_lin_wr;
    }

    /* only 4,2, and 1 planes are valid (destination) */
    if ( 0xfff8 & info.plane_ct )
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
            info.fg_col = 0;		/* were only interested in one color */
            info.bg_col = bg;		/* save the color of interest */
            break;

        case MD_REPLACE:
            /* CHECK: bug, that colors are reversed? */
            info.op_tab[0] = 00;	/* fg:0 bg:0  D' <- 0 */
            info.op_tab[1] = 12;	/* fg:0 bg:1  D' <- not S */
            info.op_tab[2] = 03;	/* fg:1 bg:0  D' <- S */
            info.op_tab[3] = 15;	/* fg:1 bg:1  D' <- 1 */
            info.bg_col = fg;		/* save fore and background colors */
            info.fg_col = bg;
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

#if 0
    CONTRL[N_PTSOUT] = 0;
    CONTRL[N_INTOUT] = 0;
#endif
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


