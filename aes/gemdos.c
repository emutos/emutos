/*      JDOS.C          11/12/84 - 04/14/85     Lowell Webster          */
/*      GEMDOSIF.C      5/15/85 - 6/4/85        MDF                     */

/*
*       Copyright (C) 2002 The EmuTOS development team
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

#include "portab.h"
#include "machine.h"


GLOBAL UWORD    DOS_AX; /* really a "DOS_RET"   */
GLOBAL UWORD    DOS_ERR;

extern LONG gemdos();
extern LONG do_pexec(WORD mode, LONG p1, LONG p2, LONG p3);  /* in gemstart.S */


#define X_TABOUT 0x02
#define X_PRTOUT 0x05
#define X_RAWCON 0x06
#define X_SETDRV 0x0E
#define X_GETDRV 0x19
#define X_SETDTA 0x1A
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
        LONG    length;
        LONG    *temp;

        *ldaddr = (LONG *) gemdos(X_EXEC, 3, pname, "", NULLPTR);
        if (!DOS_ERR)
        {                                                /* code+data+bss lengths */
          temp = *ldaddr;
          length = temp[3] + temp[5] + temp[7] + 0x100; /* and base page length */
          gemdos(X_SETBLOCK,0, *ldaddr, length);
          if (!DOS_ERR)
            return(TRUE);
          else
            return(-1);
        }
        else
          return(-1);
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


WORD dos_gdrv()
{
        return( gemdos(X_GETDRV) );
}


void dos_sdta(LONG ldta)
{
        gemdos(X_SETDTA,ldta);
}


WORD dos_sfirst(LONG pspec, WORD attr)
{
        return(!gemdos(X_SFIRST,pspec,attr));
}

WORD dos_snext()
{
        return(!gemdos(X_SNEXT));
}


WORD dos_open(BYTE *pname, WORD access)
{
        LONG            ret;

        ret = gemdos(X_OPEN,pname,access);

        if (DOS_ERR)
          return(FALSE);
        else
          return((UWORD)ret);
}

WORD dos_close(WORD handle)
{
        return( gemdos(X_CLOSE,handle) );
}


UWORD dos_read(WORD handle, UWORD cnt, LONG pbuffer)
{
        return(gemdos(X_READ,handle,(ULONG)cnt,pbuffer));
}

UWORD dos_write(WORD handle, UWORD cnt, LONG pbuffer)
{
        return(gemdos(X_WRITE,handle,(ULONG)cnt,pbuffer));
}


LONG dos_lseek(WORD handle, WORD smode, LONG sofst)
{
        return( gemdos(X_LSEEK,sofst, handle, smode) );
}



void dos_exec(LONG pcspec, LONG segenv, LONG pcmdln)
{
#if 0
        gemdos(X_EXEC, 0, pcspec, pcmdln, segenv); 
#else
        do_pexec(0, pcspec, pcmdln, segenv);
#endif
}       



LONG dos_chdir(BYTE *pdrvpath)
{
        return(gemdos(X_CHDIR,pdrvpath));
}


WORD dos_gdir(WORD drive, BYTE *pdrvpath)
{
        REG WORD ret;

        ret = gemdos(X_GETDIR,pdrvpath,drive);
        if (pdrvpath[0] == '\\')
          strcpy(pdrvpath, &pdrvpath[1]);      /* remove leading '\' */
        return(ret);
}


WORD dos_sdrv(WORD newdrv)
{
        return( gemdos(X_SETDRV,newdrv) );
}


/*
WORD isdrive()
{
        return ( dos_sdrv( dos_gdrv() ) );
}
*/


LONG dos_create(BYTE *name, WORD attr)
{
        return(gemdos(X_CREAT,name,attr));
}


WORD dos_mkdir(BYTE *path)
{
        gemdos(X_MKDIR,path);
        return( !DOS_ERR );
}


WORD dos_chmod(BYTE *name, WORD wrt, WORD mod)
{
        return( gemdos(X_CHMOD,name,wrt,mod) );
}


WORD dos_setdt(UWORD h, UWORD time, UWORD date)
{
        UWORD   buf[2];

        buf[0] = time;
        buf[1] = date;
        return( gemdos(X_GSDTOF,&buf[0],h,TRUE) );
}


WORD dos_label(BYTE drive, BYTE *plabel)
{
        BYTE    buf[50];                /* 44 bytes used        */
        BYTE    path[8];
        REG WORD i;

        for (i=0;i<50;)
          buf[i++] = 0;
        gemdos(X_SETDTA,&buf[0]);
        strcpy(path, " :\\*.*");
        path[0] = (drive + 'A') - 1;
        if (!gemdos(X_SFIRST,path,0x08))
        {
          strcpy(plabel, &buf[30]);
          return(TRUE);
        }
        else
          return(FALSE);
}


LONG dos_delete(BYTE *name)
{
        return (gemdos(X_UNLINK,name) );
}


WORD dos_space(WORD drv, LONG *ptotal, LONG *pavail)
{
        LONG    buf[4];
        LONG    mult;

        gemdos(X_GETFREE,buf,drv);      /* 0=default, 1=A for gemdos    */
        mult = buf[3] * buf[2];
        *ptotal = mult * buf[1];
        *pavail = mult * buf[0];
        return(TRUE);
}


WORD dos_rename(BYTE *p1, BYTE *p2)
{
        return(gemdos(X_RENAME,0x0,p1,p2) );
}


WORD dos_rmdir(BYTE *path)
{
        return ( gemdos(X_RMDIR,path) );
}



LONG dos_alloc(LONG nbytes)
{
        REG LONG ret;

        ret = gemdos(X_MALLOC,nbytes);
        if (ret == 0)
          DOS_ERR = TRUE;               /* gemdos() sets it to FALSE    */
        return(ret);
}


/*
*       Returns the amount of memory available in bytes
*/
LONG dos_avail()
{
        return( gemdos( X_MALLOC, -1L) );
}


WORD dos_free(LONG maddr)
{
        return( gemdos(X_MFREE,maddr) );
}

