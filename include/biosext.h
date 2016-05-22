/*
 * biosext.h - EmuTOS BIOS extensions not callable with trap
 *
 * Copyright (C) 2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOSEXT_H
#define BIOSEXT_H

void invalidate_instruction_cache(void *start, long size);

#if CONF_WITH_SHUTDOWN
BOOL can_shutdown(void);
#endif

#endif /* BIOSEXT_H */
