/*
 * vdistub.h - VDI entry points, called by BIOS
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _VDISTUB_H
#define _VDISTUB_H

#include "portab.h"

/* The VDI is just a library. It has no initialization routine.
 * To make it available, the BIOS just needs to install the vditrap function
 * below as trap #2 handler. */
void vditrap(void);

/* End of the VDI BSS section.
 * This is referenced by the OSHEADER */
extern UBYTE _endvdibss[];

#endif /* _VDISTUB_H */
