/*
 * detect.h - detect hardware features
 *
 * Copyright (c) 2001 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

long check_read_byte(long);
long detect_cpu(void);
long detect_fpu(void);

