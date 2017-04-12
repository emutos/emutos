/*
 * EmuTOS aes
 *
 * Copyright (C) 2002-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMINIT_H
#define GEMINIT_H
#include "gsxdefs.h"

#define DEF_DESKTOP "EMUDESK"   /* default desktop */

extern BYTE     *ad_envrn;

extern MFORM    gl_mouse;
extern BYTE     gl_logdrv;

extern WORD     totpds;
extern WORD     num_accs;

extern THEGLO   D;


void all_run(void);
void set_mouse_to_arrow(void);
void set_mouse_to_hourglass(void);

#endif
