/*
 * aespub.h - Public AES functions
 *
 * Copyright (C) 2016-2017 The EmuTOS development team
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

#define PATH_ENV    "PATH="     /* PATH environment variable */

/* AES entry point */
void ui_start(void) NORETURN;   /* found in aes/gemstart.S */

/* returns default mouse form */
MFORM *default_mform(void);

#endif /* AESPUB_H */
