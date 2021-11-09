/*
 * EmuTOS AES
 *
 * Copyright (C) 2002-2021 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef AESCFG_H
#define AESCFG_H

#include "emutos.h"

#define AES_CFG_PROVIDES_RESOLUTION   1<<0
#define AES_CFG_PROVIDES_BACKGROUND   1<<1
#define AES_CFG_PROVIDES_DCLICK_SPEED 1<<2
#define AES_CFG_PROVIDES_BLITTER      1<<3
#define AES_CFG_PROVIDES_CPUCACHE     1<<4
#define AES_CFG_PROVIDES_AUTOSTART    1<<5

struct aes_configuration_t /* Parsed from the config file */
{
        WORD  flags;                    /* Indicates which config elements are present */ 
        WORD  videomode;                /* For feeding into check_moderez */
        UBYTE desktop_background[3];    /* Desktop backgrounds (for when 1, 2, and >2 planes) */
        WORD  double_click_rate;
        WORD  blitter_on;
        WORD  cpu_cache_on; /* Not too sure if it's cache on or cache off */
        BOOL  autostart_is_gem;
        char  autostart[MAXPATHLEN];
};

extern struct aes_configuration_t aes_configuration;


void aescfg_read(void);

#endif
