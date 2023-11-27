/*
 * nvram.h - header for NVRAM access
 *
 * Copyright (C) 2001-2023 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#if CONF_WITH_NVRAM

/* internal feature detection */

void detect_nvram(void);
UBYTE get_nvram_rtc(int index);
void set_nvram_rtc(int index, int data);

/* XBios function */

WORD nvmaccess(WORD type, WORD start, WORD count, UBYTE *buffer);

#endif  /* CONF_WITH_NVRAM */
