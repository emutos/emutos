/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMSHLIB_H
#define GEMSHLIB_H

extern SHELL    sh[];

extern LONG     ad_scmd;
extern LONG     ad_stail;
extern LONG     ad_ssave;
extern LONG     ad_dta;
extern LONG     ad_path;

extern LONG     ad_pfile;

extern WORD     gl_shgem;

extern WORD     gl_changerez;
extern WORD     gl_nextrez;

void sh_read(LONG pcmd, LONG ptail);
void sh_curdir(LONG ppath);
WORD sh_write(WORD doex, WORD isgem, WORD isover, LONG pcmd, LONG ptail);
void sh_get(LONG pbuffer, WORD len);
void sh_put(LONG pdata, WORD len);
void sh_tographic(void);
void sh_toalpha(void);

BYTE *sh_name(BYTE *ppath);
void sh_envrn(LONG ppath, LONG psrch);

WORD sh_find(LONG pspec);

void sh_rdef(LONG lpcmd, LONG lpdir);
void sh_wdef(LONG lpcmd, LONG lpdir);

void sh_main(void);

#endif
