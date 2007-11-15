/*      GEMINIT.C       4/23/84 - 08/14/85      Lee Lorenzen            */
/*      GEMCLI.C        1/28/84 - 08/14/85      Lee Jay Lorenzen        */
/*      GEM 2.0         10/31/85                Lowell Webster          */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */ 
/*      fix command tail handling               10/19/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002, 2007 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "taddr.h"
#include "struct.h"
#include "basepage.h"
#include "gemlib.h"
#include "crysbind.h"
#include "gem_rsc.h"
#include "dos.h"
#include "xbiosbind.h"

#include "gemgsxif.h"
#include "gemdosif.h"
#include "gemctrl.h"
#include "gemshlib.h"
#include "gempd.h"
#include "gemdisp.h"
#include "gemrslib.h"
#include "gemobed.h"
#include "gemdos.h"
#include "gemgraf.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemfslib.h"
#include "gemoblib.h"
#include "gemsclib.h"
#include "gemfmlib.h"
#include "gemasm.h"
#include "gemaplib.h"
#include "gemsuper.h"
#include "geminput.h"
#include "gemmnlib.h"
#include "geminit.h"
#include "optimize.h"
#include "optimopt.h"

#include "string.h"
#include "ikbd.h"
#include "kprint.h"     // just for debugging


#define ROPEN 0

#define ARROW 0
#define HGLASS 2

#define PATH_LEN 0x40                   /* path length defined by PC_DOS*/

#define IG_HARD 0
#define SIZE_AFILE 2048
#if SINGLAPP
#define INF_SIZE 300                    /* Number of bytes to read of   */
                                        /*  the desktop.inf file.       */
#else
#define INF_SIZE 2048
#endif


#if MULTIAPP
GLOBAL WORD     gl_numaccs;
GLOBAL ACCNODE  gl_caccs[3];            /* max 3 accessories    */
#endif

                                                /* in GEMINIT.C         */
#if I8086
EXTERN VOID     start();
#endif
#if MC68K
static BYTE     start[SIZE_AFILE];              /* can't play the same  */
                                                /* trick in 68k land    */
#endif

/* Some global variables: */

GLOBAL WORD     totpds;

GLOBAL LONG     ad_valstr;

GLOBAL LONG     ad_sysglo;
GLOBAL LONG     ad_armice;
GLOBAL LONG     ad_hgmice;
GLOBAL LONG     ad_mouse;
GLOBAL LONG     ad_envrn;               /* initialized in GEMSTART      */
GLOBAL LONG     ad_stdesk;

GLOBAL BYTE     gl_dta[128];
GLOBAL BYTE     gl_dir[130];
GLOBAL BYTE     gl_1loc[256];
GLOBAL BYTE     gl_2loc[256];
GLOBAL BYTE     pqueue[128];
GLOBAL BYTE     usuper[128];
GLOBAL WORD     gl_mouse[37];
GLOBAL LONG     ad_scdir;
GLOBAL BYTE     gl_logdrv;

GLOBAL PD       *rlr, *drl, *nrl;
GLOBAL EVB      *eul, *dlr, *zlr;

#if I8086
GLOBAL UWORD    elinkoff;
#else
GLOBAL LONG     elinkoff;
#endif
GLOBAL BYTE     indisp;

GLOBAL WORD     fpt, fph, fpcnt;                /* forkq tail, head,    */
                                                /*   count              */
GLOBAL SPB      wind_spb;
GLOBAL WORD     curpid;

GLOBAL THEGLO   D;



/*
*       Convert a single hex ASCII digit to a number
*/
WORD hex_dig(BYTE achar)
{
        if ( (achar >= '0') && (achar <= '9') )
          return(achar - '0');  
        else
        {
          achar = toupper(achar);
          if ( (achar >= 'A') && (achar <= 'F') )
             return(achar - 'A' + 10);  
          else
            return(NULL);
        }
}


