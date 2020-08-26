/*
 * deskact.h - the header for EmuDesk's deskact.c
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKACT_H
#define _DESKACT_H

/* Prototypes: */
WORD act_chg(WORD wh, WORD root, WORD obj, GRECT *pc,
             BOOL set, BOOL dodraw);
void act_allchg(WORD wh, WORD root, WORD ex_obj, GRECT *pt, GRECT *pc,
                BOOL set);
void act_bsclick(WORD wh, WORD root, WORD mx, WORD my, WORD keystate,
                 GRECT *pc, BOOL dclick);
WORD act_bdown(WORD wh, WORD root, WORD *in_mx, WORD *in_my,
               WORD *keystate, GRECT *pc, WORD *pdobj);

#endif  /* _DESKACT_H */
