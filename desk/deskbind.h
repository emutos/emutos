/*      DESKBIND.H              12/13/84-06/09/85       Lee Lorenzen    */
/*      for 3.0                 4/9/86  - 5/5/86        MDF             */
/*      for 2.3                 9/25/87                 mdf             */
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

#ifndef _DESKBIND_H
#define _DESKBIND_H

#include "deskconf.h"
#include "desk_rsc.h"           /* for RS_NTREE */
#include "dta.h"

#define ARROW   0
#define HGLASS  2
#define FLATHAND 4


/*
 * the following defines determine the minimum spacing between icons,
 * both in a desktop window and on the desktop.  because the desktop
 * (between resolution changes) is a fixed size, the actual spacing
 * between desktop icons is adjusted for symmetrical spacing (both
 * horizontal & vertical).
 */
#define SMALL_HSPC      4      /* horizontal, for ST low/medium, Falcon double-line */
#define LARGE_HSPC      8      /* horizontal, for ST high, Falcon not-double-line */
#define MIN_WINT        ((gl_height<=300)?SMALL_HSPC:LARGE_HSPC)
#define MIN_HINT        2      /* vertical */


/*
 * the following defines apply to the date & time format fields
 * in GLOBES (g_cdateform, g_ctimeform) and in CSAVE (cs_datefmt,
 * cs_timefmt)
 */
#define DATEFORM_SEP    '/'     /* separator used if not using _IDT */
#define DATEFORM_DMY    0       /* day-month-year */
#define DATEFORM_MDY    1       /* month-day-year */
#define DATEFORM_IDT    2       /* use format (incl separator) from _IDT cookie */
#define TIMEFORM_24H    0       /* 24 hour clock */
#define TIMEFORM_12H    1       /* 12 hour clock */
#define TIMEFORM_IDT    2       /* use format from _IDT cookie */


/*
 * An object in the g_screen[] array can be one of two types; each type
 * needs additional data, as follows:
 *  G_ICON      Used for icons on the desktop or in a desktop window.
 *              Needs an ICONBLK plus an index into g_iblist[].
 *  G_USERDEF   Used for text display in a desktop window.
 *              Needs a USERBLK.
 *
 * The following typedefs allow us to conveniently use a single array
 * for the additional data, thus saving some RAM space over the previous
 * approach.
 */
typedef struct
{
    WORD index;         /* index into G.g_iblist[] (transformed icon data/mask */
    ICONBLK block;      /* the ICONBLK for this object */
} ICONINFO;

typedef union
{
    ICONINFO icon;      /* relevant when the corresponding object is a G_ICON */
    USERBLK udef;       /* relevant when the corresponding object is a G_USERDEF */
} SCREENINFO;


/*
 * The desktop global data area
 */
