/*      GEMLIB.H        03/15/84 - 06/07/85     Lee Lorenzen            */
/*      EVNTLIB.H       03/15/84 - 05/16/84     Lee Lorenzen            */
/*      2.0             10/30/85 - 10/30/85     Lowell Webster          */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2019 The EmuTOS development team
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

#include "bdosdefs.h"
                                /* internal-use mu_flags */
#define MU_SDMSG    0x0040
#define MU_MUTEX    0x0080

typedef struct moblk
{
    BOOL m_out;
    GRECT m_gr;
} MOBLK;


/*      WINDLIB.H       05/05/84 - 01/26/85     Lee Lorenzen            */

/* indexes of elements in AES window */
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

/* values used in w_flags field of window structure */
#define VF_INUSE    0x0001      /* the window has been created */
#define VF_BROKEN   0x0002      /* the window is overlapped, can't be blitted */
#define VF_INTREE   0x0004      /* the window is currently open */

/* the AES window structure */
typedef struct window
{
    WORD  w_flags;
    AESPD *w_owner;
    WORD  w_kind;
    char  *w_pname;
    char  *w_pinfo;
    GRECT w_full;
    GRECT w_work;
    GRECT w_prev;
    WORD  w_hslide;
    WORD  w_vslide;
    WORD  w_hslsiz;
    WORD  w_vslsiz;
#if CONF_WITH_WINDOW_COLOURS
    WORD  w_tcolor[NUM_ELEM];   /* TEDINFO colour words for topped window */
    WORD  w_bcolor[NUM_ELEM];   /* TEDINFO colour words for untopped window */
#endif
    ORECT *w_rlist;             /* owner rectangle list */
    ORECT *w_rnext;             /* used for search first, search next */
} WINDOW;

#define NUM_ORECT (NUM_WIN * 10)        /* is this enough???    */

#define WS_FULL 0
#define WS_CURR 1
#define WS_PREV 2
#define WS_WORK 3
#define WS_TRUE 4


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

    char  g_scrap[LEN_ZPATH];   /* current scrap directory */
    char  s_cdir[LEN_ZPATH];    /* current desktop directory */
    char  s_cmd[MAXPATHLEN];    /* fully-qualified program name */
    char  g_work[WORKAREASIZE]; /* general work area */
    DTA   g_dta;                /* AES's DTA */

    FPD   g_fpdx[NFORKS];       /* the fork ring, used by gemdisp.c */
    ORECT g_olist[NUM_ORECT];

    char  g_shelbuf[SIZE_SHELBUF];  /* AES shell buffer */

    char  g_rawstr[MAX_LEN];
    char  g_tmpstr[MAX_LEN];
    char  g_valstr[MAX_LEN];
    char  g_fmtstr[MAX_LEN];

    WINDOW w_win[NUM_WIN];

    WORD  g_accreg;             /* number of entries used in g_acctitle[] */
    char  *g_acctitle[NUM_ACCS];/* used by menu_register(). must always   */
                                /*  be NUM_ACCS since one DA can issue    */
                                /*   more than one menu_register()!       */

    AESPROCESS *g_acc;          /* for up to NUM_ACCS desk accessories */
} THEGLO;

#endif /* GEMLIB_H */
