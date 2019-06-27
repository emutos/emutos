/*
 * aesext.h - EmuTOS AES extensions not callable with trap
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _AESEXT_H
#define _AESEXT_H

#include "obdefs.h"
#include "gsxdefs.h"

/* functions used by AES and desktop, found in gemrslib.c */
void xlate_obj_array(OBJECT *obj_array, int nobj);
BOOL fix_tedinfo(TEDINFO *tedinfo, int nted);

#endif /* _AESEXT_H */
