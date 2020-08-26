/*
 * gemshlib.h - header for EmuTOS AES Shell Library functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMSHLIB_H
#define GEMSHLIB_H

extern char     *ad_stail;

extern WORD     gl_changerez;
extern WORD     gl_nextrez;

void sh_read(char *pcmd, char *ptail);
WORD sh_write(WORD doex, WORD isgem, WORD isover, const char *pcmd, const char *ptail);
void sh_get(void *pbuffer, WORD len);
void sh_put(const void *pdata, WORD len);
void sh_tographic(void);

char *sh_name(char *ppath);
void sh_envrn(char **ppath, const char *psrch);

WORD sh_find(char *pspec);

void sh_rdef(char *lpcmd, char *lpdir);
void sh_wdef(const char *lpcmd, const char *lpdir);

void sh_main(BOOL isauto, BOOL isgem);

LONG aes_run_rom_program(PRG_ENTRY *entry);
#endif