/*
*       Scan off and convert the next two hex digits and return with
*       pcurr pointing one space past the end of the four hex digits
*/
BYTE *scan_2(BYTE *pcurr, UWORD *pwd)
{
        UWORD   temp;

        if (*pcurr==' ')
          pcurr += 1;

        temp = 0x0;
        temp |= hex_dig(*pcurr++) << 4;
        temp |= hex_dig(*pcurr++);
        if (temp == 0x00ff)
          temp = NIL;
        *pwd = temp;

        return( pcurr );
}


static void ini_dlongs(void)
{
/*
#if I8086
        REG LONG        ad_dseg;
#endif
*/
#if MULTIAPP
        WORD            ii;
#endif
        register BYTE   *ps;
        
                                                /* use all of this      */
                                                /*   initialization     */
                                                /*   code area for the  */
                                                /*   save area, must be */
                                                /*   on para. boundary  */
                                                /*   so new environment */
                                                /*   can be copied here */
#if I8086
        /*        
                  ad_ssave = LLCS() + LW(start);
                  // init. long pointer   
                  //   to global array    
                  //   which is used by   
                  //   resource calls     
                  ad_dseg = ADDR(&D.g_sysglo[0]) & 0xFFFF0000L;
                  ad_sysglo = ad_dseg + LW(&D.g_sysglo[0]);
                  ad_windspb = ad_dseg + LW(&wind_spb);
                  ad_mouse = ad_dseg + LW(&gl_mouse[0]);
                  // gemfslib longs       *
                  ad_tmp1 = ad_dseg + LW(&gl_tmp1[0]);
                  ad_tmp2 = ad_dseg + LW(&gl_tmp2[0]);
                  // gemrslib             
                  ad_hdrbuff = ad_dseg + LW(&hdr_buff[0]);
                  // gemoblib             
                  ad_valstr = ad_dseg + LW(&D.g_valstr[0]);
                  ad_fmtstr = ad_dseg + LW(&D.g_fmtstr[0]);
                  ad_rawstr = ad_dseg + LW(&D.g_rawstr[0]);
                  ad_tmpstr = ad_dseg + LW(&D.g_tmpstr[0]);
                  ad_edblk = ad_dseg + LW(&edblk);
                  ad_bi = ad_dseg + LW(&bi);
                  ad_ib = ad_dseg + LW(&ib);
                  
                  D.s_cmd = (BYTE *) &pqueue[0];
                  ad_scmd = ad_dseg + LW(D.s_cmd);
                  // put scrap and some   
                  //   other arrays at    
                  //   at top of the      
                  //   screen mgr stack   
                  ps = D.g_scrap = (BYTE *) &usuper[0];
                  ad_scrap = ad_dseg + LW(ps);
                  D.s_cdir = ps += 82;
                  ad_scdir = ad_dseg + LW(ps);
                  D.g_loc1 = ps = &gl_1loc[0];
                  ad_g1loc = ad_dseg + LW(ps);
                  D.g_loc2 = ps = &gl_2loc[0];
                  ad_g2loc = ad_dseg + LW(ps);
                  D.g_dir = ps = &gl_dir[0];
                  ad_path = ad_dseg + LW(ps);
                  D.g_dta = ps = &gl_dta[0];
                  ad_dta = ad_dseg + LW(ps);
                  ad_fsdta = ad_dseg + LW(&gl_dta[30]);
        */
#endif
#if MC68K
                                                /* init. long pointer   */
                                                /*   to global array    */
                                                /*   which is used by   */
                                                /*   resource calls     */
        ad_ssave = ADDR(&start);
        ad_sysglo = ADDR(&D.g_sysglo[0]);
        ad_windspb = ADDR(&wind_spb);
        ad_mouse = ADDR(&gl_mouse[0]);
                                                /* gemoblib             */
        ad_valstr = ADDR(&D.g_valstr[0]);
        ad_fmtstr = ADDR(&D.g_fmtstr[0]);
        ad_rawstr = ADDR(&D.g_rawstr[0]);
        ad_tmpstr = ADDR(&D.g_tmpstr[0]);
        ad_edblk = ADDR(&edblk);
        ad_bi = ADDR(&bi);
        ad_ib = ADDR(&ib);

        D.s_cmd = (BYTE *) &pqueue[0];
        ad_scmd = ADDR(D.s_cmd);
                                                /* put scrap and some   */
                                                /*   other arrays at    */
                                                /*   at top of the      */
                                                /*   screen mgr stack   */
        ps = D.g_scrap = (BYTE *) &usuper[0];
        ad_scrap = ADDR(ps);
        D.s_cdir = ps += 82;
        ad_scdir = ADDR(ps);
        D.g_loc1 = &gl_1loc[0];
        D.g_loc2 = &gl_2loc[0];
        D.g_dir = ps = &gl_dir[0];
        ad_path = ADDR(ps);
        D.g_dta = ps = &gl_dta[0];
        ad_dta = ADDR(ps);
        ad_fsdta = ADDR(&gl_dta[30]);
#endif

#if MULTIAPP
        for (ii = 0; ii < NUM_PDS; ii++)
        {
          gl_mnpds[ii] = 0x0;
          desk_root[ii] = 0x0;
          menu_tree[ii] = 0x0L;
          desk_tree[ii] = 0x0L;
        }
        for (ii = 0; ii < NUM_DESKACC; ii++)
        {  
          desk_ppd[ii] = NULLPTR;
          desk_acc[ii] = 0x0L;
        }
#endif

}


