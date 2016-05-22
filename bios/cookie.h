/*
 * cookie.h - declarations for the cookie jar
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef COOKIE_H
#define COOKIE_H

struct cookie {
        long tag;
        long value;
};

#define CJAR (* (struct cookie **) 0x5a0)

/*
 * Some relevant tag values
 * cookies will also exist in variables, defined in bios/machine.c
 */

#define COOKIE_CPU      0x5f435055L
#define COOKIE_VDO      0x5f56444fL
#define COOKIE_FPU      0x5f465055L
#define COOKIE_FDC      0x5f464443L
#define COOKIE_SND      0x5f534e44L
#define COOKIE_MCH      0x5f4d4348L
#define COOKIE_SWI      0x5f535749L
#define COOKIE_FRB      0x5f465242L
#define COOKIE_FLK      0x5f464c4bL
#define COOKIE_NET      0x5f4e4554L
#define COOKIE_IDT      0x5f494454L
#define COOKIE_AKP      0x5f414b50L
#define COOKIE_NATFEAT  0x5f5f4e46L
#define COOKIE_XHDI     0x58484449L
#define COOKIE_COLDFIRE 0x5f43465fL
#define COOKIE_MCF      0x5f4d4346L

/*
 * values of _MCH cookie
 */

#define MCH_NOHARD  0xffffffffL /* No ST-compatible hardware */
#define MCH_ST      0
#define MCH_STE     0x00010000L
#define MCH_MSTE    0x00010010L
#define MCH_TT      0x00020000L
#define MCH_FALCON  0x00030000L
#define MCH_MILAN_C 0x00040000L
#define MCH_ARANYM  0x00050000L

/*
 * value of _FDC cookie
 */

#define FDC_0ATC    0x00415443L
#define FDC_1ATC    0x01415443L
#define FDC_2ATC    0x02415443L

/*
 * values of _SND cookie
 */

#define SND_PSG         0x01    /* Yamaha PSG */
#define SND_8BIT        0x02    /* 8 bit DMA stereo */
#define SND_16BIT       0x04    /* 16 bit CODEC */
#define SND_DSP         0x08    /* DSP */
#define SND_MATRIX      0x10    /* Connection Matrix */
#define SND_XBIOS       0x20    /* XBIOS / XGSB */

/* functions */

void cookie_init(void);
void cookie_add(long tag, long val);
#if CONF_WITH_FRB
UBYTE *get_frb_cookie(void);
#endif

#endif
