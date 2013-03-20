/*
 * coldfire.h - ColdFire specific functions
 *
 * Copyright (c) 2013 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef COLDFIRE_H
#define COLDFIRE_H

#ifdef __mcoldfire__
 
#if CONF_WITH_COLDFIRE_RS232
BOOL coldfire_rs232_can_write(void);
void coldfire_rs232_write_byte(UBYTE b);
BOOL coldfire_rs232_can_read(void);
UBYTE coldfire_rs232_read_byte(void);
#endif

#endif /* __mcoldfire__ */

#endif /* COLDFIRE_H */