/*
 *#if I8086
 *
 *        WORD
 *sizeglo()*
 *{
 *        return(sizeof(THEGLO) );
 *}
 *#endif
 */
     
LONG size_theglo(void)
{
    return( sizeof(THEGLO)/2 );
}



/*
*       called from startup code to get the stack for process 0
*/
/*
WORD sizeuda(void)
{
        return( sizeof(UDA) - sizeof(WORD) );
}
*/


static void ev_init(EVB evblist[], WORD cnt)
{
        WORD            i;

        for(i=0; i<cnt; i++)
        {
          evblist[i].e_nextp = eul;
          eul = &evblist[i];
        }
}


/*
*       Create a local process for the routine and start him executing.
*       Also do all the initialization that is required.
* TODO - get rid of this.
*/
static PD *iprocess(BYTE *pname, void (*routine)())
{
        register ULONG  ldaddr;

        /* figure out load addr */

        /* #if I8086
         * ldaddr = LLCS() + ((ULONG) routine);
         * #endif
         */

        ldaddr = (ULONG) routine;

        /* create process to execute it */
        return( pstart(routine, pname, ldaddr) );
}


/*
*       Start up the file selector by initializing the fs_tree
*/
static void fs_start(void)
{
        LONG            tree;

#ifdef USE_GEM_RSC
        rs_gaddr(ad_sysglo, R_TREE, FSELECTR, &tree);
#else
        tree = (LONG) rs_tree[FSELECTR];
#endif
        ad_fstree = tree;
        ob_center(tree, &gl_rfs);
}


#if SINGLAPP
/*
*       Routine to load program file pointed at by pfilespec, then
*       create new process context for it.  This uses the load overlay
*       function of DOS.  The room for accessories variable will be
*       decremented by the size of the accessory image.  If the
*       accessory is too big to fit it will be not be loaded.
*/

static void sndcli(BYTE *pfilespec, WORD *paccroom)   /* paccroom in paragraphs*/
{
        register WORD   handle;
        WORD            err_ret;
        LONG            ldaddr;


        strcpy(&D.s_cmd[0], pfilespec);

        handle = dos_open( (BYTE *)ad_scmd, ROPEN );
        if (!DOS_ERR)
        {
          err_ret = pgmld(handle, &D.s_cmd[0], (LONG **)(void*)&ldaddr /*, paccroom*/);
          dos_close(handle);
                                                /* create process to    */
                                                /*   execute it         */
          if (err_ret != -1)
            pstart(gotopgm, pfilespec, ldaddr);
        }
}



