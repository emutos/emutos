/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMDOS_H
#define GEMDOS_H

extern UWORD    DOS_AX;
extern UWORD    DOS_ERR;


WORD pgmld(WORD handle, BYTE *pname, LONG **ldaddr);

WORD dos_gdrv(void);
void dos_sdta(void *ldta);
void *dos_gdta(void);
WORD dos_sfirst(BYTE *pspec, WORD attr);
WORD dos_snext(void);
WORD dos_open(BYTE *pname, WORD access);
WORD dos_close(WORD handle);
LONG dos_read(WORD handle, LONG cnt, void *pbuffer);
LONG dos_write(WORD handle, LONG cnt, void *pbuffer);
LONG dos_lseek(WORD handle, WORD smode, LONG sofst);
void dos_exec(WORD mode, const BYTE *pcspec, const BYTE *pcmdln, const BYTE *segenv);  /* see: gemstart.S */
LONG dos_chdir(BYTE *pdrvpath);
WORD dos_gdir(WORD drive, BYTE *pdrvpath);
LONG dos_sdrv(WORD newdrv);
LONG dos_create(BYTE *name, WORD attr);
WORD dos_mkdir(BYTE *path);
WORD dos_chmod(BYTE *name, WORD wrt, WORD mod);
WORD dos_setdt(UWORD h, UWORD time, UWORD date);
WORD dos_label(BYTE drive, BYTE *plabel);
LONG dos_delete(BYTE *name);
WORD dos_space(WORD drv, LONG *ptotal, LONG *pavail);
WORD dos_rename(BYTE *p1, BYTE *p2);
WORD dos_rmdir(BYTE *path);

LONG dos_alloc(LONG nbytes);
LONG dos_avail(void);
WORD dos_free(LONG maddr);
WORD dos_shrink(void *maddr, LONG length);

#endif
