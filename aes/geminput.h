/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMINPUT_H
#define GEMINPUT_H

#include "struct.h"
#include "gemlib.h"

extern WORD     button, xrat, yrat, kstate, mclick, mtrans;
extern WORD     pr_button, pr_xrat, pr_yrat, pr_mclick;

extern PD       *gl_mowner;
extern PD       *gl_cowner;
extern PD       *ctl_pd;
extern GRECT    ctrl;

extern WORD     gl_bclick;
extern WORD     gl_bpend;
extern WORD     gl_bdesired;
extern WORD     gl_btrue;
extern WORD     gl_bdely;


UWORD in_mrect(MOBLK *pmo);
void set_ctrl(GRECT *pt);
void get_ctrl(GRECT *pt);
void get_mown(PD **pmown);
void set_mown(PD *mp);
UWORD dq(CQUEUE *qptr);
void fq(void);
void evremove(EVB *e, UWORD ret);

void kchange(UWORD ch, WORD kstat);
void post_keybd(CDA *c, UWORD ch);
void bchange(WORD new, WORD clicks);
WORD downorup(WORD new, LONG buparm);
void mchange(WORD rx,  WORD ry);

void akbin(EVB *e);
void adelay(EVB *e, LONG c);
void abutton(EVB *e, LONG p);
void amouse(EVB *e, LONG pmo);


#endif