/*
*       Routine to load in desk accessory's.  Files by the name of *.ACC
*       will be loaded, if there is sufficient room for them in the system.
*       The number that this gives is the room available for desk accessories.
*
*       0x20000         128 kb for app
*       0x01000           4 kb for virt wk station
*       0x00800           2 kb for file selector
*       0x02000           8 kb for fudge factor
*       -------
*       0x23800
*
*       FOR GEM2.0 max is 192kb
*       0x33800
*/
static void ldaccs(void)
{
        register WORD   i;
        WORD            ret;
        ULONG           laccroom;
        UWORD           accroom;

        laccroom = dos_avail() - 0x00033800L - gsx_mcalc();
        accroom = (laccroom + 0x0000000fL) >> 4;
        strcpy(&D.g_dir[0], rs_str(STACC));
        dos_sdta(ad_dta);

        /* if Control is held down then skip loading of accs */
        if ((kbshift(-1) & (1<<2)))
          return;

        ret = TRUE;
        for(i=0; (i<NUM_ACCS) && (accroom > 0) && (ret); i++)
        {

          ret = (i==0) ? dos_sfirst(ad_path, F_RDONLY) : dos_snext();
          if (ret)
            sndcli(&gl_dta[30], &accroom);
        }
}
#endif



static void sh_addpath(void)
{
        LONG    lp, np, new_envr;
        WORD    oelen, oplen, nplen, fstlen;
        BYTE    tmp;
        char    tmpstr[MAX_LEN];

        lp = ad_envrn;
                                                /* get to end of envrn  */
        while ( (tmp = LBGET(lp)) != 0  ||      /* ends with 2 nulls    */
                (LBGET(lp+1)) )
          lp++;
        lp++;                                   /* past 2nd null        */
                                                /* old evironment length*/
        oelen = (lp - ad_envrn) + 2;
                                                /* new path length      */
#ifdef USE_GEM_RSC
        rs_gaddr(ad_sysglo, R_STRING, STINPATH, &np);
#else
        strcpy(tmpstr, rs_fstr[STINPATH]);
        np = (LONG) tmpstr;
#endif
        nplen = LSTRLEN(np);
                                                /* fix up drive letters */
        lp = np;
        while ( (tmp = LBGET(lp)) != 0 )
        {
          if (tmp == ':')
            LBSET(lp - 1, gl_logdrv);
          lp++;
        }
                                                /* alloc new environ    */
        new_envr = ad_ssave;
        ad_ssave += LW(oelen + nplen);
                                                /* get ptr to initial   */
                                                /*   PATH=              */
        sh_envrn(ADDR(&lp), ADDR(rs_str(STPATH)));

        if(lp)
        {
                                                /* first part length    */
          oplen = LSTRLEN(lp);                  /* length of actual path */

          fstlen = lp - ad_envrn + oplen;       /* len thru end of path */
          LBCOPY(new_envr, ad_envrn, fstlen);
        }
        else
        {
          oplen = fstlen = 0;
        }

        if (oplen)
        {
          LBSET(new_envr + fstlen, ';');        /* to splice in new path */
          fstlen += 1;
        }

        LBCOPY(new_envr + fstlen, np, nplen);   /* splice on more path  */
                                                /* copy rest of environ */
        if(lp)
        {
          LBCOPY(new_envr + fstlen + nplen, lp + oplen, oelen - fstlen);
        }

        ad_envrn = new_envr;                    /* remember new environ.*/
}




void sh_deskf(WORD obj, LONG plong)
{
        register LONG   tree;

        tree = ad_stdesk;
        LLSET(plong, LLGET(OB_SPEC(obj)));
}



