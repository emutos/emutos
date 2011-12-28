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
 * Defaults for the ARAnyM target
 */
#ifdef MACHINE_ARANYM
# ifndef CONF_WITH_ACSI
#  define CONF_WITH_ACSI 0
# endif
# ifndef CONF_WITH_MEGARTC
#  define CONF_WITH_MEGARTC 0
# endif
# ifndef CONF_WITH_IKBD_CLOCK
#  define CONF_WITH_IKBD_CLOCK 0
# endif
# ifndef CONF_WITH_CARTRIDGE
#  define CONF_WITH_CARTRIDGE 0
# endif
#endif

/*
 * Defaults for the FireBee target
 */
#ifdef MACHINE_FIREBEE
# ifndef CONF_WITH_IDE
#  define CONF_WITH_IDE 1 /* For CompactFlash support */
# endif
# ifndef CONF_WITH_MEGARTC
#  define CONF_WITH_MEGARTC 0
# endif
# ifndef ALWAYS_SHOW_INITINFO
#  define ALWAYS_SHOW_INITINFO 1 /* Because the FireBee always do warm boot */
# endif
# ifndef INITINFO_DURATION
#  define INITINFO_DURATION 8 /* Longer time for LCD monitors startup */
# endif
#endif

/*
 * Defaults for the 192 target.
 * This target is only useful on ST hardware, and the ROM size is very limited,
 * so strip out all the advanced features.
 */
#ifdef TARGET_192
# ifndef TOS_VERSION
#  define TOS_VERSION 0x102
# endif
# ifndef DETECT_NATIVE_FEATURES
#  define DETECT_NATIVE_FEATURES 0
# endif
# ifndef CONF_WITH_ALT_RAM
#  define CONF_WITH_ALT_RAM 0
# endif
# ifndef CONF_WITH_FASTRAM
#  define CONF_WITH_FASTRAM 0
# endif
# ifndef CONF_WITH_FALCON_MMU
#  define CONF_WITH_FALCON_MMU 0
# endif
# ifndef CONF_WITH_VIDEL
#  define CONF_WITH_VIDEL 0
# endif
# ifndef CONF_WITH_TT_SHIFTER
#  define CONF_WITH_TT_SHIFTER 0
# endif
# ifndef CONF_WITH_STE_SHIFTER
#  define CONF_WITH_STE_SHIFTER 0
# endif
# ifndef CONF_WITH_DMASOUND
#  define CONF_WITH_DMASOUND 0
# endif
# ifndef CONF_WITH_VME
#  define CONF_WITH_VME 0
# endif
# ifndef CONF_WITH_DIP_SWITCHES
#  define CONF_WITH_DIP_SWITCHES 0
# endif
# ifndef CONF_WITH_NVRAM
#  define CONF_WITH_NVRAM 0
# endif
# ifndef CONF_WITH_XHDI
#  define CONF_WITH_XHDI 0
# endif
# ifndef CONF_WITH_ASSERT
#  define CONF_WITH_ASSERT 0
# endif
# ifndef USE_STOP_INSN_TO_FREE_HOST_CPU
#  define USE_STOP_INSN_TO_FREE_HOST_CPU 0 /* Good idea to disable ? */
# endif
# ifndef CONF_WITH_DESK1
#  define CONF_WITH_DESK1 0 /* Sad but necessary to fit size requirements */
# endif
# ifndef CONF_WITH_DESKTOP_ICONS
#  define CONF_WITH_DESKTOP_ICONS 0
# endif
# ifndef CONF_WITH_EASTER_EGG
#  define CONF_WITH_EASTER_EGG 0
# endif
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
# define TOS_VERSION 0x206
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
# ifdef __mcoldfire__
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

/*
 * Set DIAGNOSTIC_CARTRIDGE to 1 when building a diagnostic cartridge.
 */
#ifndef DIAGNOSTIC_CARTRIDGE
# define DIAGNOSTIC_CARTRIDGE 0
#endif

/*
 * Define CONF_STRAM_SIZE to the actual size of the ST-RAM, in bytes.
 * If set to 0, the amount of ST-RAM will be autodetected.
 */
#ifndef CONF_STRAM_SIZE
# define CONF_STRAM_SIZE 0
#endif

/*
 * Set CONF_WITH_ALT_RAM to 1 to add support for alternate RAM
 */
#ifndef CONF_WITH_ALT_RAM
# define CONF_WITH_ALT_RAM 1
#endif

/*
 * Set CONF_WITH_FASTRAM to 1 to enable detection and usage of FastRAM (TT-RAM)
 */
#ifndef CONF_WITH_FASTRAM
# define CONF_WITH_FASTRAM 1
#endif

/*
 * Define CONF_FASTRAM_SIZE to the actual size of the FastRAM, in bytes.
 * If set to 0, the amount of FastRAM will be autodetected.
 */
#ifndef CONF_FASTRAM_SIZE
# define CONF_FASTRAM_SIZE 0
#endif

/*
 * Set CONF_WITH_FDC to 1 to enable floppy disk controller support
 */
