/*      STRUCT.H        1/28/84 - 01/18/85      Lee Jay Lorenzen        */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
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

#ifndef GEMSTRUCT_H
#define GEMSTRUCT_H
#include "aesdefs.h"

typedef struct aespd   AESPD;           /* process descriptor           */
typedef struct uda     UDA;             /* user stack data area         */
typedef struct cdastr  CDA;             /* console data area structure  */
typedef struct qpb     QPB;             /* queue parameter block        */
typedef struct evb     EVB;             /* event block                  */
typedef struct cqueue  CQUEUE;          /* console kbd queue            */
typedef struct spb     SPB;             /* sync parameter block         */
typedef struct fpd     FPD;             /* fork process descriptor      */

typedef UWORD   EVSPEC;

#define NUM_PDS (NUM_ACCS + 2)          /* acc's + ctrlpd + dos appl.   */

/*
 * EVBs are used to track events that an AES process is waiting on.  the
 * maximum number of events that a process can wait for is 6 (MU_KEYBD,
 * MU_BUTTON, MU_M1, MU_M2, MU_MESAG, MU_TIMER), when ev_multi() is used.
 *
 * therefore we create 6 EVBs per AES process and ensure that we cannot
 * run out of EVBs.
 */
#define EVBS_PER_PD     6               /* EVBs per AES process */

#define KBD_SIZE 8
#define QUEUE_SIZE 128
#define NFORKS 32

struct cqueue               /* console keyboard queue */
{
        WORD    c_buff[KBD_SIZE];
        WORD    c_front;
        WORD    c_rear;
        WORD    c_cnt;
};


#define C_KOWNER 0x0001
#define C_MOWNER 0x0002

struct cdastr               /* console data area */
{
        UWORD   c_flags;
        EVB     *c_iiowait;     /* waiting for input            */
        EVB     *c_msleep;      /* wait for mouse rect          */
        EVB     *c_bsleep;      /* wait for button              */
        CQUEUE  c_q;            /* input queue                  */
};


struct uda                  /* user stack data area */
{
        ULONG   u_regs[15];             /*   0  d0-d7, a0-a6             */
        ULONG   *u_spsuper;             /*  3C  supervisor stack         */
        ULONG   *u_spuser;              /*  40  user stack               */
        ULONG   *u_oldspsuper;          /*  44  old ssp, used in trapaes [gemdosif.S] */
        ULONG   u_super[AES_STACK_SIZE];/*  48  */
        ULONG   u_supstk;
} ;


#define NOCANCEL 0x0001         /* event is occurring */
#define COMPLETE 0x0002         /* event completed */
#define EVDELAY  0x0004         /* event is delay event */
#define EVMOUT   0x0008         /* event flag for mouse wait outside of rect*/

struct evb                  /* event block */
{
        EVB     *e_nextp;       /* link to next event on AESPD event list */
        EVB     *e_link;        /* link to next block on event chain */
        EVB     *e_pred;        /* link to prev block on event chain */
        AESPD   *e_pd;          /* owner AESPD (data for fork) */
        LONG    e_parm;         /* parm for request -> event comm */
        WORD    e_flag;
        EVSPEC  e_mask;         /* mask for event notification */
        LONG    e_return;
} ;

/* pd defines */
/* p_name */
#define AP_NAMELEN  8           /* architectural */
/* p_stat */
#define WAITIN      0x0001      /* process is waiting for an event */
/* p_flags */
#define AP_OPEN     0x0001      /* application is between appl_init() & appl_exit() */
#define AP_MESAG    0x0002      /* application has waited for a message */
#define AP_ACCLOSE  0x0004      /* application has seen an AC_CLOSE message */

struct aespd                /* process descriptor */
{
        AESPD   *p_link;        /*  0 */
        AESPD   *p_thread;      /*  4 */
        UDA     *p_uda;         /*  8  UDA - assembler code expects this offset */

        char    p_name[AP_NAMELEN]; /*  C */

        CDA     *p_cda;         /* 14  cio data area        */
        LONG    p_ldaddr;       /* 18  load address - assembler code expects this offset */
        WORD    p_pid;          /* 1C */
        WORD    p_stat;         /* 1E */

        EVSPEC  p_unused;       /* 20  was p_evbits (remnant of old code) */
        EVSPEC  p_evwait;       /* 22  event wait mask      */
        EVSPEC  p_evflg;        /* 24  event flags          */

        UWORD   p_flags;        /* 26  process status flags, see above */
        EVB     *p_evlist;      /* 28 */
        EVB     *p_qdq;         /* 2C */
        EVB     *p_qnq;         /* 30 */

        struct {            /* used by ctlmgr() to pass WM_ARROWED msg to event handlers */
            WORD action;        /* action to perform (WA_UPLINE etc) [-ve means no msg] */
            WORD wh;            /* window handle of applicable window */
        }       p_msg;

#if CONF_WITH_GRAF_MOUSE_EXTENSION
        MFORM   p_mouse;        /* used by graf_mouse(SAVE,RESTORE) */
#endif

        char    *p_qaddr;       /* */
        WORD    p_qindex;       /* */
        char    p_queue[QUEUE_SIZE];    /* */
        char    p_appdir[LEN_ZPATH+2];  /* directory containing the executable */
                                        /* (includes trailing path separator)  */
};


struct qpb                  /* queue parameter block */
{
        AESPD   *qpb_ppd;
        WORD    qpb_cnt;
        LONG    qpb_buf;
} ;

struct spb                  /* sync parameter block */
{
        WORD    sy_tas;
        AESPD   *sy_owner;
        EVB     *sy_wait;
} ;

typedef void (*FCODE)(LONG fdata);      /* pointer to function used by forkq() */

struct fpd                  /* fork process descriptor */
{
        FCODE   f_code;
        LONG    f_data;
} ;
#endif /* GEMSTRUCT_H */
