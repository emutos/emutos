/*
 * bdosext.h - EmuTOS BDOS extensions not callable with trap
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BDOSEXT_H
#define _BDOSEXT_H

/* BDOS initialization entry points. Called by BIOS. */
extern void osinit_before_xmaddalt(void);
extern void osinit_after_xmaddalt(void);

#endif /* _BDOSEXT_H */
