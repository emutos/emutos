/*
 * amiga.h - Amiga specific functions
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef AMIGA_H
#define AMIGA_H

#ifdef MACHINE_AMIGA

struct IDE
{
    UBYTE filler00[4];
    UBYTE features; /* Read: error */
    UBYTE filler06[3];
    UBYTE sector_count;
    UBYTE filler0a[3];
    UBYTE sector_number;
    UBYTE filler0e[3];
    UBYTE cylinder_low;
    UBYTE filler12[3];
    UBYTE cylinder_high;
    UBYTE filler16[3];
    UBYTE head;
    UBYTE filler1a[3];
    UBYTE command; /* Read: status */
    UBYTE filler1e[4091];
    UBYTE control; /* Read: Alternate status */
    UBYTE filler1019[3];
    UBYTE address; /* Write: Not used */
    UBYTE filler02[4067];
    UWORD data;
};

#define ide_interface ((volatile struct IDE*)0x00da0000)

extern const UBYTE scancode_atari_from_amiga[128];
extern const UBYTE *amiga_screenbase;
extern UWORD copper_list[6];
extern int has_gayle;

void amiga_machine_detect(void);
#if CONF_WITH_ALT_RAM
void amiga_add_alt_ram(void);
#endif
void amiga_screen_init(void);
void amiga_setphys(const UBYTE *addr);
const UBYTE *amiga_physbase(void);
void amiga_mouse_vbl(void);
void amiga_clock_init(void);
ULONG amiga_getdt(void);
#if CONF_WITH_UAE
void amiga_uaelib_init(void);
void kprintf_outc_uae(int c);
#endif
void amiga_shutdown(void);
BOOL amiga_flop_detect_drive(WORD dev);
WORD amiga_floprw(UBYTE *buf, WORD rw, WORD dev, WORD sect, WORD track, WORD side, WORD count);

/* The following functions are defined in amiga2.S */

void amiga_init_keyboard_interrupt(void);
void amiga_vbl(void);

#endif /* MACHINE_AMIGA */

#endif /* AMIGA_H */
