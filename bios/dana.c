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

/* #define ENABLE_KDEBUG */

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
#define URX1 *(volatile UWORD*)0xffffff904
#define UTX1 *(volatile UWORD*)0xffffff906
#define UTX1D *(volatile UBYTE*)0xffffff907

static const UBYTE* dana_screenbase;

void dana_init(void)
{
	phystop = (UBYTE*) (8L*1024L*1024L);
	bootflags |= BOOTFLAG_EARLY_CLI;
}

void dana_init_system_timer(void)
{
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
	return 480L*160L / 8;
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

