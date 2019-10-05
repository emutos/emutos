/*
 * screen.h - low-level screen routines
 *
 * Copyright (C) 2013-2019 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef VIDEL_H
#define VIDEL_H

#if CONF_WITH_VIDEL

#define SPSHIFT             0xffff8266L

#define FRGB_BLACK     0x00000000       /* Falcon palette */
#define FRGB_BLUE      0x000000ff
#define FRGB_GREEN     0x00ff0000
#define FRGB_CYAN      0x00ff00ff
#define FRGB_RED       0xff000000
#define FRGB_MAGENTA   0xff0000ff
#define FRGB_LTGRAY    0xbbbb00bb
#define FRGB_GRAY      0x88880088
#define FRGB_LTBLUE    0x000000aa
#define FRGB_LTGREEN   0x00aa0000
#define FRGB_LTCYAN    0x00aa00aa
#define FRGB_LTRED     0xaa000000
#define FRGB_LTMAGENTA 0xaa0000aa
#define FRGB_YELLOW    0xffff0000
#define FRGB_LTYELLOW  0xaaaa0000
#define FRGB_WHITE     0xffff00ff

/* test for VDI support of videomode */
#define VALID_VDI_BPP(mode) ((mode&VIDEL_BPPMASK)<=VIDEL_8BPP)

/* selected Falcon videomodes */
#define FALCON_ST_HIGH      (VIDEL_COMPAT|VIDEL_VGA|VIDEL_80COL|VIDEL_1BPP)

#define FALCON_DEFAULT_BOOT (VIDEL_VERTICAL|VIDEL_80COL|VIDEL_4BPP) /* 640x480x16 colours, TV, NTSC */

typedef struct {
    WORD vmode;         /* video mode (-1 => end marker) */
    WORD monitor;       /* applicable monitors */
    UWORD hht;          /* H hold timer */
    UWORD hbb;          /* H border begin */
    UWORD hbe;          /* H border end */
    UWORD hdb;          /* H display begin */
    UWORD hde;          /* H display end */
    UWORD hss;          /* H SS */
    UWORD vft;          /* V freq timer */
    UWORD vbb;          /* V border begin */
    UWORD vbe;          /* V border end */
    UWORD vdb;          /* V display begin */
    UWORD vde;          /* V display end */
    UWORD vss;          /* V SS */
} VMODE_ENTRY;

void initialise_falcon_palette(WORD mode);
const VMODE_ENTRY *lookup_videl_mode(WORD mode,WORD monitor);

/* Public XBIOS functions */
WORD vsetmode(WORD mode);
WORD vmontype(void);
WORD vsetsync(WORD external);
LONG vgetsize(WORD mode);
WORD vsetrgb(WORD index,WORD count,const ULONG *rgb);
WORD vgetrgb(WORD index,WORD count,ULONG *rgb);

/* misc routines */
WORD vfixmode(WORD mode);
WORD videl_check_moderez(WORD moderez);
void videl_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez);

extern WORD current_video_mode;

#endif /* CONF_WITH_VIDEL */

#endif /* VIDEL_H */
