
#ifndef GEMDOS_H
#define GEMDOS_H

extern UWORD    DOS_AX;
extern UWORD    DOS_ERR;


WORD pgmld(WORD handle, BYTE *pname, LONG **ldaddr);

WORD dos_gdrv();
void dos_sdta(LONG ldta);
WORD dos_sfirst(LONG pspec, WORD attr);
WORD dos_snext();
WORD dos_open(BYTE *pname, WORD access);
WORD dos_close(WORD handle);
UWORD dos_read(WORD handle, UWORD cnt, LONG pbuffer);
UWORD dos_write(WORD handle, UWORD cnt, LONG pbuffer);
LONG dos_lseek(WORD handle, WORD smode, LONG sofst);
void dos_exec(LONG pcspec, LONG segenv, LONG pcmdln);
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
LONG dos_avail();
WORD dos_free(LONG maddr);

#endif
