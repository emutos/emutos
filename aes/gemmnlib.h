/*
 * gemmnlib.h - header for EmuTOS AES Menu Library functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMMNLIB_H
#define GEMMNLIB_H


extern OBJECT   *gl_mntree;
extern AESPD    *gl_mnppd;

extern WORD     gl_dabox;


UWORD do_chg(OBJECT *tree, WORD iitem, UWORD chgvalue,
             WORD dochg, WORD dodraw, WORD chkdisabled);
WORD mn_do(WORD *ptitle, WORD *pitem);

void mn_bar(OBJECT *tree, WORD showit);
void mn_clsda(void);
void mn_init(void);
WORD mn_register(WORD pid, char *pstr);
void mn_unregister(WORD da_id);
void mn_getownid(AESPD **owner,WORD *id,WORD item);


#endif
