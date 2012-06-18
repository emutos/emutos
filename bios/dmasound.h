/*
 * dmasound.h - STe/TT/Falcon DMA sound routines
 *
 * Copyright (c) 2011 EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef DMASOUND_H
#define DMASOUND_H

#if CONF_WITH_DMASOUND

void detect_dmasound(void);
void dmasound_init(void);

#endif /* CONF_WITH_DMASOUND */

#endif /* DMASOUND_H */