#ifndef CONF_WITH_FDC
# define CONF_WITH_FDC 1
#endif

/*
 * Set this to 1 to activate experimental ACSI support 
 */
#ifndef CONF_WITH_ACSI
# define CONF_WITH_ACSI 1
#endif

/*
 * Set CONF_WITH_IDE to 1 to activate Falcon IDE support.
 */
#ifndef CONF_WITH_IDE
  /* For safety, the experimental IDE support is disabled by default. */
# define CONF_WITH_IDE 0
#endif

/*
 * Set CONF_WITH_ST_MMU to 1 to enable support for ST MMU.
 */
#ifndef CONF_WITH_ST_MMU
# define CONF_WITH_ST_MMU 1
#endif

/*
 * Set CONF_WITH_FALCON_MMU to 1 to enable support for Falcon MMU.
 */
#ifndef CONF_WITH_FALCON_MMU
# define CONF_WITH_FALCON_MMU 1
#endif

/*
 * Set CONF_WITH_VIDEL to 1 to enable support for Falcon Videl.
 */
#ifndef CONF_WITH_VIDEL
# define CONF_WITH_VIDEL 1
#endif

/*
 * Set CONF_WITH_TT_SHIFTER to 1 to enable support for TT Shifter
 */
#ifndef CONF_WITH_TT_SHIFTER
# define CONF_WITH_TT_SHIFTER 1
#endif

/*
 * Set CONF_WITH_STE_SHIFTER to 1 to enable support for STe Shifter
 */
#ifndef CONF_WITH_STE_SHIFTER
# define CONF_WITH_STE_SHIFTER 1
#endif

/*
 * Set CONF_WITH_SHIFTER to 1 to enable general Shifter support
 */
#ifndef CONF_WITH_SHIFTER
# define CONF_WITH_SHIFTER 1
#endif

/*
 * CONF_VRAM_ADDRESS allows to set the video ram address to a fixed location,
 * outside ST-RAM or FastRam. This allows using custom graphic cards.
 * Set to 0 to allocate the video ram in the ST-RAM as usual.
 */
#ifndef CONF_VRAM_ADDRESS
# define CONF_VRAM_ADDRESS 0
#endif

/*
 * Set CONF_WITH_DMASOUND to 1 to enable support for STe/TT/Falcon DMA sound
 */
#ifndef CONF_WITH_DMASOUND
# define CONF_WITH_DMASOUND 1
#endif

/*
 * Set CONF_WITH_VME to 1 to enable support for Mega STe VME bus
 */
#ifndef CONF_WITH_VME
# define CONF_WITH_VME 1
#endif

/*
 * Set CONF_WITH_DIP_SWITCHES to 1 to enable support for STe/TT/Falcon DIP switches
 */
#ifndef CONF_WITH_DIP_SWITCHES
# define CONF_WITH_DIP_SWITCHES 1
#endif

/*
 * Set CONF_WITH_NVRAM to 1 to enable NVRAM support
 */
#ifndef CONF_WITH_NVRAM
# define CONF_WITH_NVRAM 1
#endif

/*
 * Set CONF_WITH_MEGARTC to 1 to enable MegaST real-time clock support
 */
#ifndef CONF_WITH_MEGARTC
# define CONF_WITH_MEGARTC 1
#endif

/*
 * Set CONF_WITH_MFP to 1 to enable support for the MFP 68901.
 */
#ifndef CONF_WITH_MFP
# define CONF_WITH_MFP 1
#endif

/*
 * Set CONF_WITH_MFP_RS232 to 1 to enable MFP RS-232 support
 */
#ifndef CONF_WITH_MFP_RS232
# define CONF_WITH_MFP_RS232 1
#endif

/*
 * Set CONF_WITH_YM2149 to 1 to enable YM2149 soundchip support
 */
#ifndef CONF_WITH_YM2149
# define CONF_WITH_YM2149 1
#endif

/*
 * Set CONF_WITH_PRINTER_PORT to 1 to enable Parallel Printer Port support
 */
#ifndef CONF_WITH_PRINTER_PORT
# define CONF_WITH_PRINTER_PORT 1
#endif

/*
 * Set CONF_WITH_MIDI_ACIA to 1 to enable MIDI ACIA support
 */
#ifndef CONF_WITH_MIDI_ACIA
# define CONF_WITH_MIDI_ACIA 1
#endif

/*
 * Set CONF_WITH_IKBD_ACIA to 1 to enable IKBD ACIA support
 */
#ifndef CONF_WITH_IKBD_ACIA
# define CONF_WITH_IKBD_ACIA 1
#endif

/*
 * Set CONF_WITH_IKBD_CLOCK to 1 to enable IKBD clock support
 */
#ifndef CONF_WITH_IKBD_CLOCK
# define CONF_WITH_IKBD_CLOCK 1
#endif

/*
 * Set CONF_WITH_CARTRIDGE to 1 to enable ROM port cartridge support
 */
#ifndef CONF_WITH_CARTRIDGE
# if DIAGNOSTIC_CARTRIDGE
   /* Diagnostic and Application cartridges have different magic numbers,
    * so a diagnostic cartridge can't also be an application cartridge. */
