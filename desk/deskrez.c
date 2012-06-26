/*
 * deskrez.c - handle desktop resolution change dialog
 *
 * This file was created to support desktop resolution changes
 * for the TT and Falcon.
 *
 * Copyright (c) 2012 EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "config.h"
#include "portab.h"

#include "aesbind.h"
#include "obdefs.h"
#include "deskrsrc.h"
#include "optimize.h"
#include "desk_rsc.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "infodef.h"
#include "deskwin.h"
#include "deskbind.h"
#include "deskglob.h"
#include "deskinf.h"

#include "xbiosbind.h"
#include "machine.h"    /* for has_videl etc */
#include "screen.h"
#include "videl.h"
#include "kprint.h"

#define R_STRING 5      /* this should be obtained from a header! */

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
    if (form_alert(1,(LONG)ini_str(STRESOL)) != 1)
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
LONG tree;
OBJECT *obj;
int i, selected;
WORD oldres;

    oldres = Getrez();
    for (i = 0; i < NUM_TT_BUTTONS; i++)
        if (oldres == ttrez_from_button[i])
            break;

    selected = i;

    /* set up dialog & display */
    tree = G.a_trees[ADTTREZ];
    for (i = 0, obj = (OBJECT *)tree+TTREZSTL; i < NUM_TT_BUTTONS; i++, obj++) {
        if (ttrez_from_button[i] == TT_LOW) /* FIXME: remove these 2 lines when */
            obj->ob_state |= DISABLED;      /* we have 256-colour support in VDI */
        if (i == selected)
            obj->ob_state |= SELECTED;
        else obj->ob_state &= ~SELECTED;
    }

    inf_show(tree,ROOT);

    if (inf_what(tree,TTREZOK,TTREZCAN) == 0)
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
LONG tree;
OBJECT *obj;
int i, selected;
WORD oldmode, oldbase, oldoptions;

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
    tree = G.a_trees[ADFALREZ];

    if (VgetMonitor() != MON_VGA) { /* fix up rez descriptions if not VGA */
        for (i = 0, obj = (OBJECT *)tree+FREZNAME; i < 4; i++, obj++)
            rsrc_gaddr(R_STRING,STREZ1+i,&obj->ob_spec);
    }

    /* FIXME: change the next 2 lines when we have 256-colour support in VDI */
    obj = (OBJECT *)tree+FREZTEXT;  /* this hides the "256  TC" header text */
    obj->ob_flags |= HIDETREE;

    for (i = 0, obj = (OBJECT *)tree+FREZLIST; i < NUM_FALCON_BUTTONS; i++, obj++) {
        /* FIXME: change the next 2 lines when we have 256-colour support in VDI */
        if ((falconmode_from_button[i]&VIDEL_BPPMASK) > VIDEL_4BPP)
            obj->ob_flags |= HIDETREE;
        if (i == selected)
            obj->ob_state |= SELECTED;
        else obj->ob_state &= ~SELECTED;
    }

    inf_show(tree,ROOT);

    if (inf_what(tree,FREZOK,FREZCAN) == 0)
        return 0;

    /* look for button with SELECTED state */
    i = inf_gindex(tree,FREZLIST,NUM_FALCON_BUTTONS);
    if (i < 0)                  /* paranoia */
        return 0;
    if (i == selected)          /* no change */
        return 0;

    *newres = FALCON_REZ;
    *newmode = falconmode_from_button[i] | oldoptions;
    if (!(oldoptions&VIDEL_VGA))    /* if RGB mode, */
        *newmode ^= VIDEL_VERTICAL; /* invert the bit returned */

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