static void sh_init(void)
{
        WORD    cnt, need_ext;
        BYTE    *psrc, *pdst, *pend;
        BYTE    *s_tail;
        SHELL   *psh;
        BYTE    savch;

#if MULTIAPP
        gl_pids = 0;
#endif
        psh = &sh[0];

        sh_deskf(2, ADDR(&ad_pfile));
                                                /* add in internal      */
                                                /*   search paths with  */
                                                /*   right drive letter */
        
        sh_addpath();
                                                /* set defaults         */
        psh->sh_doexec = psh->sh_dodef = gl_shgem
                 = psh->sh_isgem = TRUE;
        psh->sh_fullstep = FALSE;

#if SINGLAPP
                                                /* parse command tail   */
                                                /*   that was stored in */
                                                /*   geminit            */
        psrc = s_tail = &D.g_dir[0];            /* reuse part of globals*/
        LBCOPY(ADDR(&s_tail[0]), ad_stail, 128);
        cnt = *psrc++;

        if (cnt)
        {
                                                /* null-terminate it    */
          pend = psrc + cnt;
          *pend = NULL;
                                                /* scan off leading     */
                                                /*   spaces             */
          while( (*psrc) &&
                 (*psrc == ' ') )
            psrc++;
                                                /* if only white space  */
                                                /*   get out don't      */
                                                /*   bother parsing     */
          if (*psrc)
          {
            pdst = psrc;
            while ( (*pdst) && (*pdst != ' ') )
              pdst++;                           /* find end of app name */

                                                /* save command to do   */
                                                /*   instead of desktop */
            savch = *pdst;
            *pdst = '\0';                       /* mark for sh_name()   */
            pend = sh_name(psrc);               /* see if path also     */
            *pdst = savch;                      /* either blank or null */      
            pdst = &D.s_cmd[0];
            if (pend != psrc)
            {
              if (*(psrc+1) != ':')             /* need drive           */
              {
                *pdst++ = gl_logdrv;            /* current drive        */
                *pdst++ = ':';
                if (*psrc != '\\')
                  *pdst++ = '\\';
              }
              while (psrc < pend)               /* copy rest of path    */
                *pdst++ = *psrc++;
              if (*(pdst-1) == '\\')            /* back up one char     */
                pdst--;
              *pdst = '\0';
              pend = &D.s_cmd[0];
              while (*pend)                     /* upcase the path      */
              {
                *pend = toupper(*pend);
                pend++;
              }
              dos_sdrv(D.s_cmd[0] -'A');
              dos_chdir((BYTE *)ad_scmd);
              *pdst++ = '\\';
            }
            need_ext = TRUE;
            while ( (*psrc) &&
                    (*psrc != ' ') )
            {
              if (*psrc == '.')
                need_ext = FALSE;
              *pdst++ = *psrc++;
            }
                                                /* append .APP if no    */
                                                /*   extension given    */
            if (need_ext)
              strcpy(pdst, rs_str(STGEM));
            else
              *pdst = NULL;
            pdst = &D.s_cmd[0];
            while (*pdst)                       /* upcase the command   */
            {
              *pdst = toupper(*pdst);
              pdst++;
            }

            psh->sh_dodef = FALSE;
                                                /* save the remainder   */
                                                /*   into command tail  */
                                                /*   for the application*/
            pdst = &s_tail[1];
/*          if ( (*psrc) &&                     * if tail then take     *
               (*psrc != 0x0D) &&               *  out first space      *
               (*psrc == ' ') )
                  psrc++;
*/
            if (*psrc == ' ')
              psrc++;
                                              /* the batch file allows  */
                                              /*  three arguments       */
                                              /*  one for a gem app     */
                                              /*  and 2 for arguments   */
                                              /*  to the gem app.       */
                                              /*  if there are < three  */
                                              /*  there will be a space */
                                              /*  at the end of the last*/
                                              /*  arg followed by a 0D  */
            while ( (*psrc) && 
                    (*psrc != 0x0D) &&
                    (*psrc != 0x09) &&          /* what is this??       */
                    !((*psrc == '/') && (toupper(*(psrc+1)) == 'D')) )
            {
              if ( (*psrc == ' ') &&
                   ( (*(psrc+1) == 0x0D) ||
                     (*(psrc+1) == NULL)) )
                psrc++;
              else
                *pdst++ = toupper(*psrc++);
            }
            *pdst = NULL;
            s_tail[0] = strlen(&s_tail[1]);
                                                /* don't do the desktop */
                                                /*   after this command */
                                                /*   unless a /d was    */
                                                /*   encounterd         */
            psh->sh_doexec = (toupper(*(psrc+1)) == 'D');
          }
        }
        LBCOPY(ad_stail, ADDR(&s_tail[0]), 128);
#endif
}



