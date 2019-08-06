/*
 * vdiext.h - EmuTOS VDI extensions not callable with trap
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef _VDIEXT_H
#define _VDIEXT_H

/*
 * maximum number of vertices for v_fillarea(), v_pline(), v_pmarker()
 *
 * TOS2 allows 512, TOS3 allows 1024
 */
#define MAX_VERTICES    256

#endif /* _VDIEXT_H */
