/*
 * EmuTOS aes
 *
 * Copyright (C) 2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMSHLIB_H
#define GEMSHLIB_H

extern SHELL    sh[];

extern BYTE     *ad_stail;

extern LONG     ad_pfile;

extern WORD     gl_shgem;

extern WORD     gl_changerez;
extern WORD     gl_nextrez;

void sh_read(BYTE *pcmd, BYTE *ptail);
void sh_curdir(BYTE *ppath);
WORD sh_write(WORD doex, WORD isgem, WORD isover, const BYTE *pcmd, const BYTE *ptail);
void sh_get(void *pbuffer, WORD len);
void sh_put(const void *pdata, WORD len);
void sh_tographic(void);

BYTE *sh_name(BYTE *ppath);
void sh_envrn(BYTE **ppath, const BYTE *psrch);

WORD sh_find(BYTE *pspec);

void sh_rdef(BYTE *lpcmd, BYTE *lpdir);
void sh_wdef(const BYTE *lpcmd, const BYTE *lpdir);

void sh_main(void);

typedef void PRG_ENTRY(void);   /* Program entry point type */
void aes_run_rom_program(PRG_ENTRY *entry);
#endif
