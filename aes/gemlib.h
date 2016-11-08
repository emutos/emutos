/*      GEMLIB.H        03/15/84 - 06/07/85     Lee Lorenzen            */
/*      EVNTLIB.H       03/15/84 - 05/16/84     Lee Lorenzen            */
/*      2.0             10/30/85 - 10/30/85     Lowell Webster          */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#ifndef GEMLIB_H
#define GEMLIB_H

#include "dta.h"
                                /* mu_flags             */
#define MU_KEYBD    0x0001
#define MU_BUTTON   0x0002
#define MU_M1       0x0004
#define MU_M2       0x0008
#define MU_MESAG    0x0010
#define MU_TIMER    0x0020
#define MU_SDMSG    0x0040
#define MU_MUTEX    0x0080

typedef struct moblk
{
    WORD m_out;
    WORD m_x;
    WORD m_y;
    WORD m_w;
    WORD m_h;
} MOBLK;


/*      APPLLIB.H       05/05/84 - 10/16/84     Lee Lorenzen            */

#define SCR_MGR     0x0001      /* pid of the screen manager */

#define AP_MSG      0

#define MN_SELECTED 10

#define WM_REDRAW   20
#define WM_TOPPED   21
#define WM_CLOSED   22
#define WM_FULLED   23
#define WM_ARROWED  24
#define WM_HSLID    25
#define WM_VSLID    26
#define WM_SIZED    27
#define WM_MOVED    28
#define WM_NEWTOP   29          /* not used as of 4/16/86       */
#define WM_UNTOPPED 30          /* added 10/28/85 LKW           */

#define AC_OPEN     40
#define AC_CLOSE    41
#define AC_ABORT    42          /* added 3/27/86 LKW            */

#define CT_UPDATE   50          /* not used as of 4/16/86       */


/*      FORMLIB.H       05/05/84 - 10/16/84     Gregg Morris            */

#define FMD_START   0
#define FMD_GROW    1
#define FMD_SHRINK  2
#define FMD_FINISH  3


/*      WINDLIB.H       05/05/84 - 01/26/85     Lee Lorenzen            */

#define VF_INUSE    0x0001
#define VF_BROKEN   0x0002
#define VF_INTREE   0x0004
#define VF_SUBWIN   0x0008
#define VF_KEEPWIN  0x0010

typedef struct window
{
    WORD  w_flags;
    AESPD *w_owner;
    WORD  w_kind;
    BYTE  *w_pname;
    BYTE  *w_pinfo;
    WORD  w_xfull;
    WORD  w_yfull;
    WORD  w_wfull;
    WORD  w_hfull;
    WORD  w_xwork;
    WORD  w_ywork;
    WORD  w_wwork;
    WORD  w_hwork;
    WORD  w_xprev;
    WORD  w_yprev;
    WORD  w_wprev;
    WORD  w_hprev;
    WORD  w_hslide;
    WORD  w_vslide;
    WORD  w_hslsiz;
    WORD  w_vslsiz;
    ORECT *w_rlist;             /* owner rectangle list */
    ORECT *w_rnext;             /* used for search first, search next */
} WINDOW;

#define NUM_ORECT (NUM_WIN * 10)        /* is this enough???    */

#define WS_FULL 0
#define WS_CURR 1
#define WS_PREV 2
#define WS_WORK 3
#define WS_TRUE 4

#define NAME    0x0001
#define CLOSER  0x0002
#define FULLER  0x0004
#define MOVER   0x0008
#define INFO    0x0010
#define SIZER   0x0020
#define UPARROW 0x0040
#define DNARROW 0x0080
#define VSLIDE  0x0100
#define LFARROW 0x0200
#define RTARROW 0x0400
#define HSLIDE  0x0800
#define HOTCLOSE 0x1000         /* added 11/12/85       LKW             */

#define W_BOX       0
#define W_TITLE     1
#define W_CLOSER    2
#define W_NAME      3
#define W_FULLER    4
#define W_INFO      5
#define W_DATA      6
#define W_WORK      7
#define W_SIZER     8
#define W_VBAR      9
#define W_UPARROW   10
#define W_DNARROW   11
#define W_VSLIDE    12
#define W_VELEV     13
#define W_HBAR      14
#define W_LFARROW   15
#define W_RTARROW   16
#define W_HSLIDE    17
#define W_HELEV     18

#define NUM_ELEM    19

                                /* arrow message        */
#define WA_UPPAGE 0
#define WA_DNPAGE 1
#define WA_UPLINE 2
#define WA_DNLINE 3
#define WA_LFPAGE 4
#define WA_RTPAGE 5
#define WA_LFLINE 6
#define WA_RTLINE 7


typedef struct sh_struct
{
    WORD sh_doexec;             /* TRUE during normal processing; set */
                                /*  to FALSE to shutdown the system   */
    WORD sh_dodef;              /* if TRUE then run the default startup   */
                                /*  app: normally EMUDESK, but can be an  */
                                /*  autorun program if so configured.     */
                                /*  if FALSE, run a normal application.   */
    WORD sh_isdef;              /* if TRUE then using the default startup   */
                                /*  app: normally EMUDESK, but can be an    */
                                /*  autorun program if so configured.       */
                                /*  if FALSE, running a normal application. */
    WORD sh_isgem;              /* TRUE if the application to be run is a GEM */
                                /*  application; FALSE if character-mode      */
    BYTE sh_desk[LEN_ZFNAME];   /* the name of the default startup app */
    BYTE sh_cdir[LEN_ZPATH];    /* the current directory for the default startup app */
} SHELL;


#define CMDTAILSIZE 128         /* architectural */
#if MAXPATHLEN > CMDTAILSIZE
  #define WORKAREASIZE  MAXPATHLEN
#else
  #define WORKAREASIZE  CMDTAILSIZE
#endif


/*
 * (most of) the set of structures required to manage one AES process
 */
typedef struct {
    UDA   a_uda;
    AESPD a_pd;
    CDA   a_cda;
    EVB   a_evb[EVBS_PER_PD];
} AESPROCESS;


typedef struct
{
    AESPROCESS g_int[2];        /* for AES internal processes, must be 1st */

    BYTE  g_scrap[LEN_ZPATH];   /* current scrap directory */
    BYTE  s_cdir[LEN_ZPATH];    /* current desktop directory */
    BYTE  s_cmd[MAXPATHLEN];    /* fully-qualified program name */
    BYTE  g_work[WORKAREASIZE]; /* general work area */
    DTA   g_dta;                /* AES's DTA */

    FPD   g_fpdx[NFORKS];       /* the fork ring, used by gemdisp.c */
    ORECT g_olist[NUM_ORECT];

    BYTE  g_rawstr[MAX_LEN];
    BYTE  g_tmpstr[MAX_LEN];
    BYTE  g_valstr[MAX_LEN];
    BYTE  g_fmtstr[MAX_LEN];

    WINDOW w_win[NUM_WIN];

    WORD  g_accreg;             /* number of entries used in g_acctitle[] */
    BYTE  *g_acctitle[NUM_ACCS];/* used by menu_register(). must always   */
                                /*  be NUM_ACCS since one DA can issue    */
                                /*   more than one menu_register()!       */

    AESPROCESS *g_acc;          /* for up to NUM_ACCS desk accessories */
} THEGLO;

#endif /* GEMLIB_H */
