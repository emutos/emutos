/*
 * dana.c - Amiga specific functions
 *
 * Copyright (C) 2013-2020 The EmuTOS development team
 *
 * Authors:
 *  DG		David Given
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

//#define ENABLE_KDEBUG

#include "emutos.h"
#include "dana.h"
#include "vectors.h"
#include "tosvars.h"
#include "bios.h"
#include "processor.h"
#include "biosext.h"            /* for cache control routines */
#include "gemerror.h"
#include "ikbd.h"               /* for call_mousevec() */
#include "screen.h"
#include "videl.h"
#include "delay.h"
#include "asm.h"
#include "string.h"
#include "disk.h"
#include "biosmem.h"
#include "bootparams.h"
#include "machine.h"
#include "has.h"
#include "../bdos/bdosstub.h"

#ifdef MACHINE_DANA

/* Custom registers */
#define URX1   *(volatile UWORD*)0xfffff904
#define UTX1   *(volatile UWORD*)0xfffff906
#define UTX1D  *(volatile UBYTE*)0xfffff907
#define IVR    *(volatile UBYTE*)0xfffff300
#define ICR    *(volatile UWORD*)0xfffff302
#define IMR    *(volatile ULONG*)0xfffff304
#define ISR    *(volatile ULONG*)0xfffff30c
#define TCTL1  *(volatile UWORD*)0xfffff600
#define TPRER  *(volatile UWORD*)0xfffff602
#define TCMP1  *(volatile UWORD*)0xfffff604
#define PLLFSR *(volatile UWORD*)0xfffff202

static const UBYTE* dana_screenbase;

#define VECTOR(i) (*(volatile PFVOID*)((i)*4))
#define VEC_USER(i) VECTOR(0x40 + i)

void dana_init(void)
{
	IVR = 0x40; /* map interrupts to autovector vectors */
	ICR = 0x8000;
	IMR = 0x00ffffff; /* all interrupt sources disabled */
	ISR = 0;

	phystop = (UBYTE*) (8L*1024L*1024L);
	bootflags |= BOOTFLAG_EARLY_CLI;
}

void dana_init_system_timer(void)
{
	ULONG pllfsr = PLLFSR;
	ULONG qc = (pllfsr >> 8) & 0x3f;
	ULONG pc = pllfsr & 0xff;
	ULONG clockfreq = (pc*14 + qc + 15) * 0x8000;
	KDEBUG(("system clock probably %ld Hz\n", clockfreq));
  	
    ULONG prescale = 1;
    ULONG dc = (clockfreq + (CLOCKS_PER_SEC/2)) / CLOCKS_PER_SEC;
    ULONG cv = dc;
    while (cv > 0xffff) {
      prescale += 1;
      cv = dc / prescale;
    }
	KDEBUG(("prescale=%ld cv=%ld\n", prescale, cv));

	VEC_USER(6) = dana_int_6;

	/* Assuming the system clock is at 16MHz. */
	TPRER = prescale - 1;
	TCMP1 = cv;

	TCTL1 = 0x33;
	IMR &= 0xfffffd;
}

void dana_rs232_init(void)
{
}

BOOL dana_rs232_can_write(void)
{
    return UTX1 & (1<<13);
}

void dana_rs232_writeb(UBYTE b)
{
    while (!dana_rs232_can_write())
    {
        /* Wait */
    }

    /* Send the byte */
	UTX1D = b;
}

void dana_screen_init(void)
{
}

ULONG dana_initial_vram_size(void)
{
	return 560L*160L / 8;
};

void dana_setphys(const UBYTE *addr)
{
    KDEBUG(("dana_setphys(%p)\n", addr));
    dana_screenbase = addr;
}

const UBYTE *dana_physbase(void)
{
    return dana_screenbase;
}

void dana_ikbd_writeb(UBYTE b)
{
#if CONF_SERIAL_IKBD
    dana_rs232_writeb(b);
#endif
}

#endif

