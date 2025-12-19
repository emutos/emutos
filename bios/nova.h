/*
 * nova.h - Nova/Vofa/Wittich adapter graphics card routines
 *
 * Copyright (C) 2018-2025 The EmuTOS development team
 *
 * Authors:
 * Christian Zietz
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef NOVA_H
#define NOVA_H

#if CONF_WITH_NOVA
void detect_nova(void);
BOOL init_nova(void);
UBYTE* get_novamembase(void);
#endif

#endif
