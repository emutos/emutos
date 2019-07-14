/*
 * font.h - font specific definitions
 *
 * Copyright (C) 2001 Lineo, Inc.
 * Copyright (C) 2004 by Authors (see below)
 * Copyright (C) 2015-2017 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef FONT_H
#define FONT_H

#include "fonthdr.h"

/* font specific linea variables */

extern const UWORD *v_fnt_ad;   /* address of current monospace font */
extern const UWORD *v_off_ad;   /* address of font offset table */
extern UWORD v_fnt_nd;          /* ascii code of last cell in font */
extern UWORD v_fnt_st;          /* ascii code of first cell in font */
extern UWORD v_fnt_wr;          /* font cell wrap */

/*
 * font_ring is a struct of four pointers, each of which points to
 * a list of font headers linked together to form a string.
 */

extern const Fonthead *font_ring[4];    /* Ring of available fonts */
extern WORD font_count;                 /* all three fonts and NULL */

/* prototypes */

void font_init(void);           /* initialize BIOS font ring */
void font_set_default(WORD cellheight); /* choose the default font */

#endif /* FONT_H */
