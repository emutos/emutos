/*
 * aros.h - Interface to the AROS Amiga functions
 *
 * Copyright (c) 2013 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef AROS_H
#define AROS_H

#ifdef MACHINE_AMIGA

void aros_machine_detect(void);
#if CONF_WITH_ALT_RAM
void aros_add_alt_ram(void);
#endif
BOOL aros_flop_detect_drive(WORD dev);
WORD aros_floprw(LONG buf, WORD rw, WORD dev, WORD sect, WORD track, WORD side, WORD count);

#endif /* MACHINE_AMIGA */

#endif /* AROS_H */
