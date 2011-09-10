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

/*
 * Define the TOS version here. Valid values are 0x102 and 0x206 for example.
 * Note that using a value less than 0x200 might force some parts of
 * EmuTOS not to be compiled to save some space in the ROM image.
 */
#ifndef TOS_VERSION
#define TOS_VERSION 0x206
#endif

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
# if defined(__mcoldfire__) || TOS_VERSION < 0x200
#  define DETECT_NATIVE_FEATURES 0 /* Conflict with ColdFire instructions. */
# else
#  define DETECT_NATIVE_FEATURES 1
# endif
#endif

/* set this to 1 to redirect debug prints on RS232 out, for emulator
 * without any native debug print capabilities or real hardware.
 */
#ifndef RS232_DEBUG_PRINT
#define RS232_DEBUG_PRINT 0
#endif

/* set this to 1 to redirect debug prints on MIDI out, for emulator
 * without ANY native debug print capabilities.
 * This overrides previous debug print settings.
 */
#ifndef MIDI_DEBUG_PRINT
#define MIDI_DEBUG_PRINT 0
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
# ifdef MACHINE_ARANYM
   /* ACSI is unsupported on ARAnyM */
#  define CONF_WITH_ACSI 0
# else
#  define CONF_WITH_ACSI 1
# endif
#endif

/*
 * Set CONF_WITH_IDE to 1 to activate Falcon IDE support.
 */
#ifndef CONF_WITH_IDE
# ifdef MACHINE_FIREBEE
   /* The FireBee's CompactFlash card requires this. */
#  define CONF_WITH_IDE 1
# else
   /* For safety, the experimental IDE support is disabled by default. */
#  define CONF_WITH_IDE 0
# endif
#endif

/*
 * Set this to 1 if your emulator is capable of emulating properly the 
 * STOP opcode (used to spare host CPU burden during loops).
 * Set to zero for all emulators which do not properly support STOP opcode.
 */
#ifndef USE_STOP_INSN_TO_FREE_HOST_CPU
# if TOS_VERSION < 0x200
#  define USE_STOP_INSN_TO_FREE_HOST_CPU 0
# else
#  define USE_STOP_INSN_TO_FREE_HOST_CPU 1
# endif
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

/*
 * By default, the EmuTOS welcome screen (initinfo) is only shown on cold boot.
 * If you set ALWAYS_SHOW_INITINFO to 1, the welcome screen will always be
 * displayed, on both cold boot and warm boot (reset).
 * This is typically a good idea on the FireBee where the OS is always started
 * in warm mode.
 */
#ifndef ALWAYS_SHOW_INITINFO
# if defined(EMUTOS_RAM) || defined(MACHINE_FIREBEE)
#  define ALWAYS_SHOW_INITINFO 1
# else
#  define ALWAYS_SHOW_INITINFO 0
# endif
#endif

/*
 * By default, the EmuTOS welcome screen (initinfo) is displayed for 3 seconds.
 * On emulators, this is enough to read the text, and optionally to press Shift
 * to keep the screen displayed. But on real hardware, it can take several
 * seconds for the monitor to recover from stand-by mode, so the welcome screen
 * may never be seen. I such cases, it is wise to increase the welcome screen
 * duration.
 * You can use the INITINFO_DURATION define to specifiy the welcome screen
 * duration, in seconds. If it is set to 0, the welcome screen will never be
 * displayed.
 */
#ifndef INITINFO_DURATION
# ifdef MACHINE_FIREBEE
#  define INITINFO_DURATION 10
# else
#  define INITINFO_DURATION 3
# endif
#endif

/* The keyboard and language are now set using
 *   make COUNTRY="xx" 
 * where xx is a lowercase two-letter country code as
 * found in the table in bios/country.c
 */

#endif /* CONFIG_H */
