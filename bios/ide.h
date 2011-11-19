/*
 * ide.h - Falcon IDE functions
 *
 * Copyright (c) 2011 EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Riviere
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef IDE_H
#define IDE_H

#include "portab.h"

#if CONF_WITH_IDE

LONG ide_rw(WORD rw, LONG sector, WORD count, LONG buf, WORD dev);

#endif /* CONF_WITH_IDE */

#endif /* IDE_H */
