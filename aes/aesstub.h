/*
 * aesstub.h - AES entry points, called by BIOS and VDI
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _AESSTUB_H
#define _AESSTUB_H
#include "aesdefs.h"

/* returns default mouse form . Used by VDI. */
MFORM *default_mform(void);

/* AES entry point */
void ui_start(void) NORETURN;   /* found in aes/gemstart.S */

#endif /* _AESSTUB_H */