/*
*       Routine to read the desktop.inf file from the \gemdesk
*       directory and set the bvdisk and bvhard variables
*       so that apps and accessories can always use this data.
*/
void sh_rdinf(void)
{
        WORD    fh, size, ishdisk;
        LONG    pcurr;
        WORD    bvdisk, bvhard, bvect, env;
        char    *pfile;
        BYTE    tmp;
        WORD    i;
        char    tmpstr[MAX_LEN];

#ifdef USE_GEM_RSC
        rs_gaddr(ad_sysglo, R_STRING, STINFPAT, &pfile);
#else
        strcpy(tmpstr, rs_fstr[STINFPAT]);
        pfile = tmpstr;
#endif
        LBSET(pfile, D.s_cdir[0] );             /* set the drive        */

        fh = dos_open((BYTE *)pfile, ROPEN);
        if ( (!fh) || DOS_ERR)
          return;
                                                /* NOTE BENE all disk info */
                                                /*  MUST be within INF_SIZE*/
                                                /*  bytes from beg of file */
        size = dos_read(fh, INF_SIZE, ad_ssave);
        dos_close(fh);
        if (DOS_ERR)
          return;
        pcurr = ad_ssave;
        bvdisk = bvhard = 0x0;
#if MULTIAPP
        gl_numaccs = 0x0;
#endif
        LBSET(pcurr + (ULONG)size, NULL);       /* set end to NULL      */
        while ( LBGET(pcurr) != NULL)
        {
          if ( LBGET(pcurr++) != '#' )
            continue;
          tmp = LBGET(pcurr);
          if (tmp == 'M')               /* #M 00 00 01 FF B FLOPPY DISK@ @ */
          {
            pcurr += 8;                 /* convert the icon number      */
            scan_2((BYTE *)pcurr, &ishdisk);
            pcurr += 6;                 /* get the disk letter          */
            bvect = ((UWORD) 0x8000) >> ((UWORD) ( LBGET(pcurr) - 'A'));
            bvdisk |= bvect;
            if (ishdisk == IG_HARD)
              bvhard |= bvect;
          }
          else if (tmp == 'E')          /* #E 3A 11                     */
          {                             /* desktop environment          */
            pcurr += 2;
            scan_2((BYTE *)pcurr, &env);
            ev_dclick(env & 0x07, TRUE);
            pcurr += 3;
            scan_2((BYTE *)pcurr, &env);
            gl_mnclick = ((env & 0x08) != 0);
            sound(FALSE, !(env & 0x01), 0);
          }
          else if (tmp == 'Z')      /* something like "#Z 01 C:\THING.APP@" */
          {
            BYTE *tmpptr1, *tmpptr2;
            pcurr += 5;
            tmpptr1 = (BYTE *)pcurr;
            while (LBGET(pcurr) != 0 && LBGET(pcurr) != '@')
              ++pcurr;
            *(BYTE *)pcurr = 0;
            tmpptr2 = sh_name(tmpptr1);
            *(tmpptr2-1) = 0;
            kprintf("Found #Z entry in EMUDESK.INF with path=%s and prg=%s\n",
                    tmpptr1, tmpptr2);
            sh_wdef((LONG)tmpptr2, (LONG)tmpptr1);
            ++pcurr;
          }
#if MULTIAPP
          else if (tmp == 'A')          /* test for accessories */
          {                             /* #A 59 CALCLOCK.ACC@  */
            BYTE  *pstr;
            pcurr++;
            scan_2((BYTE *)pcurr, &(gl_caccs[gl_numaccs].acc_swap));
            pcurr += 4;
            pstr = &gl_caccs[gl_numaccs].acc_name[0];
            while( (tmp = LBGET(pcurr++)) != '@')
              *pstr++ = tmp;
            *pstr = NULL;
            gl_numaccs++;
          }
#endif    
        }
        gl_bvdisk = bvdisk;
        gl_bvhard = bvhard;
                                        /* clean up tmp buffer          */
        pcurr = ad_ssave;
        for(i = 0; i < (INF_SIZE / 2); i++)
        {
          LWSET(pcurr, 0x0);
          pcurr += 2;
        }
}



