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

#if CONF_COLDFIRE_TIMER_C
void coldfire_init_system_timer(void);
void coldfire_int_61(void); /* In coldfire2.S */
#endif

#ifdef MACHINE_M548X
const char* m548x_machine_name(void);
# if CONF_WITH_IDE
void m548x_init_cpld(void);
# endif
#endif /* MACHINE_M548X */

#endif /* __mcoldfire__ */

#endif /* COLDFIRE_H */
