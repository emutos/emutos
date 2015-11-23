/*      JDOS.C          11/12/84 - 04/14/85     Lowell Webster          */
/*      GEMDOSIF.C      5/15/85 - 6/4/85        MDF                     */

/*
*       Copyright (C) 2002-2015 The EmuTOS development team
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

#include "config.h"
#include "portab.h"
#include "string.h"
#include "asm.h"
#include "gemdos.h"
#include "dta.h"


GLOBAL WORD     DOS_AX; /* really a "DOS_RET"   */
GLOBAL UWORD    DOS_ERR;

/*
 *  K&R prototype to avoid prototype mismatch warnings
 *  with different arguments.
 */
extern LONG gemdos();


#define X_TABOUT 0x02
#define X_PRTOUT 0x05
#define X_RAWCON 0x06
#define X_RAWCIN 0x07
#define X_CONWS  0x09
#define X_CONIS  0x0B
#define X_SETDRV 0x0E
#define X_GETDRV 0x19
#define X_SETDTA 0x1A
#define X_GETDTA 0x2F
#define X_GETFREE 0x36
#define X_MKDIR 0x39
#define X_RMDIR 0x3A
#define X_CHDIR 0x3B
#define X_CREAT 0x3C
#define X_OPEN 0x3D
#define X_CLOSE 0x3E
#define X_READ 0x3F
#define X_WRITE 0x40
#define X_UNLINK 0x41
#define X_LSEEK 0x42
#define X_CHMOD 0x43
#define X_GETDIR 0x47
#define X_MALLOC 0x48
#define X_MFREE 0x49
#define X_SETBLOCK 0x4A
#define X_EXEC 0x4B
#define X_SFIRST 0x4E
#define X_SNEXT 0x4F
#define X_RENAME 0x56
#define X_GSDTOF 0x57



WORD pgmld(WORD handle, BYTE *pname, LONG **ldaddr)
{
    LONG    length, ret;
    LONG    *temp;

    ret = gemdos(X_EXEC, 3, pname, "", NULL);
    if (ret < 0L)
        return -1;

    *ldaddr = (LONG *) ret;

    /* program length = code+data+bss lengths plus basepage length */
    temp = *ldaddr;
    length = temp[3] + temp[5] + temp[7] + 0x100;
    if (gemdos(X_SETBLOCK, 0, *ldaddr, length) < 0L)
        return -1;

    return 0;
}


/*
void chrout(WORD chr)
{
    return( gemdos(X_TABOUT,chr) );
}
*/

/*
BYTE rawcon(WORD parm)
{
    return( (BYTE)gemdos(X_RAWCON, parm) );
}
*/

/*
void prt_chr(WORD chr)
{
    return( gemdos(X_PRTOUT,chr) );
}
*/


/*
void dos_func(UWORD function, LONG parm)
{
    return( gemdos(function,parm) );
}
*/


void dos_conout(WORD ch)
{
    gemdos(X_TABOUT,ch);
}


LONG dos_rawcin(void)
{
    return gemdos(X_RAWCIN);
}


WORD dos_conis(void)
{
    return gemdos(X_CONIS);
}


void dos_conws(const char *str)
{
    gemdos(X_CONWS,str);
}


WORD dos_gdrv(void)
{
    return gemdos(X_GETDRV);
}


void dos_sdta(void *ldta)
{
    gemdos(X_SETDTA,ldta);
}


void *dos_gdta(void)
{
    return (void *)gemdos(X_GETDTA);
}


WORD dos_sfirst(BYTE *pspec, WORD attr)
{
    return !gemdos(X_SFIRST,pspec,attr);
}


WORD dos_snext(void)
{
    return !gemdos(X_SNEXT);
}


LONG dos_open(BYTE *pname, WORD access)
{
    return gemdos(X_OPEN,pname,access);
}


WORD dos_close(WORD handle)
{
    return gemdos(X_CLOSE,handle);
}


LONG dos_read(WORD handle, LONG cnt, void *pbuffer)
{
    return gemdos(X_READ,handle,cnt,pbuffer);
}


LONG dos_write(WORD handle, LONG cnt, void *pbuffer)
{
    return gemdos(X_WRITE,handle,cnt,pbuffer);
}


LONG dos_lseek(WORD handle, WORD smode, LONG sofst)
{
    return gemdos(X_LSEEK,sofst, handle, smode);
}


LONG dos_chdir(BYTE *pdrvpath)
{
    return gemdos(X_CHDIR,pdrvpath);
}


WORD dos_gdir(WORD drive, BYTE *pdrvpath)
{
    return gemdos(X_GETDIR,pdrvpath,drive);
}


LONG dos_sdrv(WORD newdrv)
{
    return gemdos(X_SETDRV,newdrv);
}


LONG dos_create(BYTE *name, WORD attr)
{
    return gemdos(X_CREAT,name,attr);
}


WORD dos_mkdir(BYTE *path)
{
    gemdos(X_MKDIR,path);
    return !DOS_ERR;
}


WORD dos_chmod(BYTE *name, WORD wrt, WORD mod)
{
    return gemdos(X_CHMOD,name,wrt,mod);
}


WORD dos_setdt(UWORD h, UWORD time, UWORD date)
{
    UWORD   buf[2];

    buf[0] = time;
    buf[1] = date;
    return gemdos(X_GSDTOF,&buf[0],h,TRUE);
}


WORD dos_label(BYTE drive, BYTE *plabel)
{
    DTA     dta;
    BYTE    path[8];

    gemdos(X_SETDTA,&dta);
    strcpy(path, " :\\*.*");
    path[0] = (drive + 'A') - 1;
    if (!gemdos(X_SFIRST,path,0x08))
    {
        strcpy(plabel,dta.d_fname);
        return TRUE;
    }
    else
        return FALSE;
}


LONG dos_delete(BYTE *name)
{
    return gemdos(X_UNLINK,name);
}


void dos_space(WORD drv, LONG *ptotal, LONG *pavail)
{
    LONG    buf[4];
    LONG    mult;

    *ptotal = *pavail = 0L;

    if (gemdos(X_GETFREE,buf,drv) < 0L) /* 0=default, 1=A for gemdos */
        return;

    mult = buf[3] * buf[2];
    *ptotal = mult * buf[1];
    *pavail = mult * buf[0];
}


WORD dos_rename(BYTE *p1, BYTE *p2)
{
    return gemdos(X_RENAME,0x0,p1,p2);
}


WORD dos_rmdir(BYTE *path)
{
    return gemdos(X_RMDIR,path);
}



LONG dos_alloc(LONG nbytes)
{
    register LONG ret;

    ret = gemdos(X_MALLOC,nbytes);
    if (ret == 0)
        DOS_ERR = TRUE;             /* gemdos() sets it to FALSE    */
    return ret;
}


/*
 *  Returns the amount of memory available in bytes
 */
LONG dos_avail(void)
{
    return gemdos(X_MALLOC,-1L);
}


WORD dos_free(LONG maddr)
{
    return gemdos(X_MFREE,maddr);
}


WORD dos_shrink(void *maddr, LONG length)
{
    return gemdos(X_SETBLOCK,0,maddr,length);
}
