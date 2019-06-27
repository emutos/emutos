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

/*
 * values for shel_write() 'doex' arg
 */
#define SHW_NOEXEC      0       /* just return to desktop */
#define SHW_EXEC        1       /* run another program after this */
#define SHW_SHUTDOWN    4       /* shutdown system */
#define SHW_RESCHNG     5       /* change resolution */

/* AES entry point */
void ui_start(void) NORETURN;   /* found in aes/gemstart.S */

/* returns default mouse form */
MFORM *default_mform(void);

/* functions used by AES and desktop, found in gemrslib.c */
void xlate_obj_array(OBJECT *obj_array, int nobj);
BOOL fix_tedinfo(TEDINFO *tedinfo, int nted);

#endif /* AESPUB_H */
