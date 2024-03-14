/*
 * lisa.h - Apple Lisa specific functions
 *
 * Copyright (C) 2021-2024 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef LISA_H
#define LISA_H

#ifdef MACHINE_LISA

/* Interrupt handlers */
void lisa_int_1(void); /* Raw INT1 handler, defined in lisa2.S */
void lisa_int_1_c(void);
void lisa_int_2(void); /* Raw INT2 handler, defined in lisa2.S */
void lisa_int_2_c(void);

/* Other functions */
void lisa_call_5ms(void); /* Defined in lisa2.S */
void lisa_setphys(const UBYTE *addr);
const UBYTE *lisa_physbase(void);
void lisa_screen_init(void);
void lisa_kbd_init(void);
void lisa_init_system_timer(void);
ULONG lisa_getdt(void);
void lisa_shutdown(void);

void lisa_read_lba_sector(ULONG lba, UBYTE *buffer); /* Defined in lisa2.S */

void lisa_floppy_init(void);
BOOL lisa_flop_detect_drive(WORD dev);
WORD lisa_floprw(UBYTE *buf, WORD rw, WORD dev, WORD sect, WORD track, WORD side, WORD count);
LONG lisa_flop_mediach(WORD dev);
void lisa_flop_eject(void);

#endif /* MACHINE_LISA */

#endif /* LISA_H */
