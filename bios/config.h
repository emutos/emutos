/*
 * config.h - default settings
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD     Martin Doering
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _CONFIG_H
#define _CONFIG_H

/*
 * File localconf.h will be included if reported present by the Makefile.
 * Use it to put your local configuration. File localconf.h will not be 
 * imported into cvs.
 */

#ifdef LOCALCONF
#include "../localconf.h"
#endif

/*
 * use #ifndef ... #endif for definitions below, to allow them to
 * be overriden by the Makefile or by localconf.h
 */

/* set this to 1 if your emulator provides an STonX-like 
 * native_print() function, i.e. if the code:
 *   dc.w 0xa0ff
 *   dc.l 0
 * executes native function void print_native(char *string);
 */
#ifndef STONX_NATIVE_PRINT
#define STONX_NATIVE_PRINT 0
#endif

/* set this to 1 if you run on ARAnyM >= 0.0.11
 * i.e. if the code:
 *   dc.w 0x7135
 * executes native function void print_native(char *string);
 */
#ifndef ARANYM_NATIVE_PRINT
#define ARANYM_NATIVE_PRINT 0
#endif

/* set this to 1 if your emulator is capable of emulating properly the 
 * STOP opcode (used to spare host CPU burden during loops). This is
 * currently set to zero as STOP does not work well on all emulators.
 */
#ifndef USE_STOP_INSN_TO_FREE_HOST_CPU
#define USE_STOP_INSN_TO_FREE_HOST_CPU 0
#endif

/* The keyboard ans language are now set using
 *   make LOCALE="xx" 
 * where xx is a lowercase two-letter country code as
 * found in the table in bios/country.c
 */

#endif /* _CONFIG_H */