typedef struct
{
/*GLOBAL*/ PNODE        g_plist[NUM_PNODES];
/*GLOBAL*/ PNODE        *g_pavail;
/*GLOBAL*/ PNODE        *g_phead;

/*GLOBAL*/ DTA          g_wdta;

/*GLOBAL*/ WNODE        g_wdesktop;             /* the desktop pseudo-window */
/*GLOBAL*/ WNODE        *g_wfirst;
/*GLOBAL*/ WNODE        g_wlist[NUM_WNODES];
/*GLOBAL*/ WORD         g_wcnt;

                                            /* view-related parms:      */
/*GLOBAL*/ WORD         g_num;                  /* number of points     */
/*GLOBAL*/ WORD         *g_pxy;                 /* outline pts to drag  */
/*GLOBAL*/ WORD         g_iview;                /* current view type    */
/*GLOBAL*/ WORD         g_iwext;                /* w,h of extent of a   */
/*GLOBAL*/ WORD         g_ihext;                /*   single item        */
/*GLOBAL*/ WORD         g_iwint;                /* w,h of interval      */
/*GLOBAL*/ WORD         g_ihint;                /*   between items      */
/*GLOBAL*/ WORD         g_iwspc;                /* w,h of space used by */
/*GLOBAL*/ WORD         g_ihspc;                /*   a single item      */
/*GLOBAL*/ WORD         g_isort;                /* current sort type    */

                                                /* stack of fcb's to use*/
                                                /*   for non-recursive  */
                                                /*   directory tree     */
                                                /*   traversal          */
/*GLOBAL*/ DTA          g_dtastk[MAX_LEVEL];
/*GLOBAL*/ LONG         g_nfiles;
/*GLOBAL*/ LONG         g_ndirs;
/*GLOBAL*/ LONG         g_size;

/*GLOBAL*/ WORD         g_xyobpts[MAX_OBS * 2];

/*GLOBAL*/ WORD         g_rmsg[8];

/*GLOBAL*/ WORD         g_xdesk;
/*GLOBAL*/ WORD         g_ydesk;
/*GLOBAL*/ WORD         g_wdesk;
/*GLOBAL*/ WORD         g_hdesk;

/*GLOBAL*/ WORD         g_xfull;
/*GLOBAL*/ WORD         g_yfull;
/*GLOBAL*/ WORD         g_wfull;
/*GLOBAL*/ WORD         g_hfull;

/*GLOBAL*/ BYTE         g_cmd[128];
/*GLOBAL*/ BYTE         g_tail[128];

/*GLOBAL*/ BYTE         *a_alert;

/*GLOBAL*/ OBJECT       *a_trees[RS_NTREE];     /* ptrs to dialog trees */

/*GLOBAL*/ WORD         g_croot;                /* current pseudo root  */

/*GLOBAL*/ WORD         g_cwin;                 /* current window #     */
/*GLOBAL*/ WORD         g_wlastsel;             /* window holding last  */
                                                /*   selection          */
/*GLOBAL*/ WORD         g_csortitem;            /* curr. sort item chked*/
/*GLOBAL*/ WORD         g_ccopypref;            /* curr. copy pref.     */
/*GLOBAL*/ WORD         g_cdelepref;            /* curr. delete pref.   */
/*GLOBAL*/ WORD         g_covwrpref;            /* curr. overwrite pref.*/
/*GLOBAL*/ WORD         g_cdclkpref;            /* curr. double click   */
/*GLOBAL*/ WORD         g_cmclkpref;            /* curr. menu click     */
/*GLOBAL*/ WORD         g_ctimeform;            /* curr. time format    */
/*GLOBAL*/ WORD         g_cdateform;            /* curr. date format    */

/*GLOBAL*/ BYTE         g_1text[256];

/*GLOBAL*/ WORD         g_icw;
/*GLOBAL*/ WORD         g_ich;
/*GLOBAL*/ WORD         g_nmicon;               /* number of points in g_xyicon[] */
/*GLOBAL*/ WORD         g_nmtext;               /* number of points in g_xytext[] */
/*GLOBAL*/ WORD         g_xyicon[18];           /* outline for dragging file as icon */
/*GLOBAL*/ WORD         g_xytext[10];           /* outline for dragging file as text */

/*GLOBAL*/ WORD         g_wicon;
/*GLOBAL*/ WORD         g_hicon;

/*GLOBAL*/ LONG         g_afsize;

/*GLOBAL*/ BYTE         *g_pbuff;               /* pointer to text buffer used by ANODEs */
/*GLOBAL*/ ANODE        *g_alist;               /* pointer to ANODE array */
/*GLOBAL*/ ANODE        *g_aavail;              /* pointer to chain of free ANODEs */
/*GLOBAL*/ ANODE        *g_ahead;               /* pointer to chain of allocated ANODEs */

/*GLOBAL*/ WORD         g_numiblks;             /* number of icon blocks */
/*GLOBAL*/ UWORD        **g_origmask;           /* ptr to array of ptrs to untransformed icon mask */
/*GLOBAL*/ ICONBLK      *g_iblist;              /* ptr to array of icon blocks */

/*GLOBAL*/ CSAVE        g_cnxsave;

/* Number of first free item object within g_screen[]; free objects
 * are chained via ob_next.
 */
        WORD            g_screenfree;

/* This points to an array used to store two types of object:
 * . the desktop and desktop windows
 *      these are stored in the first NUM_WNODES+2 entries
 * . item objects displayed on the desktop and within desktop windows
 *      these are stored in the remaining entries as either G_ICON
 *      or G_USERDEF objects.  if sufficient memory is available, the
 *      number of entries is sufficient to display the desktop and all
 *      available windows, all filled with icons.
 */
        OBJECT          *g_screen;

/* This points to an array used to store the additional information
 * required for the individual item objects on the desktop and in
 * screen windows; therefore this could have NUM_WNODES+2 entries
 * fewer than g_screen[].
 * However, by allocating the same number of entries (and not using
 * the first few), we can use the same value to access entries in both
 * the g_screen[] & g_screeninfo[] arrays.  This has a small cost in
 * RAM usage, but the code itself is simpler and smaller.
 */
        SCREENINFO      *g_screeninfo;
} GLOBES;

#endif  /* _DESKBIND_H */