/*
 * Give everyone a chance to run, at least once
 */
void all_run(void)
{
    WORD  i;

    /* let all the acc's run*/
    for(i=0; i<NUM_ACCS; i++)
    {
        dsptch();
    }
    /* then get in the wait line */
    wm_update(TRUE);
    wm_update(FALSE);
}



void gem_main(void)
{
    WORD    i;
    LONG    tmpadbi;
#if MULTIAPP
    for (i=0; i<NUM_PDS; i++)
        sh[i].sh_loadable = FALSE;
#endif

    totpds = NUM_PDS;
    ml_ocnt = 0;

    gl_changerez = FALSE;

    ini_dlongs();               /* init longs */
#if I8086
    cli();                      /* no ticks during init */
#endif
#if MC68K
    hcli();
#endif
    takecpm();                  /* take the 0efh int. */

    /* init event recorder  */
    gl_recd = FALSE;
    gl_rlen = 0;
    gl_rbuf = 0x0L;
    /* initialize pointers to heads of event list and thread list */
    elinkoff = (BYTE *) &(D.g_intevb[0].e_link) - (BYTE *) &(D.g_intevb[0]);

    /* link up all the evb's to the event unused list */
    eul = NULLPTR;
    ev_init(&D.g_intevb[0], NUM_IEVBS);
    if (totpds > 2)
        ev_init(&D.g_extevb[0], NUM_EEVBS);

    /* initialize sync blocks */
    wind_spb.sy_tas = 0;
    wind_spb.sy_owner = NULLPTR;
    wind_spb.sy_wait = 0;

    /*
     * init processes - TODO: should go in gempd or gemdisp.
     */

    /* initialize list and unused lists   */
    nrl = drl = NULLPTR;
    dlr = zlr = NULLPTR;
    fph = fpt = fpcnt = 0;

    /* init initial process */
    for(i=totpds-1; i>=0; i--)
    {
        rlr = pd_index(i);
        if (i < 2)
        {
            rlr->p_uda = &D.g_intuda[i];
            rlr->p_cda = &D.g_intcda[i];
        }
        else
        {
            rlr->p_uda = &D.g_extuda[i-2];
            rlr->p_cda = &D.g_extcda[i-2];
        }
        rlr->p_qaddr = ADDR(&rlr->p_queue[0]);
        rlr->p_qindex = 0;
        memset(rlr->p_name, ' ', 8);
        /* if not rlr then initialize his stack pointer */
        if (i != 0)
            rlr->p_uda->u_spsuper = &rlr->p_uda->u_supstk;
        rlr->p_pid = i;
        rlr->p_stat = 0;
    }
    curpid = 0;
    rlr->p_pid = curpid++;
    rlr->p_link = NULLPTR;

    /* end of process init */

    /* restart the tick     */
#if I8086
    sti();
#endif
#if MC68K
    hsti();
#endif
    /*
     * screen manager process init. this process starts out owning the mouse
     * and the keyboard. it has a pid == 1
     */
    gl_dacnt = 0;
    gl_mowner = ctl_pd = iprocess("SCRENMGR", ctlmgr);

#if MULTIAPP
    if (totpds > 2)
    {
        for (i=0; i<NUM_ACCS; i++)
            iprocess("AVAILNUL", &nulmgr);
    }
#endif

    /* load gem resource and fix it up before we go */
#ifndef USE_GEM_RSC
    gem_rsc_init();
#else
    if ( !rs_readit(ad_sysglo, ADDR("GEM.RSC")) )
    {
        /* bad resource load, so dive out */
        cprintf("gem_main: failed to load GEM.RSC...\n");
    }
    else
#endif
    {
        /* get mice forms       */
#ifdef USE_GEM_RSC
        rs_gaddr(ad_sysglo, R_BIPDATA, 3 + ARROW, &ad_armice);
        rs_gaddr(ad_sysglo, R_BIPDATA, 3 + HGLASS, &ad_hgmice);
#else
        ad_armice = (LONG) &rs_fimg[3+ARROW];
        ad_hgmice = (LONG) &rs_fimg[3+HGLASS];
#endif
        ad_armice = LLGET(ad_armice);
        ad_hgmice = LLGET(ad_hgmice);

        /* init button stuff    */
        gl_btrue = 0x0;
        gl_bdesired = 0x0;
        gl_bdely = 0x0;
        gl_bclick = 0x0;

        gl_logdrv = dos_gdrv() + 'A';   /* boot directory       */
        gsx_init();                     /* do gsx open work station */

#if SINGLAPP
        /* load all desk acc's  */
        if (totpds > 2)
            ldaccs();
#endif
        /* fix up icons         */
        for(i=0; i<3; i++) {
#ifdef USE_GEM_RSC
            rs_gaddr(ad_sysglo, R_BITBLK, i, &tmpadbi);
#else
            tmpadbi = (LONG) &rs_fimg[i];
#endif
            LBCOPY(ad_bi, tmpadbi, sizeof(BITBLK));
            gsx_trans(bi.bi_pdata, bi.bi_wb, bi.bi_pdata, bi.bi_wb, bi.bi_hl);
        }

        /* take the critical err handler int. */
        cli();
        takeerr();
        sti();

        /* go into graphic mode */
        sh_tographic();

        /* take the tick int.   */
        cli();
        gl_ticktime = gsx_tick(tikaddr, &tiksav);
        sti();

        /* set init. click rate */
        ev_dclick(3, TRUE);

        /* fix up the GEM rsc. file now that we have an open WS */
#ifdef USE_GEM_RSC
        rs_fixit(ad_sysglo);

        /* get st_desk ptr */
        rs_gaddr(ad_sysglo, R_TREE, 2, &ad_stdesk);
#else
        gem_rsc_fixit();

        /* get st_desk ptr */
        ad_stdesk = (LONG) rs_tree[2];
#endif
        /* init. window vars. */
        wm_start();

        /* startup gem libs */
        fs_start();

        /* remember current desktop directory */
        sh_curdir(ad_scdir);

        /* read the desktop.inf */
        /* 2/20/86 LKW          */
#if SINGLAPP
        sh_rdinf();
#endif
        /* off we go !!!        */
        dsptch();

        /* let them run         */
        all_run();

        /*
         * init for shell loop up thru here it is okay for system to
         * overlay this initialization code
         */
        sh_init();

        /*
         * main shell loop. From here on down data should not overlay
         * this code
         */
        sh_main();

#if MULTIAPP
        /* fixup memory */
        pr_load(SCR_MGR);
#endif
        /* free up resource space */
#ifdef USE_GEM_RSC
        rs_free(ad_sysglo);
#endif
        /* give back the tick   */
        cli();
        gl_ticktime = gsx_tick(tiksav, &tiksav);
        sti();

        /* close workstation    */
        gsx_wsclose();

        if (gl_changerez)
        {
            /* Change resolution before starting over again... */
            if (gl_changerez == 1)
            {
                Setscreen(-1L, -1L, gl_nextrez-2);
            }
            /* TODO: Support for Falcon screen modes */
        }
    }

    /* return GEM's 0xEF int*/
    cli();
    givecpm();
    sti();
}


