/*
 * gemmnlib.h - header for EmuTOS AES Menu Library functions
 *
 * Copyright (C) 2002-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMMNLIB_H
#define GEMMNLIB_H

/* thickness of menu outline */
#define MENU_THICKNESS  1

extern OBJECT   *gl_mntree;
extern AESPD    *gl_mnppd;

extern WORD     gl_dafirst;


BOOL do_chg(OBJECT *tree, WORD iitem, UWORD chgvalue,
             WORD dochg, WORD dodraw, WORD chkdisabled);

#if CONF_WITH_MENU_EXTENSION
WORD mn_do(WORD *ptitle, WORD *pitem, OBJECT **ptree);
#else
WORD mn_do(WORD *ptitle, WORD *pitem);
#endif

void mn_bar(OBJECT *tree, WORD showit);
void mn_cleanup(void);
void mn_init(void);
WORD mn_register(WORD pid, char *pstr);
void mn_getownid(AESPD **owner,WORD *id,WORD item);


#endif
