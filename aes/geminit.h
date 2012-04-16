/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMINIT_H
#define GEMINIT_H

extern LONG     ad_valstr;

extern LONG     ad_sysglo;
extern LONG     ad_armice;
extern LONG     ad_hgmice;
extern LONG     ad_mouse;
extern LONG     ad_envrn;
extern LONG     ad_stdesk;

extern BYTE     gl_dta[128];
extern BYTE     gl_dir[130];
extern BYTE     gl_1loc[256];
extern BYTE     gl_2loc[256];
extern WORD     gl_mouse[37];
extern LONG     ad_scdir;
extern BYTE     gl_logdrv;

extern WORD     totpds;

extern THEGLO   D;


void all_run(void);
void sh_deskf(WORD obj, LONG plong);
void sh_rdinf(void);

#endif
