/*
 * deskrez.c - handle desktop resolution change dialog
 *
 * This file was created to support desktop resolution changes
 * for the TT and Falcon.
 *
 * Copyright (C) 2012-2021 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "aesbind.h"
#include "obdefs.h"
#include "optimize.h"

#include "deskbind.h"
#include "deskglob.h"
#include "deskrsrc.h"
#include "desk_rsc.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "deskinf.h"
#include "deskfun.h"
#include "deskrez.h"
#include "desksupp.h"

#include "bdosbind.h"
#include "xbiosbind.h"
#include "has.h"                /* for has_videl etc */
#include "biosdefs.h"
#include "biosext.h"


#if CONF_WITH_TT_SHIFTER

/*
 * maps TT dialog buttons to resolution
 */
#define NUM_TT_BUTTONS  5
static const WORD ttrez_from_button[NUM_TT_BUTTONS] =
    { ST_LOW, ST_MEDIUM, ST_HIGH, TT_LOW, TT_MEDIUM };

#endif /* CONF_WITH_TT_SHIFTER */

#if CONF_WITH_VIDEL

/*
 * maps Falcon dialog buttons to 'base' mode
 * note: these correspond to the VGA videomode settings; the
 * VIDEL_VERTICAL bit is inverted for RGB mode (see the code)
 */
#define NUM_FALCON_BUTTONS  19
static const WORD falconmode_from_button[NUM_FALCON_BUTTONS] =
    { VIDEL_80COL|VIDEL_1BPP,                       /* 640x480x2 */
      VIDEL_80COL|VIDEL_2BPP,                       /* 640x480x4 */
      VIDEL_80COL|VIDEL_4BPP,                       /* 640x480x16 */
      VIDEL_80COL|VIDEL_8BPP,                       /* 640x480x256 */
      VIDEL_VERTICAL|VIDEL_80COL|VIDEL_1BPP,        /* 640x240x2 */
      VIDEL_VERTICAL|VIDEL_80COL|VIDEL_2BPP,        /* 640x240x4 */
      VIDEL_VERTICAL|VIDEL_80COL|VIDEL_4BPP,        /* 640x240x16 */
      VIDEL_VERTICAL|VIDEL_80COL|VIDEL_8BPP,        /* 640x240x256 */
      VIDEL_2BPP,                                   /* 320x480x4 */
      VIDEL_4BPP,                                   /* 320x480x16 */
      VIDEL_8BPP,                                   /* 320x480x256 */
      VIDEL_TRUECOLOR,                              /* 320x480x65536 */
      VIDEL_VERTICAL|VIDEL_2BPP,                    /* 320x240x4 */
      VIDEL_VERTICAL|VIDEL_4BPP,                    /* 320x240x16 */
      VIDEL_VERTICAL|VIDEL_8BPP,                    /* 320x240x256 */
      VIDEL_VERTICAL|VIDEL_TRUECOLOR,               /* 320x240x65536 */
      VIDEL_COMPAT|VIDEL_80COL|VIDEL_1BPP,                  /* ST High */
      VIDEL_COMPAT|VIDEL_VERTICAL|VIDEL_80COL|VIDEL_2BPP,   /* ST Medium */
      VIDEL_COMPAT|VIDEL_VERTICAL|VIDEL_4BPP };             /* ST Low */

#endif /* CONF_WITH_VIDEL */

/*
 *  change_st_rez(): change desktop ST resolution
 *  returns:    0   user cancelled change
 *              1   user wants to change; newres is updated with new resolution.
 */
static int change_st_rez(WORD *newres)
{
    if (fun_alert(1,STRESOL) != 1)
        return 0;

    *newres = Getrez() ? 0 : 1;
    return 1;
}

#if CONF_WITH_TT_SHIFTER
/*
 *  change_tt_rez(): change desktop TT resolution
 *  returns:    0   user cancelled change
 *              1   user wants to change; newres is updated with new resolution.
 */
static int change_tt_rez(WORD *newres)
{
OBJECT *tree, *obj;
int i, selected;
WORD oldres;

    oldres = Getrez();
    for (i = 0; i < NUM_TT_BUTTONS; i++)
        if (oldres == ttrez_from_button[i])
            break;

    selected = i;

    /* set up dialog & display */
    tree = desk_rs_trees[ADTTREZ];
    for (i = 0, obj = tree+TTREZSTL; i < NUM_TT_BUTTONS; i++, obj++) {
        if (i == selected)
            obj->ob_state |= SELECTED;
        else obj->ob_state &= ~SELECTED;
    }

    inf_show(tree,ROOT);

    if (inf_what(tree,TTREZOK) == 0)
        return 0;

    /* look for button with SELECTED state */
    i = inf_gindex(tree,TTREZSTL,NUM_TT_BUTTONS);
    if (i < 0)                  /* paranoia */
        return 0;
    if (i == selected)          /* no change */
        return 0;

    *newres = ttrez_from_button[i];
    return 1;
}
#endif

#if CONF_WITH_VIDEL
/*
 *  change_falcon_rez(): change desktop Falcon resolution
 *  returns:    0   user cancelled change
 *              1   user wants to change; newres is set to 3, and newmode
 *                  is updated with the new video mode.
 */
