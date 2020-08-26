/*
 * deskmain.h - the header for EmuDesk's deskmain.c
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKMAIN_H
#define _DESKMAIN_H

/* architectural */
#define CHAR_WIDTH      8   /* in pixels */

/* menu shortcut specifications */
#define SHORTCUT_SIZE   3   /* allow for ' ^X' in menu items */
#define NUM_SHORTCUTS   33

extern char     gl_amstr[4];
extern char     gl_pmstr[4];

extern WORD     gl_apid;

extern UBYTE    menu_shortcuts[NUM_SHORTCUTS];
extern const UBYTE shortcut_mapping[NUM_SHORTCUTS];

extern GRECT    gl_savewin[];
extern GRECT    gl_normwin;


WORD hndl_msg(void);
BOOL deskmain(void);
void centre_title(OBJECT *tree);
void install_shortcuts(void);

#endif  /* _DESKMAIN_H */
