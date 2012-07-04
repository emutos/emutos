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

#include "config.h"
#include "portab.h"
#include "compat.h"
#include "obdefs.h"
#include "taddr.h"
#include "struct.h"
#include "basepage.h"
#include "gemlib.h"
#include "crysbind.h"
#include "gem_rsc.h"
#include "dos.h"
#include "xbiosbind.h"
#include "screen.h"
#include "videl.h"

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

#define DBG_GEMINIT 0

#define ROPEN 0

#define SIZE_AFILE 2048                 /* size of AES shell buffer: must   */
                                        /*  agree with #define in deskapp.h */
#define INF_SIZE   300                  /* size of buffer used by sh_rdinf() */
                                        /*  for start of EMUDESK.INF file    */
#define ENV_SIZE   200                  /* size of initial desktop environment */

static BYTE     start[SIZE_AFILE];      /* AES shell buffer */
static BYTE     infbuf[INF_SIZE+1];     /* used to read part of EMUDESK.INF */
static BYTE     envbuf[ENV_SIZE];       /* for initial desktop environment */

/* Some global variables: */

GLOBAL WORD     totpds;

GLOBAL LONG     ad_valstr;

GLOBAL LONG     ad_sysglo;
GLOBAL LONG     ad_armice;
GLOBAL LONG     ad_hgmice;
GLOBAL LONG     ad_envrn;               /* initialized in GEMSTART      */
GLOBAL LONG     ad_stdesk;

GLOBAL BYTE     gl_dta[128];
GLOBAL BYTE     gl_dir[130];
GLOBAL BYTE     gl_1loc[256];
GLOBAL BYTE     gl_2loc[256];
GLOBAL WORD     gl_mouse[37];
GLOBAL LONG     ad_scdir;
GLOBAL BYTE     gl_logdrv;

GLOBAL PD       *rlr, *drl, *nrl;
GLOBAL EVB      *eul, *dlr, *zlr;

GLOBAL LONG     elinkoff;

GLOBAL BYTE     indisp;

GLOBAL WORD     fpt, fph, fpcnt;                /* forkq tail, head,    */
                                                /*   count              */
GLOBAL SPB      wind_spb;
GLOBAL WORD     curpid;

GLOBAL THEGLO   D;

static BYTE     scrap_dir[LEN_ZPATH];
static BYTE     cur_dir[LEN_ZPATH];
static BYTE     cmd[LEN_ZPATH];


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
BYTE *scan_2(BYTE *pcurr, WORD *pwd)
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

/*
 * in the following code, there are many initialisations of global ad_xxx
 * variables to point to members of D (which is also global).  Most or all
 * of the ad_xxx variables & their initialisations could be removed with
 * appropriate changes to the modules that use them.
 */
static void ini_dlongs(void)
{
                                                /* init. long pointer   */
                                                /*   to global array    */
                                                /*   which is used by   */
                                                /*   resource calls     */
        ad_ssave = (LONG)&start;
        ad_sysglo = (LONG)&D.g_sysglo[0];
                                                /* gemoblib             */
        ad_valstr = (LONG)&D.g_valstr[0];
        ad_fmtstr = (LONG)&D.g_fmtstr[0];
        ad_rawstr = (LONG)&D.g_rawstr[0];
        ad_tmpstr = (LONG)&D.g_tmpstr[0];

        D.s_cmd = &cmd[0];
        ad_scmd = (LONG)D.s_cmd;
        D.g_scrap = &scrap_dir[0];
        ad_scrap = (LONG)D.g_scrap;
        D.s_cdir = &cur_dir[0];
        ad_scdir = (LONG)D.s_cdir;
        D.g_loc1 = &gl_1loc[0];
        D.g_loc2 = &gl_2loc[0];
        D.g_dir = &gl_dir[0];
        ad_path = (LONG)D.g_dir;
        D.g_dta = &gl_dta[0];
        ad_dta = (LONG)D.g_dta;
        ad_fsdta = (LONG)&gl_dta[30];
}


LONG size_theglo(void)
{
    return( sizeof(THEGLO)/2 );
}



