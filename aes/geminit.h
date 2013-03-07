/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2013 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMINIT_H
#define GEMINIT_H
#include "gsxdefs.h"

#define PATH_ENV    "PATH="     /* PATH environment variable */
#define DEF_PATH    "C:\\GEMAPPS\\GEMSYS;C:\\GEMAPPS;C:\\"  /* default value */

#define DEF_DESKTOP "EMUDESK"   /* default desktop */

extern LONG     ad_valstr;

extern LONG     ad_sysglo;
extern LONG     ad_armice;
extern LONG     ad_hgmice;
extern BYTE     *ad_envrn;
extern LONG     ad_stdesk;

extern BYTE     gl_dir[130];
extern BYTE     gl_1loc[256];
extern BYTE     gl_2loc[256];
extern MFORM    gl_mouse;
extern BYTE     gl_logdrv;

extern WORD     totpds;

extern THEGLO   D;


void all_run(void);
void sh_deskf(WORD obj, LONG plong);

#endif
