/*
 * ide.h - Falcon IDE functions
 *
 * Copyright (C) 2011-2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef IDE_H
#define IDE_H

#include "portab.h"

#if CONF_WITH_IDE

void detect_ide(void);
void ide_init(void);
LONG ide_ioctl(WORD dev, UWORD ctrl, void *arg);
LONG ide_rw(WORD rw, LONG sector, WORD count, UBYTE *buf, WORD dev, BOOL need_byteswap);

#endif /* CONF_WITH_IDE */

#endif /* IDE_H */
