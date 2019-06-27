/*
 * aespub.h - Public AES functions
 *
 * Copyright (C) 2016-2019 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef AESPUB_H
#define AESPUB_H

#include "portab.h"
#include "obdefs.h"
#include "gsxdefs.h"

/* functions used by AES and desktop, found in gemrslib.c */
void xlate_obj_array(OBJECT *obj_array, int nobj);
BOOL fix_tedinfo(TEDINFO *tedinfo, int nted);

#endif /* AESPUB_H */
