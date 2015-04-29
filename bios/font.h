/*
 * font.h - font specific definitions
 *
 * Copyright (c) 2001 Lineo, Inc.
 * Copyright (c) 2004 by Authors:
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef FONT_H
#define FONT_H

#include "portab.h"
#include "fonthdr.h"

/* font style bits */

#define F_THICKEN 1
#define F_LIGHT 2
#define F_SKEW  4
#define F_UNDER 8
#define F_OUTLINE 16
#define F_SHADOW        32

/* font specific linea variables */

extern const UWORD *v_fnt_ad;   /* address of current monospace font */
extern const UWORD *v_off_ad;   /* address of font offset table */
extern UWORD v_fnt_nd;          /* ascii code of last cell in font */
extern UWORD v_fnt_st;          /* ascii code of first cell in font */
extern UWORD v_fnt_wr;          /* font cell wrap */

/* character cell specific linea variables */

extern UWORD    v_cel_ht;       /* cell height (width is 8) */
extern UWORD    v_cel_mx;       /* needed by MiNT: columns on the screen minus 1 */
extern UWORD    v_cel_my;       /* needed by MiNT: rows on the screen minus 1 */
extern UWORD    v_cel_wr;       /* needed by MiNT: length (in bytes) of a line of characters */

/*
 * font_ring is a struct of four pointers, each of which points to
 * a list of font headers linked together to form a string.
 */

extern Fonthead *font_ring[4];  /* Ring of available fonts */
extern WORD font_count;                 /* all three fonts and NULL */

/* prototypes */

void font_init(void);           /* initialize BIOS font ring */
void font_set_default(void);    /* choose the default font */

#endif /* FONT_H */
