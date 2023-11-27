/*
 * xbios.h - misc XBIOS function prototypes
 *
 * Copyright (C) 2002-2023 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef XBIOS_H
#define XBIOS_H

/* used by vectors.S */
LONG xbios_do_unimpl(WORD number);

#endif /* XBIOS_H */