/*
*       called from startup code to initialise the process 0 supervisor stack ptr:
*        1. determines the end of the supervisor stack
*        2. initialises the supervisor stack pointer in the UDA
*        3. returns the offset from the start of THEGLO to the end of the stack
*/
LONG init_p0_stkptr(void)
{
    UDA *u = &D.g_intuda[0];

    u->u_spsuper = &u->u_supstk + 1;

    return (char *)u->u_spsuper - (char *)u;
}



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

#if DBG_GEMINIT
        kprintf("iprocess(\"%s\")\n", (const char*)pname);
#endif
        /* figure out load addr */

        ldaddr = (ULONG) routine;

        /* create process to execute it */
        return( pstart(routine, pname, ldaddr) );
}


/*
*       Start up the file selector by initializing the fs_tree
*/
static void fs_start(void)
{
        OBJECT *tree;

        tree = rs_trees[FSELECTR];
        ad_fstree = (LONG)tree;
        ob_center((LONG)tree, &gl_rfs);
}


/*
*       Routine to load program file pointed at by pfilespec, then
*       create new process context for it.  This uses the load overlay
*       function of DOS.
*/

static void sndcli(BYTE *pfilespec)
{
        register WORD   handle;
        WORD            err_ret;
        LONG            ldaddr;

#if DBG_GEMINIT
        kprintf("sndcli(\"%s\")\n", (const char*)pfilespec);
#endif
        strcpy(&D.s_cmd[0], pfilespec);

        handle = dos_open( (BYTE *)ad_scmd, ROPEN );
        if (!DOS_ERR)
        {
          err_ret = pgmld(handle, &D.s_cmd[0], (LONG **)&ldaddr);
          dos_close(handle);
                                                /* create process to    */
                                                /*   execute it         */
          if (err_ret != -1)
            pstart(gotopgm, pfilespec, ldaddr);
        }
}



/*
*       Routine to load in desk accessories.  Files by the name of *.ACC
*       will be loaded.
*/
static void ldaccs(void)
{
        register WORD   i;
        WORD            ret;

        strcpy(&D.g_dir[0], rs_str(STACC));
        dos_sdta(ad_dta);

        /* if Control is held down then skip loading of accs */
        if ((kbshift(-1) & (1<<2)))
          return;

        ret = TRUE;
        for(i=0; (i<NUM_ACCS) && (ret); i++)
        {

          ret = (i==0) ? dos_sfirst(ad_path, F_RDONLY) : dos_snext();
          if (ret)
            sndcli(&gl_dta[30]);
        }
}



static void sh_addpath(void)
{
        char    *lp, *np, *new_envr;
        const char *pp;
        WORD    oelen, oplen, nplen, pplen, fstlen;
        BYTE    tmp;
        char    tmpstr[MAX_LEN];

        lp = (char *)ad_envrn;
                                                /* get to end of envrn  */
        while ( *lp || *(lp+1) )                /* ends with 2 nulls    */
          lp++;
        lp++;                                   /* past 2nd null        */
                                                /* old environment length*/
        oelen = (lp - (char *)ad_envrn) + 2;
                                                /* PATH= length & new path length */
        pp = rs_fstr[STPATH];
        strcpy(tmpstr, rs_fstr[STINPATH]);
        np = tmpstr;

        pplen = strlen(pp);
        nplen = strlen(np);

        if (oelen+nplen+pplen+1 > ENV_SIZE) {
#if DBG_GEMINIT
            kprintf("sh_addpath(): cannot add path, environment buffer too small\n");
#endif
            return;
        }
                                                /* fix up drive letters */
        lp = np;
        while ( (tmp = *lp) != 0 )
        {
          if (tmp == ':')
            *(lp-1) = gl_logdrv;
          lp++;
        }
                                                /* alloc new environ    */
        new_envr = envbuf;
                                                /* get ptr to initial   */
                                                /*   PATH=              */
        sh_envrn((LONG)&lp, (LONG)pp);

        if(lp)
        {
                                                /* first part length    */
          oplen = strlen(lp);                   /* length of actual path */

          fstlen = lp - (char *)ad_envrn + oplen; /* len thru end of path */
          memcpy(new_envr,(char *)ad_envrn,fstlen);
        }
        else
        {
          oplen = 0;
          strcpy(new_envr,pp);
          fstlen = pplen + 1;
        }

        if (oplen)
        {
          *(new_envr+fstlen) = ';';     /* to splice in new path */
          fstlen += 1;
        }

        memcpy(new_envr+fstlen,np,nplen);       /* splice on more path  */
                                                /* copy rest of environ */
        if(lp)
        {
          memcpy(new_envr+fstlen+nplen,lp+oplen,oelen-fstlen);
        }

        ad_envrn = (LONG)new_envr;              /* remember new environ.*/
}




