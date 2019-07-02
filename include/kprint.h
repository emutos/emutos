/*
 *  kprint.h - header file for keyboard/console routines
 *
 * Copyright (C) 2001-2019 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef KPRINT_H
#define KPRINT_H

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

/* KINFO(()) outputs to the debugger, if kprintf() is available */
#if HAS_KPRINTF
#define KINFO(args) kprintf args
#else
#define KINFO(args) NULL_FUNCTION()
#endif

/* KDEBUG(()) may call KINFO(()) when locally enabled */
#ifdef ENABLE_KDEBUG
#define KDEBUG(args) KINFO(args)
#else
#define KDEBUG(args) NULL_FUNCTION()
#endif

/* print a panic message both via kprintf and cprintf, then halt */
extern void panic(const char *fmt, ...) PRINTF_STYLE NORETURN;

/* halt the machine */
extern void halt(void) NORETURN;

#endif /* KPRINT_H */
