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

#ifndef CONFIG_H
#define CONFIG_H


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

/* set this to 1 to try autodetect whether STonX
 * native print is available (experimental).
 */
#ifndef DETECT_NATIVE_PRINT
#define DETECT_NATIVE_PRINT 0
#endif

/* set this to 1 to detect, and (if detected) use native features provided
 * by the standard "native features" interface. 
 */
#ifndef DETECT_NATIVE_FEATURES
#define DETECT_NATIVE_FEATURES 1
#endif

/* set this to 1 to redirect debug prints on MIDI out, for emulator
 * without ANY native debug print capabilities.
 * This overrides ..._NATIVE_PRINT and DETECT_NATIVE_FEATURES flags.
 */
#ifndef MIDI_DEBUG_PRINT
#define MIDI_DEBUG_PRINT 0
#endif

/* set this to 1 if your emulator is capable of emulating properly the 
 * STOP opcode (used to spare host CPU burden during loops). This is
 * currently set to zero as STOP does not work well on all emulators.
 */
#ifndef USE_STOP_INSN_TO_FREE_HOST_CPU
#define USE_STOP_INSN_TO_FREE_HOST_CPU 0
#endif

/* Set this to 1 if you want  Timer-D to be initialized.
 * We don't need Timer-D yet, but some software might depend on an
 * initialized Timer-D. On the other side, a running timer D is yet
 * another task for an emulator, e.g. Hatari slows down with it :-(
 */
#ifndef INIT_TIMER_D
#define INIT_TIMER_D 1
#endif

/* The two parameters below are commented out, since the same result
 * can now be obtained by simply doing
 *
 *   make UNIQUE=xx
 *
 * (the Makefile creates a file include/i18nconf.h that contains the 
 * correct setting for those two parameters).
 *
 * Set this to 1 if you want the EmuTOS ROM to contain only one set
 * of keyboards and fonts. The keyboard and fonts will still be those
 * specified as usual (make COUNTRY=xx). 
 *
 * #ifndef CONF_UNIQUE_COUNTRY
 * #define CONF_UNIQUE_COUNTRY 0
 * #endif
 *
 * Set this to 1 if you do not want any Native Language Support (NLS)
 * included in EmuTOS. The only language will be default English.
 *
 * #ifndef CONF_NO_NLS
 * #define CONF_NO_NLS 0
 * #endif
 */
 
/*
 * Set this to 1 to activate experimental ACSI support 
 */
#ifndef CONF_WITH_ACSI
#define CONF_WITH_ACSI 1
#endif

/*
 * Define the TOS version here. Valid values are 0x102 and 0x206 for example.
 * Note that using a value less than 0x200 might force some parts of
 * EmuTOS not to be compiled to save some space in the ROM image.
 */
#ifndef TOS_VERSION
#define TOS_VERSION 0x206
#endif

/*
 * Define the boot timeout in seconds. The higher the number the better
 * chance for the user to read all the initinfo screen.
 * Undefined or defined with zero value will skip the whole boot timeout
 * routine.
 */
#ifndef TIMEOUT_ON_BOOT
#define TIMEOUT_ON_BOOT 8
#endif

/*
 * With this switch you can control if some functions should be used as
 * static-inlines. This is generally a good idea if your compiler supports
 * this (a recent GCC does it!). It will shrink the size of the ROM since
 * only very small functions will be used as static inlines, and it will
 * also make the code faster!
 */
#ifndef USE_STATIC_INLINES
#define USE_STATIC_INLINES 1
#endif


/* The keyboard and language are now set using
 *   make COUNTRY="xx" 
 * where xx is a lowercase two-letter country code as
 * found in the table in bios/country.c
 */

#endif /* CONFIG_H */
