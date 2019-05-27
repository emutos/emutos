/*
 * EmuTOS AES
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMSCLIB_H
#define GEMSCLIB_H

#define SCRAP_DIR_NAME  "C:\\CLIPBRD"

WORD sc_read(char *pscrap);
WORD sc_write(const char *pscrap);
WORD sc_clear(void);


#endif
