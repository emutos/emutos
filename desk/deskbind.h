/*      DESKBIND.H              12/13/84-06/09/85       Lee Lorenzen    */
/*      for 3.0                 4/9/86  - 5/5/86        MDF             */
/*      for 2.3                 9/25/87                 mdf             */
/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
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
#include "bdosdefs.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "aesext.h"

#define DESKWH  0               /* the desktop's window handle */

#define ARROW   0
#define HGLASS  2
#define FLATHAND 4

#define ALLFILES    (FA_SUBDIR|FA_SYSTEM|FA_HIDDEN)

/*
 * attributes used to select files/folders for desktop window display.
 * used by pn_active() when building the list of FNODEs to display, and
 * by search_recursive() when searching for matching FNODEs.
 */
#define DISPATTR    FA_SUBDIR


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
 * It is also convenient to store a pointer to the FNODE of the file that
 * the screen object represents, so that we can mark a file/folder as
 * selected when we select the object.
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

typedef struct
{
    FNODE *fnptr;       /* ptr to FNODE corresponding to screen object (if applicable) */
    union {
        ICONINFO icon;  /* relevant when the corresponding object is a G_ICON */
        USERBLK udef;   /* relevant when the corresponding object is a G_USERDEF */
    } u;
} SCREENINFO;


#if CONF_WITH_BACKGROUNDS
/*
 * the following structure is used in GLOBES to store the current
 * background pattern/colour values for the desktop & windows, for
 * a resolution with a given number of planes.  note that only the
 * low-order byte is used.
 */
typedef struct
{
    WORD desktop;       /* for desktop background */
    WORD window;        /* for window backgrounds */
} PATCOL;
#endif


/*
 * the number of points required for outlining an item when dragging it
 */
#define NUM_ICON_POINTS 9   /* item is an icon (on desktop or in window) */
#define NUM_TEXT_POINTS 5   /* item is text (in window) */


/*
 * The desktop global data area
 */
typedef struct
{
/*GLOBAL*/ WORD         g_stdrv;                /* start drive */

/*GLOBAL*/ DTA          g_wdta;                 /* general use DTA */

                                            /* desktop window management: */
/*GLOBAL*/ WNODE        g_wdesktop;             /* the desktop pseudo-window */
/*GLOBAL*/ WNODE        *g_wlist;               /* ptr to array of WNODEs */
/*GLOBAL*/ WNODE        *g_wfirst;              /* current top window     */
/*GLOBAL*/ WORD         g_wcnt;                 /* # WNODEs currently allocated */

                                            /* view-related parms:      */
/*GLOBAL*/ WORD         g_iview;                /* current view type (V_ICON/V_TEXT) */
/*GLOBAL*/ WORD         g_iwext;                /* w,h of extent of a   */
/*GLOBAL*/ WORD         g_ihext;                /*   single item        */
/*GLOBAL*/ WORD         g_iwint;                /* w,h of interval      */
/*GLOBAL*/ WORD         g_ihint;                /*   between items      */
/*GLOBAL*/ WORD         g_iwspc;                /* w,h of space used by */
/*GLOBAL*/ WORD         g_ihspc;                /*   a single item      */
/*GLOBAL*/ WORD         g_isort;                /* current sort type (S_NAME etc) */

#if CONF_WITH_SIZE_TO_FIT
/*GLOBAL*/ BOOL         g_ifit;                 /* size to fit flag     */
/*GLOBAL*/ WORD         g_icols;                /* number of columns in full window */
#endif

                                            /* for recursive counts in a given path: */
/*GLOBAL*/ LONG         g_nfiles;               /* files */
/*GLOBAL*/ LONG         g_ndirs;                /* folders */
/*GLOBAL*/ LONG         g_size;                 /* total filesize */

/*GLOBAL*/ WORD         g_rmsg[8];              /* general AES message area */

/*GLOBAL*/ WORD         g_xdesk;                /* desktop work area coordinates */
/*GLOBAL*/ WORD         g_ydesk;
/*GLOBAL*/ WORD         g_wdesk;
/*GLOBAL*/ WORD         g_hdesk;

/*GLOBAL*/ WORD         g_croot;                /* current pseudo root  */

/*GLOBAL*/ WORD         g_cwin;                 /* current window #     */
/*GLOBAL*/ WORD         g_wlastsel;             /* window holding last selection */

                                            /* current desktop preference values: */
/*GLOBAL*/ char         g_ccopypref;            /* confirm copy (boolean)       */
/*GLOBAL*/ char         g_cdelepref;            /* confirm delete (boolean)     */
/*GLOBAL*/ char         g_covwrpref;            /* confirm overwrite (boolean)  */
/*GLOBAL*/ char         g_cdclkpref;            /* double click speed           */
/*GLOBAL*/ char         g_cmclkpref;            /* click for menu (boolean)     */
/*GLOBAL*/ char         g_ctimeform;            /* time format                  */
/*GLOBAL*/ char         g_cdateform;            /* date format                  */
/*GLOBAL*/ char         g_blitter;              /* blitter enabled (boolean)    */
/*GLOBAL*/ char         g_cache;                /* cache enabled (boolean)      */
/*GLOBAL*/ char         g_appdir;               /* default is app dir (boolean) */
/*GLOBAL*/ char         g_fullpath;             /* full path for arg (boolean)  */

/*GLOBAL*/ LONG         g_idt;                  /* value from _IDT cookie */

/*GLOBAL*/ char         g_work[256];            /* general text work area */

                                            /* icon-related globals: */
/*GLOBAL*/ WORD         g_icw;                  /* width (pixels, incl spacing)  */
/*GLOBAL*/ WORD         g_ich;                  /* height (pixels, incl spacing) */
/*GLOBAL*/ Point        g_xyicon[NUM_ICON_POINTS];  /* outline for dragging file as icon */
/*GLOBAL*/ Point        g_xytext[NUM_TEXT_POINTS];  /* outline for dragging file as text */
/*GLOBAL*/ WORD         g_wicon;                /* width (pixels, excl spacing)  */
/*GLOBAL*/ WORD         g_hicon;                /* height (pixels, excl spacing) */

                                            /* ANODE-related variables */
/*GLOBAL*/ char         *g_atext;               /* ptr to text buffer */
/*GLOBAL*/ ANODE        *g_alist;               /* pointer to ANODE array */
/*GLOBAL*/ ANODE        *g_aavail;              /* pointer to chain of free ANODEs */
/*GLOBAL*/ ANODE        *g_ahead;               /* pointer to chain of allocated ANODEs */

/*GLOBAL*/ WORD         g_numiblks;             /* number of icon blocks */
/*GLOBAL*/ ICONBLK      *g_iblist;              /* ptr to array of icon blocks */

/* This points to the desktop context save area, which is a staging
 * area between the current desktop preferences (see above) and the
 * EMUDESK.INF file.
 */
        CSAVE           *g_cnxsave;             /* ptr to context save area */

#if CONF_WITH_BACKGROUNDS
/* Default pattern/colour for desktop/windows:
 * [0] applies to 1-plane (ST high, TT high)
 * [1] applies to 2-plane (ST medium)
 * [2] applies to everything else
 */
        PATCOL          g_patcol[3];
#endif

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