#  define CONF_WITH_CARTRIDGE 0
# else
#  define CONF_WITH_CARTRIDGE 1
# endif
#endif

/*
 * Set CONF_WITH_XHDI to 1 to enable XHDI support (i.e. the XHDI cookie etc.)
 */
#ifndef CONF_WITH_XHDI
# define CONF_WITH_XHDI 1
#endif

/*
 * Set CONF_WITH_ASSERT to 1 to enable the assert() function support.
 */
#ifndef CONF_WITH_ASSERT
# define CONF_WITH_ASSERT 1
#endif

/*
 * Set this to 1 if your emulator is capable of emulating properly the 
 * STOP opcode (used to spare host CPU burden during loops).
 * Set to zero for all emulators which do not properly support STOP opcode.
 */
#ifndef USE_STOP_INSN_TO_FREE_HOST_CPU
# define USE_STOP_INSN_TO_FREE_HOST_CPU 1
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
 * Set FULL_INITINFO to 0 to display the EmuTOS version as a single line of text
 * instead of the full welcome screen.
 * This is only useful when there are severe ROM size restrictions.
 */
#ifndef FULL_INITINFO
# define FULL_INITINFO 1
#endif

/*
 * By default, the EmuTOS welcome screen (initinfo) is only shown on cold boot.
 * If you set ALWAYS_SHOW_INITINFO to 1, the welcome screen will always be
 * displayed, on both cold boot and warm boot (reset).
 * This is typically a good idea on the FireBee where the OS is always started
 * in warm mode.
 */
#ifndef ALWAYS_SHOW_INITINFO
# ifdef EMUTOS_RAM
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
# define INITINFO_DURATION 3
#endif

/*
 * Define CONF_WITH_DESK1 to 1 to use the modern PC-GEM v1.0 style desktop.
 * Define it to 0 to use the old desktop with 2 fixed windows.
 */
#ifndef CONF_WITH_DESK1
# define CONF_WITH_DESK1 1
#endif
#if CONF_WITH_DESK1
  /* The current EmuDesk sources test the presence of this define */
# define DESK1
#endif

/*
 * Set CONF_WITH_DESKTOP_ICONS to 1 to include all the desktop icons.
 */
#ifndef CONF_WITH_DESKTOP_ICONS
# define CONF_WITH_DESKTOP_ICONS 1
#endif

/*
 * Set CONF_WITH_EASTER_EGG to 1 to include EmuDesk Easter Egg.
 */
#ifndef CONF_WITH_EASTER_EGG
# define CONF_WITH_EASTER_EGG 1
#endif

/*
 * Miscellaneous definitions that apply to more than one EmuTOS subsystem
 */
#define BLKDEVNUM 26                    /* number of block devices supported: A: ... Z: */
/*
 * Maximum lengths for pathname, filename, and filename components
 */
#define LEN_ZPATH 67                    /* max path length, incl drive */
#define LEN_ZFNAME 13                   /* max fname length, incl '\' separator */
#define LEN_ZNODE 8                     /* max node length */
#define LEN_ZEXT 3                      /* max extension length */

/*
 * Sanity checks
 */

#if !CONF_WITH_YM2149
# if CONF_WITH_FDC
#  error "CONF_WITH_FDC requires CONF_WITH_YM2149."
# endif
#endif

#if !CONF_WITH_ALT_RAM
# if CONF_WITH_FASTRAM
#  error "CONF_WITH_FASTRAM requires CONF_WITH_ALT_RAM."
# endif
#endif

#if !CONF_WITH_MFP
# if CONF_WITH_MFP_RS232
#  error "CONF_WITH_MFP_RS232 requires CONF_WITH_MFP."
# endif
# if CONF_WITH_PRINTER_PORT
#  error "CONF_WITH_PRINTER_PORT requires CONF_WITH_MFP."
# endif
# if CONF_WITH_FDC
#  error "CONF_WITH_FDC requires CONF_WITH_MFP."
# endif
# if CONF_WITH_IKBD_ACIA
#  error "CONF_WITH_IKBD_ACIA requires CONF_WITH_MFP."
# endif
# if CONF_WITH_MIDI_ACIA
#  error "CONF_WITH_MIDI_ACIA requires CONF_WITH_MFP."
# endif
#endif

#if !CONF_WITH_SHIFTER
# if CONF_WITH_STE_SHIFTER
#  error "CONF_WITH_STE_SHIFTER requires CONF_WITH_SHIFTER."
# endif
# if CONF_WITH_TT_SHIFTER
#  error "CONF_WITH_TT_SHIFTER requires CONF_WITH_SHIFTER."
# endif
# if CONF_WITH_VIDEL
#  error "CONF_WITH_VIDEL requires CONF_WITH_SHIFTER."
# endif
#endif

#if !CONF_WITH_DESKTOP_ICONS
# if CONF_WITH_DESK1
#  error "CONF_WITH_DESK1 requires CONF_WITH_DESKTOP_ICONS."
# endif
#endif

#endif /* CONFIG_H */