static int change_falcon_rez(WORD *newres,WORD *newmode)
{
OBJECT *tree, *obj;
int i, selected;
WORD oldmode, oldbase, oldoptions;
WORD mode;

    oldmode = VsetMode(-1);
    oldbase = oldmode & (VIDEL_VERTICAL|VIDEL_COMPAT|VIDEL_80COL|VIDEL_BPPMASK);
    oldoptions = oldmode & (VIDEL_OVERSCAN|VIDEL_PAL|VIDEL_VGA);
    if (!(oldoptions&VIDEL_VGA))    /* if RGB mode, */
        oldbase ^= VIDEL_VERTICAL;  /* this bit has inverted meaning */

    for (i = 0; i < NUM_FALCON_BUTTONS; i++)
        if (oldbase == falconmode_from_button[i])
            break;
    selected = i;

    /* set up dialog & display */
    tree = desk_rs_trees[ADFALREZ];

    if (VgetMonitor() != MON_VGA) { /* fix up rez descriptions if not VGA */
        for (i = 0, obj = tree+FREZNAME; i < 4; i++, obj++)
            obj->ob_spec = (LONG) desktop_str_addr(STREZ1+i);
    }

    /* FIXME: change the next 2 lines when we have TrueColor support in VDI */
    obj = tree + FREZTEXT;          /* this hides the "TC" header text */
    obj->ob_flags |= HIDETREE;

    for (i = 0, obj = tree+FREZLIST; i < NUM_FALCON_BUTTONS; i++, obj++) {
        /* FIXME: change the next 2 lines when we have TrueColor support in VDI */
        if ((falconmode_from_button[i]&VIDEL_BPPMASK) > VIDEL_8BPP)
            obj->ob_flags |= HIDETREE;
        if (i == selected)
            obj->ob_state |= SELECTED;
        else obj->ob_state &= ~SELECTED;
    }

    inf_show(tree,ROOT);

    if (inf_what(tree,FREZOK) == 0)
        return 0;

    /* look for button with SELECTED state */
    i = inf_gindex(tree,FREZLIST,NUM_FALCON_BUTTONS);
    if (i < 0)                  /* paranoia */
        return 0;
    if (i == selected)          /* no change */
        return 0;

    mode = falconmode_from_button[i] | oldoptions;
    if (!(oldoptions&VIDEL_VGA))    /* if RGB mode, */
        mode ^= VIDEL_VERTICAL;     /* invert the bit returned */

    if (Srealloc(-1L) < VgetSize(mode)) {
        malloc_fail_alert();
        return 0;
    }

    *newres = FALCON_REZ;
    *newmode = mode;

    return 1;
}
#endif

#ifdef MACHINE_AMIGA
/* This assumes that inside ADAMIREZ dialog, buttons are sorted
 * left to right then top to bottom. */
static const WORD amigamode_from_button[] =
{
    VIDEL_PAL|VIDEL_1BPP|VIDEL_80COL|VIDEL_VERTICAL,    /* 640x512 */
    VIDEL_PAL|VIDEL_1BPP|VIDEL_80COL,                   /* 640x256 */
    VIDEL_PAL|VIDEL_1BPP|VIDEL_VERTICAL,                /* 320x512 */
    VIDEL_PAL|VIDEL_1BPP,                               /* 320x256 */
    VIDEL_VGA|VIDEL_1BPP|VIDEL_80COL,                   /* 640x480 */
    VIDEL_VGA|VIDEL_1BPP|VIDEL_80COL|VIDEL_VERTICAL,    /* 640x240 */
    VIDEL_VGA|VIDEL_1BPP,                               /* 320x480 */
    VIDEL_VGA|VIDEL_1BPP|VIDEL_VERTICAL,                /* 320x240 */
    -1, /* Falcon label*/
    VIDEL_1BPP|VIDEL_80COL,                             /* 640x200 */
    VIDEL_1BPP|VIDEL_VERTICAL,                          /* 320x400 */
    VIDEL_1BPP,                                         /* 320x200 */
    -1, /* ST label */
    VIDEL_COMPAT|VIDEL_1BPP|VIDEL_80COL|VIDEL_VERTICAL, /* 640x400 */
};
#define NUM_AMIGA_BUTTONS ARRAY_SIZE(amigamode_from_button)

static int change_amiga_rez(WORD *newres,WORD *newmode)
{
OBJECT *tree, *obj;
int i, selected;
WORD oldmode;

    oldmode = amiga_vgetmode();

    for (i = 0; i < NUM_AMIGA_BUTTONS; i++)
        if (oldmode == amigamode_from_button[i])
            break;
    selected = i;

    /* set up dialog & display */
    tree = desk_rs_trees[ADAMIREZ];

    for (i = 0, obj = tree+AMIREZ0; i < NUM_AMIGA_BUTTONS; i++, obj++) {
        if (i == selected)
            obj->ob_state |= SELECTED;
        else obj->ob_state &= ~SELECTED;
    }

    inf_show(tree,ROOT);

    if (inf_what(tree,AMREZOK) == 0)
        return 0;

    /* look for button with SELECTED state */
    i = inf_gindex(tree,AMIREZ0,NUM_AMIGA_BUTTONS);
    if (i < 0)                  /* paranoia */
        return 0;
    if (i == selected)          /* no change */
        return 0;

    *newres = FALCON_REZ;
    *newmode = amigamode_from_button[i];

    return 1;
}
#endif

/*
 *  change_resolution(): change desktop resolution
 *
 *  note: this is only called when the resolution is changeable;
 *  i.e. it is NOT called in ST high on an ST, or TT high on a TT
 *
 *  returns:    0   user cancelled change
 *              1   user wants to change; newres is updated with new resolution.
 *                  if newres is 3, then it's Falcon-style, and newmode is set
 *                  to the new videomode.
 */
int change_resolution(WORD *newres,WORD *newmode)
{
#ifdef MACHINE_AMIGA
    return change_amiga_rez(newres,newmode);
#endif

#if CONF_WITH_VIDEL
    if (has_videl)
        return change_falcon_rez(newres,newmode);
#endif

#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        return change_tt_rez(newres);
#endif

    return change_st_rez(newres);
}
