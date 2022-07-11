/*
 * gemmnext.h - header for AES 3.30 menu library extensions
 *
 * Copyright (C) 2021-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _GEMMNEXT_H
#define _GEMMNEXT_H

/*
 * the structure used to pass data to menu_popup(), menu_attach()
 */
typedef struct {
        OBJECT  *mn_tree;       /* submenu tree */
        WORD    mn_menu;        /* parent object of menu items (enclosing box) */
        WORD    mn_item;        /* starting item number */
        WORD    mn_scroll;      /* menu scroll flag */
        WORD    mn_keystate;    /* current shiftkey state */
} MENU;

/*
 * the structure used to pass data to menu_settings()
 */
typedef struct {
        LONG    display;        /* submenu-display delay (msecs) */
        LONG    drag;           /* submenu-drag delay (msecs) */
        LONG    delay;          /* single-click scroll delay (msecs) */
        LONG    speed;          /* continuous scroll delay (msecs) */
        WORD    height;         /* default scroll height (items) */
} MN_SET;

/*
 * globals for SMIB array
 */
extern SMIB *gl_submenu;
extern SMIB *gl_submenu_hwm;

void mnext_init(void);
WORD mn_attach(WORD flag, OBJECT *tree, WORD item, MENU *mdata);
WORD mn_istart(WORD flag, OBJECT *tree, WORD menu, WORD start);
WORD mn_popup(MENU *menu, WORD xpos, WORD ypos, MENU *mdata);
void mn_settings(WORD flag, MN_SET *set);

OBJECT *display_submenu(OBJECT *tree, WORD objnum, WORD *smroot);
void undisplay_submenu(OBJECT *tree, WORD objnum);

#endif
