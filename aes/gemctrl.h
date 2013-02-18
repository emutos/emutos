/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMCTRL_H
#define GEMCTRL_H

extern MOBLK    gl_ctwait;
extern WORD     gl_ctmown;
extern WORD     appl_msg[8];
extern const WORD gl_wa[];

void ct_chgown(PD *mpd, GRECT *pr);
void ct_mouse(WORD grabit);
void ctlmgr(void);

#endif