void sh_deskf(WORD obj, LONG plong)
{
        register OBJECT *tree;

        tree = (OBJECT *)ad_stdesk;
        *(LONG *)plong = tree[obj].ob_spec;
}



static void sh_init(void)
{
        WORD    cnt, need_ext;
        BYTE    *psrc, *pdst, *pend;
        BYTE    *s_tail;
        SHELL   *psh;
        BYTE    savch;

        psh = &sh[0];

        sh_deskf(2, (LONG)&ad_pfile);
                                                /* add in internal      */
                                                /*   search paths with  */
                                                /*   right drive letter */
        
        sh_addpath();
                                                /* set defaults         */
        psh->sh_doexec = psh->sh_dodef = gl_shgem
                 = psh->sh_isgem = TRUE;
        psh->sh_fullstep = FALSE;

                                                /* parse command tail   */
                                                /*   that was stored in */
                                                /*   geminit            */
        psrc = s_tail = &D.g_dir[0];            /* reuse part of globals*/
        memcpy(s_tail,(char *)ad_stail,128);
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
        LBCOPY(ad_stail, (LONG)(&s_tail[0]), 128);
}



/*
*       Routine to read in the start of the emudesk.inf file,
*       expected to contain the #E and #Z lines.
*/
static void sh_rdinf(void)
{
        WORD    fh, size;
        char    *pfile;
        char    tmpstr[MAX_LEN];

        infbuf[0] = 0;

        strcpy(tmpstr, rs_fstr[STINFPAT]);
        pfile = tmpstr;
        *pfile = 'A' + dos_gdrv();              /* set the drive        */

        fh = dos_open(pfile, ROPEN);
        if ( !fh || DOS_ERR)
          return;
                                                /* NOTA BENE all required info */
                                                /*  MUST be within INF_SIZE    */
                                                /*  bytes from beg of file     */
        size = dos_read(fh, INF_SIZE, (LONG)infbuf);
        dos_close(fh);
        if (DOS_ERR)
          return;
        infbuf[size] = 0;
}



/*
*       Part 1 of early emudesk.inf processing
*
*       This has one function: determine if we need to change resolutions
*       (from #E).  If so, we set gl_changerez and gl_nextrez appropriately.
*/
static void process_inf1(void)
{
        WORD    env1, env2;
#if CONF_WITH_SHIFTER
        WORD    mode;
#endif
        char    *pcurr;

        gl_changerez = 0;       /* assume no change */

        for (pcurr = infbuf; *pcurr; )
        {
          if ( *pcurr++ != '#' )
            continue;
          if (*pcurr++ == 'E')          /* #E 3A 11 FF 02               */
          {                             /* desktop environment          */
            pcurr = scan_2(pcurr, &env1);
            ev_dclick(env1 & 0x07, TRUE);
            pcurr = scan_2(pcurr, &env2);
            if (*pcurr == '\r')         /* no video info saved */
              break;

#if CONF_WITH_SHIFTER
            pcurr = scan_2(pcurr, &env1);
            pcurr = scan_2(pcurr, &env2);
            mode = (env1 << 8) | (env2 & 0x00ff);
            mode = check_moderez(mode);
            if (mode == 0)              /* no change required */
              break;
            if (mode > 0)               /* need to set Falcon mode */
            {
              gl_changerez = 2;
              gl_nextrez = mode;
            }
            else                        /* set ST/TT rez */
            {
              gl_changerez = 1;
              gl_nextrez = (mode & 0x00ff) + 2;
            }
#endif /* CONF_WITH_SHIFTER */
          }
        }
}



