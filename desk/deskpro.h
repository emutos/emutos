/*
 * EmuTOS desktop
 *
 * Copyright (C) 2002-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKPRO_H
#define _DESKPRO_H

WORD pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr);

#if CONF_WITH_SHUTDOWN
WORD pro_exit(void);
#endif

#endif  /* _DESKPRO_H */
