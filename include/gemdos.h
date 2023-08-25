/*
 * gemdos.h - EmuTOS interface to GEMDOS
 *
 * Copyright (C) 2002-2023 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMDOS_H
#define GEMDOS_H

#include "bdosbind.h"

WORD pgmld(WORD handle, char *pname, LONG **ldaddr);
LONG dos_exec(WORD mode, const char *pcspec, const char *pcmdln, const char *segenv); /* see: gemstart.S */
WORD dos_setdt(UWORD h, UWORD time, UWORD date);
WORD dos_label(char drive, char *plabel);
void dos_space(WORD drv, LONG *ptotal, LONG *pavail);
LONG dos_load_file(char *filename, LONG count, char *buf);

void *dos_alloc_stram(LONG nbytes);
void *dos_alloc_anyram(LONG nbytes);
LONG dos_avail_stram(void);
LONG dos_avail_altram(void);
LONG dos_avail_anyram(void);
WORD dos_free(void *maddr);
WORD dos_shrink(void *maddr, LONG length);

static __inline__ LONG dos_rawcin(void)
{
    return Crawcin();
}

static __inline__ void dos_conws(char *string)
{
    Cconws(string);
}

static __inline__ WORD dos_conis(void)
{
    return Cconis();
}

static __inline__ WORD dos_gdrv(void)
{
    return Dgetdrv();
}

static __inline__ void dos_sdta(void *ldta)
{
    Fsetdta(ldta);
}

static __inline__ void *dos_gdta(void)
{
    return (void *)Fgetdta();
}

static __inline__ WORD dos_sfirst(char *pspec, WORD attr)
{
    return Fsfirst(pspec,attr);
}

static __inline__ WORD dos_snext(void)
{
    return Fsnext();
}

static __inline__ LONG dos_open(char *pname, WORD access)
{
    return Fopen(pname,access);
}

static __inline__ WORD dos_close(WORD handle)
{
    return Fclose(handle);
}

static __inline__ LONG dos_read(WORD handle, LONG cnt, void *pbuffer)
{
    return Fread(handle,cnt,pbuffer);
}

static __inline__ LONG dos_write(WORD handle, LONG cnt, void *pbuffer)
{
    return Fwrite(handle,cnt,pbuffer);
}

static __inline__ LONG dos_lseek(WORD handle, WORD smode, LONG sofst)
{
    return Fseek(sofst, handle, smode);
}

static __inline__ LONG dos_chdir(char *pdrvpath)
{
    return Dsetpath(pdrvpath);
}

static __inline__ WORD dos_gdir(WORD drive, char *pdrvpath)
{
    return Dgetpath(pdrvpath,drive);
}

static __inline__ LONG dos_sdrv(WORD newdrv)
{
    return Dsetdrv(newdrv);
}

static __inline__ LONG dos_create(char *name, WORD attr)
{
    return Fcreate(name,attr);
}

static __inline__ WORD dos_mkdir(char *path)
{
    return Dcreate(path);
}

static __inline__ WORD dos_chmod(char *name, WORD wrt, WORD mod)
{
    return Fattrib(name,wrt,mod);
}

static __inline__ LONG dos_delete(char *name)
{
    return Fdelete(name);
}

static __inline__ WORD dos_rename(char *p1, char *p2)
{
    return Frename(p1,p2);
}

static __inline__ WORD dos_rmdir(char *path)
{
    return Ddelete(path);
}

#endif
