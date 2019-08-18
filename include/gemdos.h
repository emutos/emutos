/*
 * EmuTOS interface to GEMDOS
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMDOS_H
#define GEMDOS_H

WORD pgmld(WORD handle, char *pname, LONG **ldaddr);

LONG dos_rawcin(void);
void dos_conws(char *string);
WORD dos_conis(void);
WORD dos_gdrv(void);
void dos_sdta(void *ldta);
void *dos_gdta(void);
WORD dos_sfirst(char *pspec, WORD attr);
WORD dos_snext(void);
LONG dos_open(char *pname, WORD access);
WORD dos_close(WORD handle);
LONG dos_read(WORD handle, LONG cnt, void *pbuffer);
LONG dos_write(WORD handle, LONG cnt, void *pbuffer);
LONG dos_lseek(WORD handle, WORD smode, LONG sofst);
LONG dos_exec(WORD mode, const char *pcspec, const char *pcmdln, const char *segenv); /* see: gemstart.S */
LONG dos_chdir(char *pdrvpath);
WORD dos_gdir(WORD drive, char *pdrvpath);
LONG dos_sdrv(WORD newdrv);
LONG dos_create(char *name, WORD attr);
WORD dos_mkdir(char *path);
WORD dos_chmod(char *name, WORD wrt, WORD mod);
WORD dos_setdt(UWORD h, UWORD time, UWORD date);
WORD dos_label(char drive, char *plabel);
LONG dos_delete(char *name);
void dos_space(WORD drv, LONG *ptotal, LONG *pavail);
WORD dos_rename(char *p1, char *p2);
WORD dos_rmdir(char *path);
LONG dos_load_file(char *filename, LONG count, char *buf);

void *dos_alloc_stram(LONG nbytes);
void *dos_alloc_anyram(LONG nbytes);
LONG dos_avail_stram(void);
LONG dos_avail_altram(void);
LONG dos_avail_anyram(void);
WORD dos_free(void *maddr);
WORD dos_shrink(void *maddr, LONG length);

#endif