/*
*       Part 2 of early emudesk.inf processing
*
*       This has two functions:
*        1. Determine the auto-run program to be started (from #Z).
*        2. Set the double-click speed (from #E).  This is done here
*           in case we have an auto-run program.
*/
static void process_inf2(void)
{
        WORD    env;
        char    *pcurr;
        BYTE    tmp;

        pcurr = infbuf;
        while (*pcurr)
        {
          if ( *pcurr++ != '#' )
            continue;
          tmp = *pcurr;
          if (tmp == 'E')               /* #E 3A 11                     */
          {                             /* desktop environment          */
            pcurr += 2;
            scan_2(pcurr, &env);
            ev_dclick(env & 0x07, TRUE);
          }
          else if (tmp == 'Z')      /* something like "#Z 01 C:\THING.APP@" */
          {
            BYTE *tmpptr1, *tmpptr2;
            pcurr += 5;
            tmpptr1 = pcurr;
            while (*pcurr && (*pcurr != '@'))
              ++pcurr;
            *pcurr = 0;
            tmpptr2 = sh_name(tmpptr1);
            *(tmpptr2-1) = 0;
#if DBG_GEMINIT
            kprintf("Found #Z entry in EMUDESK.INF with path=%s and prg=%s\n",
                    tmpptr1, tmpptr2);
#endif
            sh_wdef((LONG)tmpptr2, (LONG)tmpptr1);
            ++pcurr;
          }
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
    const BITBLK *tmpadbi;

    sh_rdinf();                 /* get start of emudesk.inf */
    if (!gl_changerez)          /* can't be here because of rez change,       */
        process_inf1();         /*  so see if .inf says we need to change rez */

    if (gl_changerez) {
        switch(gl_changerez) {
#if CONF_WITH_SHIFTER
        case 1:                     /* ST(e) or TT display */
            Setscreen(-1L,-1L,gl_nextrez-2,0);
            initialise_palette_registers(gl_nextrez-2,0);
            break;
#endif
#if CONF_WITH_VIDEL
        case 2:                     /* Falcon display */
            Setscreen(-1L, -1L, FALCON_REZ, gl_nextrez);
            initialise_palette_registers(FALCON_REZ,gl_nextrez);
            break;
#endif
        }
        gsx_wsclear();              /* avoid artefacts that may show briefly */
    }

    totpds = NUM_PDS;
    ml_ocnt = 0;

    gl_changerez = FALSE;

    ini_dlongs();               /* init longs */
    cli();
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
        rlr->p_qaddr = (LONG)(&rlr->p_queue[0]);
        rlr->p_qindex = 0;
        memset(rlr->p_name, ' ', 8);
        rlr->p_appdir[0] = '\0'; /* by default, no application directory */
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
    sti();

    /*
     * screen manager process init. this process starts out owning the mouse
     * and the keyboard. it has a pid == 1
     */
    gl_dacnt = 0;
    gl_mowner = ctl_pd = iprocess("SCRENMGR", ctlmgr);

    /* load gem resource and fix it up before we go */
    gem_rsc_init();
    {
        /* get mice forms       */
        ad_armice = *(LONG *)&rs_bitblk[MICE00];
        ad_hgmice = *(LONG *)&rs_bitblk[MICE02];

        /* init button stuff    */
        gl_btrue = 0x0;
        gl_bdesired = 0x0;
        gl_bdely = 0x0;
        gl_bclick = 0x0;

        gl_logdrv = dos_gdrv() + 'A';   /* boot directory       */
        gsx_init();                     /* do gsx open work station */

        /* load all desk acc's  */
        if (totpds > 2)
            ldaccs();

        /* fix up icons         */
        for(i=0; i<3; i++) {
            tmpadbi = &rs_bitblk[NOTEBB+i];
            memcpy((char *)&bi, tmpadbi, sizeof(BITBLK));
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

        /* set init. click rate: must do this after setting gl_ticktime */
        ev_dclick(3, TRUE);

        /* fix up the GEM rsc. file now that we have an open WS */
        gem_rsc_fixit();

        /* get st_desk ptr */
        ad_stdesk = (LONG) rs_trees[DESKTOP];

        /* init. window vars. */
        wm_start();

        /* startup gem libs */
        fs_start();

        /* remember current desktop directory */
        sh_curdir(ad_scdir);

        /* process emudesk.inf part 2 */
        process_inf2();

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

        /* give back the tick   */
        cli();
        gl_ticktime = gsx_tick(tiksav, &tiksav);
        sti();

        /* close workstation    */
        gsx_wsclose();
    }

    /* return GEM's 0xEF int*/
    cli();
    givecpm();
    sti();
}


