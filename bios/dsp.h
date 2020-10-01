/*
 * dsp.h - DSP routines
 *
 * Copyright (C) 2020 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DSP_H
#define _DSP_H

#if CONF_WITH_DSP

void detect_dsp(void);
void dsp_init(void);

/* XBIOS DSP functions */
WORD dsp_getwordsize(void);
WORD dsp_lock(void);
void dsp_unlock(void);

#endif /* CONF_WITH_DSP */

#endif /* _DSP_H */
