/*
 * vdipub.h - Public VDI functions
 *
 * Copyright (C) 2017 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef VDIPUB_H
#define VDIPUB_H
#include "portab.h"

/* sets VDI's mouse cursor coordinates */
void set_vdi_mousexy(WORD x, WORD y);   /* see vdi_misc.c */

#endif /* VDIPUB_H */
