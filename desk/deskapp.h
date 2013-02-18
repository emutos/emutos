/*      DESKAPP.H       06/11/84 - 06/11/85             Lee Lorenzen    */
/*      for 3.0         1/21/86  - 12/30/86             MDF             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2013 The EmuTOS development team
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

#ifndef DESKAPP_H
#define DESKAPP_H
#include "deskconf.h"

#define AP_APPLOPEN 0
#define AP_DATAOPEN 1
#define AP_DATADRAG 2

#define AF_ISCRYS 0x0001                        /* is crystal appl.     */
#define AF_ISGRAF 0x0002                        /* is graphic appl.     */
#define AF_ISDESK 0x0004                        /* is placed on desk    */
#define AF_ISPARM 0x0008                        /* is in need of input  */
                                                /*   parameters         */

#define AT_ISFILE 0
#define AT_ISFOLD 1
#define AT_ISDISK 2
#define AT_ISTRSH 3

                                                /* gem icons            */
#define IG_HARD 0
#define IG_FLOPPY 1
#define IG_FOLDER 2
#define IG_TRASH 3
#define IG_4RESV 4
#define IG_5RESV 5
#define IA_GENERIC_ALT 6
#define ID_GENERIC_ALT 7
#define NUM_GEM_IBLKS (ID_GENERIC_ALT+1)
                                                /* application icons    */
#define IA_GENERIC 8
#define IA_SS 9
#define IA_WP 10
#define IA_DB 11
#define IA_DRAW 12
#define IA_PAINT 13
#define IA_PROJECT 14
#define IA_GRAPH 15
#define IA_OUTLINE 16
#define IA_ACCNT 17
#define IA_MULTI 18
#define IA_EDUC 19
#define IA_COMM 20
#define IA_TOOL 21
#define IA_GAME 22
#define IA_OUTPUT 23
                                                /* document icons       */
#define ID_GENERIC 40
#define ID_SS 41
#define ID_WP 42
#define ID_DB 43
#define ID_DRAW 44
#define ID_PAINT 45
#define ID_PROJECT 46
#define ID_GRAPH 47
#define ID_OUTLINE 48
#define ID_ACCNT 49
#define ID_MULTI 50
#define ID_EDUC 51
#define ID_COMM 52
#define ID_TOOL 53
#define ID_GAME 54
#define ID_OUTPUT 55

#define NUM_ANODES 64                           /* # of appl. nodes     */
#define SIZE_AFILE 2048                         /* size of DESKTOP.APP  */
                                                /*   file               */
#define SIZE_BUFF 4096                          /* size of a string     */
                                                /*   buffer             */

#if CONF_WITH_DESKTOP_ICONS
#define NUM_IBLKS 72
#else
#define NUM_IBLKS 0
#endif

#if (NUM_IBLKS > NUM_GEM_IBLKS)
#define HAVE_APPL_IBLKS 1
#else
#define HAVE_APPL_IBLKS 0
#endif

#define ANODE struct applstr
ANODE
{
        ANODE           *a_next;
        WORD            a_flags;
        WORD            a_type;                 /* icon/appl type       */
        WORD            a_obid;                 /* object index         */
        BYTE            *a_pappl;               /* filename.ext of ap.  */
        BYTE            *a_pdata;               /* wldcards of data file*/
        WORD            a_aicon;                /* application icon #   */
        WORD            a_dicon;                /* data icon #          */
        WORD            a_letter;               /* letter for icon      */
        WORD            a_xspot;                /* desired spot on desk */
        WORD            a_yspot;                /* desired spot on desk */
};


#define WSAVE   struct window_save
WSAVE
{
        WORD    x_save;
        WORD    y_save;
        WORD    w_save;
        WORD    h_save;
        WORD    hsl_save;
        WORD    vsl_save;
        WORD    obid_save;
        BYTE    pth_save[LEN_ZPATH];
};


#define CSAVE   struct context_save
CSAVE
{
        WORD    sitem_save;     /* Sort mode */
        WORD    vitem_save;     /* Show files as icons or text */
        WORD    ccopy_save;     /* Confirm copies */
        WORD    cdele_save;     /* Confirm deletes */
        WORD    covwr_save;     /* Confirm overwrite */
        WORD    cdclk_save;     /* Double click speed */
        WORD    cmclk_save;     /* Drop down menu click mode */
        WORD    ctmfm_save;     /* Time format */
        WORD    cdtfm_save;     /* Date format */
        WSAVE   win_save[NUM_WNODES];
};


extern WORD     gl_numics;
extern WORD     gl_stdrv;


/* Prototypes: */
ANODE *app_alloc(WORD tohead);
void app_free(ANODE *pa);
BYTE *scan_str(BYTE *pcurr, BYTE **ppstr);
void app_tran(WORD bi_num);
void app_start(void);
void app_save(WORD todisk);
BYTE app_blddesk(void);
ANODE *app_afind(WORD isdesk, WORD atype, WORD obid, BYTE *pname, WORD *pisapp);


#endif  /* DESKAPP_H */
