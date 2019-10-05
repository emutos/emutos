/*
 * parport.h - limited parallel port support
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/licence.txt for details.
 */

LONG bconin0(void);
LONG bcostat0(void);
LONG bconout0(WORD dev, WORD c);

void parport_init(void);

#if CONF_WITH_PRINTER_PORT
WORD setprt(WORD config);
#endif
