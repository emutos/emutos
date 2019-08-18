/*      JDOS.C          11/12/84 - 04/14/85     Lowell Webster          */
/*      GEMDOSIF.C      5/15/85 - 6/4/85        MDF                     */

/*
*       Copyright (C) 2002-2019 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 1.1
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1985                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "emutos.h"
#include "string.h"
#include "asm.h"
#include "gemdos.h"
#include "bdosbind.h"


WORD pgmld(WORD handle, char *pname, LONG **ldaddr)
{
    LONG    length, ret;
    LONG    *temp;

    ret = Pexec(PE_LOAD, pname, "", NULL);
    if (ret < 0L)
        return -1;

    *ldaddr = (LONG *) ret;

    /* program length = code+data+bss lengths plus basepage length */
    temp = *ldaddr;
    length = temp[3] + temp[5] + temp[7] + 0x100;
    if (Mshrink(*ldaddr, length) < 0L)
        return -1;

    return 0;
}


LONG dos_rawcin(void)
{
    return Crawcin();
}


void dos_conws(char *string)
{
    Cconws(string);
}


WORD dos_conis(void)
{
    return Cconis();
}


WORD dos_gdrv(void)
{
    return Dgetdrv();
}


void dos_sdta(void *ldta)
{
    Fsetdta(ldta);
}


void *dos_gdta(void)
{
    return (void *)Fgetdta();
}


WORD dos_sfirst(char *pspec, WORD attr)
{
    return Fsfirst(pspec,attr);
}


WORD dos_snext(void)
{
    return Fsnext();
}


LONG dos_open(char *pname, WORD access)
{
    return Fopen(pname,access);
}


WORD dos_close(WORD handle)
{
    return Fclose(handle);
}


LONG dos_read(WORD handle, LONG cnt, void *pbuffer)
{
    return Fread(handle,cnt,pbuffer);
}


LONG dos_write(WORD handle, LONG cnt, void *pbuffer)
{
    return Fwrite(handle,cnt,pbuffer);
}


LONG dos_lseek(WORD handle, WORD smode, LONG sofst)
{
    return Fseek(sofst, handle, smode);
}


LONG dos_chdir(char *pdrvpath)
{
    return Dsetpath(pdrvpath);
}


WORD dos_gdir(WORD drive, char *pdrvpath)
{
    return Dgetpath(pdrvpath,drive);
}


LONG dos_sdrv(WORD newdrv)
{
    return Dsetdrv(newdrv);
}


LONG dos_create(char *name, WORD attr)
{
    return Fcreate(name,attr);
}


WORD dos_mkdir(char *path)
{
    return Dcreate(path);
}


WORD dos_chmod(char *name, WORD wrt, WORD mod)
{
    return Fattrib(name,wrt,mod);
}


WORD dos_setdt(UWORD h, UWORD time, UWORD date)
{
    UWORD   buf[2];

    buf[0] = time;
    buf[1] = date;
    return Fdatime(buf,h,TRUE);
}


WORD dos_label(char drive, char *plabel)
{
    DTA     dta;
    char    path[8];

    Fsetdta(&dta);
    strcpy(path, " :\\*.*");
    path[0] = (drive + 'A') - 1;
    if (!Fsfirst(path,0x08))
    {
        strcpy(plabel,dta.d_fname);
        return TRUE;
    }
    else
        return FALSE;
}


LONG dos_delete(char *name)
{
    return Fdelete(name);
}


void dos_space(WORD drv, LONG *ptotal, LONG *pavail)
{
    LONG    buf[4];
    LONG    mult;

    *ptotal = *pavail = 0L;

    if (Dfree(buf,drv) < 0L) /* 0=default, 1=A for gemdos */
        return;

    mult = buf[3] * buf[2];
    *ptotal = mult * buf[1];
    *pavail = mult * buf[0];
}


WORD dos_rename(char *p1, char *p2)
{
    return Frename(p1,p2);
}


WORD dos_rmdir(char *path)
{
    return Ddelete(path);
}


/* allocate in ST RAM only */
void *dos_alloc_stram(LONG nbytes)
{
    return (void *)Mxalloc(nbytes,MX_STRAM);
}


/* get max size of available RAM in ST RAM only */
LONG dos_avail_stram(void)
{
    return Mxalloc(-1L,MX_STRAM);
}


#if CONF_WITH_ALT_RAM
/* get max size of available RAM in Alt-RAM only */
LONG dos_avail_altram(void)
{
    return Mxalloc(-1L,MX_TTRAM);
}
#endif


/* allocate in Alt-RAM (e.g. TT RAM) if possible, otherwise ST RAM */
void *dos_alloc_anyram(LONG nbytes)
{
    return (void *)Mxalloc(nbytes,MX_PREFTTRAM);
}


/* get max size of available RAM in TT RAM or ST RAM */
LONG dos_avail_anyram(void)
{
    return Mxalloc(-1L,MX_PREFTTRAM);
}


WORD dos_free(void *maddr)
{
    return Mfree(maddr);
}


WORD dos_shrink(void *maddr, LONG length)
{
    return Mshrink(maddr,length);
}


/*
 *  Routine to load in (part of) a file
 *
 *  returns: >=0  number of bytes read
 *           < 0  error code from dos_open()/dos_read()
 */
LONG dos_load_file(char *filename, LONG count, char *buf)
{
    WORD    fh;
    LONG    ret;

    ret = dos_open(filename, 0);
    if (ret >= 0L)
    {
        fh = (WORD)ret;
        ret = dos_read(fh, count, buf);
        dos_close(fh);
    }

    return ret;
}
