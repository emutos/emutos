/*
 * EmuTOS AES
 *
 * Copyright (C) 2002-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMEVLIB_H
#define GEMEVLIB_H

extern WORD     gl_dclick;
extern WORD     gl_ticktime;


WORD ev_block(WORD code, LONG lvalue);
UWORD ev_button(WORD bflgclks, UWORD bmask, UWORD bstate, WORD rets[]);
UWORD ev_mouse(MOBLK *pmo, WORD rets[]);
void ev_timer(LONG count);
WORD ev_multi(WORD flags, MOBLK *pmo1, MOBLK *pmo2, LONG tmcount,
              LONG buparm, WORD *mebuff, WORD prets[]);
WORD ev_dclick(WORD rate, WORD setit);

/*
 * combine clicks/mask/state into LONG
 */
static __inline__ LONG combine_cms(WORD clicks,WORD mask,WORD state)
{
    union {
        LONG result;
        struct {
            WORD c;
            BYTE m;
            BYTE s;
        } combined;
    } u;

    u.combined.c = clicks;
    u.combined.m = mask;
    u.combined.s = state;

    return u.result;
}
#endif
