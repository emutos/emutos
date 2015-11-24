/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMDOS_H
#define GEMDOS_H

/*
 * The following globals are set by the dos_xxx() functions for errors,
 * rather than returning a special value.  Iff DOS_ERR is non-zero, an
 * error has occurred, and the value in DOS_AX indicates the type of
 * error.  For historical reasons, DOS_AX is set as follows:
 *  -1 to -31   a BIOS error has occurred: see gemerror.h for a list
 *  +1 to +18   a GEMDOS error has occurred: see dos.h for a list (the
 *              values are actually obtained by subtracting 31 from the
 *              absolute value of the "normal" GEMDOS error value)
 */
extern WORD     DOS_AX;
extern UWORD    DOS_ERR;


WORD pgmld(WORD handle, BYTE *pname, LONG **ldaddr);

void dos_conout(WORD ch);
LONG dos_rawcin(void);
WORD dos_conis(void);
WORD dos_gdrv(void);
void dos_conws(const char *str);
void dos_sdta(void *ldta);
void *dos_gdta(void);
WORD dos_sfirst(BYTE *pspec, WORD attr);
WORD dos_snext(void);
LONG dos_open(BYTE *pname, WORD access);
WORD dos_close(WORD handle);
LONG dos_read(WORD handle, LONG cnt, void *pbuffer);
LONG dos_write(WORD handle, LONG cnt, void *pbuffer);
LONG dos_lseek(WORD handle, WORD smode, LONG sofst);
LONG dos_exec(WORD mode, const BYTE *pcspec, const BYTE *pcmdln, const BYTE *segenv); /* see: gemstart.S */
LONG dos_chdir(BYTE *pdrvpath);
WORD dos_gdir(WORD drive, BYTE *pdrvpath);
LONG dos_sdrv(WORD newdrv);
LONG dos_create(BYTE *name, WORD attr);
WORD dos_mkdir(BYTE *path);
WORD dos_chmod(BYTE *name, WORD wrt, WORD mod);
WORD dos_setdt(UWORD h, UWORD time, UWORD date);
WORD dos_label(BYTE drive, BYTE *plabel);
LONG dos_delete(BYTE *name);
void dos_space(WORD drv, LONG *ptotal, LONG *pavail);
WORD dos_rename(BYTE *p1, BYTE *p2);
WORD dos_rmdir(BYTE *path);

void *dos_alloc(LONG nbytes);
LONG dos_avail(void);
WORD dos_free(LONG maddr);
WORD dos_shrink(void *maddr, LONG length);

#endif
