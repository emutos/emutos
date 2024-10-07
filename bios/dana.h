/*
 * dana.h - Dana specific functions
 *
 * Copyright (C) 2013-2019 The EmuTOS development team
 *
 * Authors:
 *  DG      David Given
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef DANA_H
#define DANA_H

#ifdef MACHINE_DANA

#define DANA_SCREEN_WIDTH 560L
#define DANA_SCREEN_HEIGHT 160L

extern void dana_init(void);

extern void dana_init_system_timer(void);

extern void dana_set_lcd_contrast(UBYTE contrast);
extern void dana_screen_init(void);
extern ULONG dana_initial_vram_size(void);
extern void dana_setphys(const UBYTE *addr);
extern const UBYTE *dana_physbase(void);

extern void dana_rs232_init(void);
extern BOOL dana_rs232_can_write(void);
extern void dana_rs232_writeb(UBYTE b);
extern void dana_rs232_interrupt(UBYTE b);

extern void dana_kbd_init(void);
extern void dana_ikbd_interrupt(void);
extern void dana_poll_keyboard(void);
extern void dana_poll_touchscreen(void);
extern void dana_ikbd_writeb(UBYTE b);
extern void dana_ts_rawread(UWORD* x, UWORD* y, UWORD* state);
extern void dana_ts_calibrate(LONG c[7]);

extern void dana_int_1(void);
extern void dana_int_4(void);
extern void dana_int_6(void);

#endif /* MACHINE_DANA */

#endif /* DANA_H */

/* vim: set ts=4 sw=4 et: */

