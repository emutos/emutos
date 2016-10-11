/*      DESKAPP.H       06/11/84 - 06/11/85             Lee Lorenzen    */
/*      for 3.0         1/21/86  - 12/30/86             MDF             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#ifndef _DESKAPP_H
#define _DESKAPP_H
#include "deskconf.h"
#include "icons.h"

/*
 * bit masks for the a_flags field in the ANODE
 */
                            /* flags applicable to installed applications */
#define AF_ISCRYS 0x0001        /* is crystal (i.e. GEM) application */
/* #define AF_ISGRAF 0x0002        no longer used */
#define AF_ISDESK 0x0004        /* requires desktop icon */
#define AF_ISPARM 0x0008        /* requires input parameters (TTP or GTP) */
#define AF_APPDIR 0x0010        /* use application dir (else top window) */
#define AF_ISFULL 0x0020        /* use full path (else filename only) */
#define AF_AUTORUN 0x0080       /* this program is to be autorun */
                            /* flags applicable to window icons */
#define AF_ISEXEC 0x4000        /* this is an executable file */
#define AF_WINDOW 0x8000        /* this is an anode for a window icon */

/*
 * values for the a_type field in the ANODE
 */
#define AT_ISFILE 0
#define AT_ISFOLD 1
#define AT_ISDISK 2
#define AT_ISTRSH 3

/*
 * standard gem icons, always available
 */
#define IG_HARD   0             /* hard drive */
#define IG_FLOPPY 1             /* floppy drive */
#define IG_FOLDER 2             /* folder */
#define IG_TRASH  3             /* trash */
#define IG_4RESV  4
#define IG_5RESV  5
#define IG_APPL   6             /* generic application icon */
#define IG_DOCU   7             /* generic document icon */
#define NUM_GEM_IBLKS (IG_DOCU+1)

/*
 * the following values are saved in EMUDESK.INF by EmuTOS 0.9.6 &
 * earlier.  app_parse() converts them to the standard values.
 */
#define IG_APPL_REV0    8       /* application icon */
#define IG_DOCU_REV0    40      /* document icon */


/*
 * configuration parameters
 */
#define NUM_ANODES  64          /* # of application nodes */
#define SIZE_AFILE  2048        /* size of buffer for EMUDESK.INF file */
#define SIZE_BUFF   4096        /* size of buffer used to store ANODE text */

#if CONF_WITH_WINDOW_ICONS
#define NUM_IBLKS (NUM_GEM_IBLKS+ICON_END)
#else
#define NUM_IBLKS (NUM_GEM_IBLKS)
#endif


typedef struct _applstr ANODE;
struct _applstr
{
    ANODE *a_next;
    UWORD a_flags;              /* see above for usage */
    BYTE a_funkey;              /* associated function key: 0=none, else 0x01-0x14 */
    BYTE a_letter;              /* letter for icon */
    WORD a_type;                /* icon type (see above) */
    WORD a_obid;                /* object index */
    BYTE *a_pappl;              /* filename.ext of appplication */
    BYTE *a_pdata;              /* mask for data files */
    BYTE *a_pargs;              /* arguments */
    WORD a_aicon;               /* application icon # */
    WORD a_dicon;               /* data icon # */
    WORD a_xspot;               /* desired spot on desk */
    WORD a_yspot;               /* desired spot on desk */
};


/*
 * save current info for one window
 */
typedef struct
{
    WORD x_save;                /* window position */
    WORD y_save;
    WORD w_save;
    WORD h_save;
    WORD vsl_save;              /* vertical slider position */
    WORD obid_save;             /* currently-selected object */
    BYTE pth_save[LEN_ZPATH];
} WSAVE;


/* values stored in cs_sort (below) range from 0 to CS_NOSORT */
#define CS_NOSORT   (NSRTITEM-NAMEITEM)

/*
 * save desktop context (preferences and windows)
 */
typedef struct
{
    BYTE cs_sort;               /* Sort mode */
    BYTE cs_view;               /* Show files as icons or text */
    BYTE cs_confcpy;            /* Confirm copies */
    BYTE cs_confdel;            /* Confirm deletes */
    BYTE cs_confovwr;           /* Confirm overwrite */
    BYTE cs_dblclick;           /* Double click speed */
    BYTE cs_mnuclick;           /* Drop down menu click mode */
    BYTE cs_timefmt;            /* Time format */
    BYTE cs_datefmt;            /* Date format */
    WSAVE cs_wnode[NUM_WNODES]; /* window save info */
} CSAVE;


extern WORD gl_stdrv;


/*
 * Function prototypes
 */
ANODE *app_alloc(WORD tohead);
void app_free(ANODE *pa);
BYTE *scan_str(BYTE *pcurr, BYTE **ppstr);
void app_tran(WORD bi_num);
void app_start(void);
void app_save(WORD todisk);
void app_blddesk(void);
ANODE *app_afind_by_id(WORD obid);
ANODE *app_afind_by_name(WORD atype, BYTE *pspec, BYTE *pname, WORD *pisapp);

#endif  /* _DESKAPP_H */
