/*
 *  kprint.h - header file for keyboard/console routines
 *
 * Copyright (c) 2001 EmuTOS .
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef KPRINT_H
#define KPRINT_H

#include "portab.h"

extern WORD boot_status;
#define RS232_AVAILABLE 0x01
#define MIDI_AVAILABLE  0x02
#define DOS_AVAILABLE   0x04
#define SCC_AVAILABLE   0x08

#ifdef __GNUC__
#define PRINTF_STYLE __attribute__ ((format (printf, 1, 2)))
#else
#define PRINTF_STYLE
#endif

/* LVL - A handy macro used when debugging */
#ifdef __GNUC__
#define HERE kprintf("HERE %s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
#else
#define HERE kprintf("HERE %s:%d\n", __FILE__, __LINE__);
#endif

/* console output */
extern int cprintf(const char *fmt, ...) PRINTF_STYLE;

/* native debugging output */
extern int kprintf(const char *fmt, ...) PRINTF_STYLE;

/* output done both through kprintf and cprintf */
extern int kcprintf(const char *fmt, ...) PRINTF_STYLE;

/* assert stuff */
#if CONF_WITH_ASSERT
extern void doassert(const char *, long, const char *, const char *);
#define assert(a) if(!(a)) { doassert(__FILE__, __LINE__, __FUNCTION__, #a); }
#else
#define assert(a) do { } while (0)
#endif

/* functions below implemented in panicasm.S */

/* print a panic message both via kprintf and cprintf, then halt */
extern void panic(const char *fmt, ...) PRINTF_STYLE NORETURN;

/* halt the machine */
extern void halt(void) NORETURN;

/* Invalidate the RAM configuration and reset the computer to the ROM OS */
void cold_reset(void) NORETURN;

/* display information found in 0x380 and halt */
extern void dopanic(const char *fmt, ...) PRINTF_STYLE NORETURN;

#endif /* KPRINT_H */
