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

#define ENABLE_KDEBUG

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
#include "spi.h"
#include "../bdos/bdosstub.h"

#ifdef MACHINE_DANA

/* Custom registers */
#define ICR      *(volatile UWORD*)0xfffff302
#define IMR      *(volatile ULONG*)0xfffff304
#define ISR      *(volatile ULONG*)0xfffff30c
#define IVR      *(volatile UBYTE*)0xfffff300
#define PBDATA   *(volatile UBYTE*)0xfffff409
#define PBDIR    *(volatile UBYTE*)0xfffff408
#define PBSEL    *(volatile UBYTE*)0xfffff40b
#define PBPUEN   *(volatile UBYTE*)0xfffff40a
#define PJDATA   *(volatile UBYTE*)0xfffff439
#define PJDIR    *(volatile UBYTE*)0xfffff438
#define PJPUEN   *(volatile UBYTE*)0xfffff43a
#define PJSEL    *(volatile UBYTE*)0xfffff43b
#define PLLFSR   *(volatile UWORD*)0xfffff202
#define SPICONT1 *(volatile UWORD*)0xfffff704
#define SPIINTCS *(volatile UWORD*)0xfffff706
#define SPIRXD   *(volatile UWORD*)0xfffff700
#define SPISPC   *(volatile UWORD*)0xfffff70a
#define SPITXD   *(volatile UWORD*)0xfffff702
#define TCMP1    *(volatile UWORD*)0xfffff604
#define TCTL1    *(volatile UWORD*)0xfffff600
#define TPRER    *(volatile UWORD*)0xfffff602
#define URX1     *(volatile UWORD*)0xfffff904
#define USTCNT1  *(volatile UWORD*)0xfffff900
#define UTX1     *(volatile UWORD*)0xfffff906
#define UTX1D    *(volatile UBYTE*)0xfffff907

#define DELAY_MS(n) delay_loop(loopcount_1_msec * n)

static const UBYTE* dana_screenbase;
static ULONG clock_frequency;

#define VECTOR(i) (*(volatile PFVOID*)((i)*4))
#define VEC_USER(i) VECTOR(0x40 + i)

void dana_init(void)
{
	IVR = 0x40; /* map interrupts to autovector vectors */
	ICR = 0x8000;
	IMR = 0x00ffffff; /* all interrupt sources disabled */
	ISR = 0;

	ULONG pllfsr = PLLFSR;
	ULONG qc = (pllfsr >> 8) & 0x3f;
	ULONG pc = pllfsr & 0xff;
	clock_frequency = (pc*14 + qc + 15) * 0x8000;
	KDEBUG(("system clock probably %ld Hz\n", clock_frequency));
  	
	phystop = (UBYTE*) (8L*1024L*1024L);
	bootflags = 0;
}

void dana_init_system_timer(void)
{
    ULONG prescale = 1;
    ULONG dc = (clock_frequency + (CLOCKS_PER_SEC/2)) / CLOCKS_PER_SEC;
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
	KDEBUG(("dana_rs232_init()\n"));
	VEC_USER(4) = dana_int_4;
	USTCNT1 |= 1<<3; /* RXRE enable */
	IMR &= ~(1<<2); /* UART1 interrupts unmask */
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

void spi_initialise(void)
{
	/* Power off the card. */
	 
	PBDATA &= ~(1<<5);
    PBDIR |= 1<<5;
    PBPUEN &= ~(1<<5);
    PBSEL |= 1<<5;
	DELAY_MS(100);

	/* Power on the card */

	PBDATA |= 1<<5;
	DELAY_MS(100);

	PJDATA = (PJDATA & 0xf8) | 0x08;
	PJDIR = (PJDIR & 0xf8) | 0x08;
	PJPUEN = PJPUEN & 0xf0;
	PJSEL = (PJSEL & 0xf8) | 0x08;

	SPISPC = 0;
	SPIINTCS = 0;
	SPICONT1 = (1<<10) /* master mode */
			| (1<<9) /* SPI enabled */
			| 7 /* transfer size of 8 bits */
			;
}

void spi_clock_sd(void)
{
	/* 8MHz */
	//SPICONT1 = (SPICONT1 & 0x1fff) | (0<<13); /* divide by 4 */
}

void spi_clock_mmc(void)
{
	/* 8MHz */
	//SPICONT1 = (SPICONT1 & 0x1fff) | (0<<13); /* divide by 4 */
}

void spi_clock_ident(void)
{
	/* 250khZ */
	SPICONT1 = (SPICONT1 & 0x1fff) | (5<<13); /* divide by 128 */
}

void spi_cs_assert(void)
{
	PJDATA &= ~(1<<3);
}

void spi_cs_unassert(void)
{
	PJDATA |= 1<<3;
}

static UBYTE sendrecv(UBYTE c)
{
	while (SPIINTCS & (1<<2))
		;
	SPITXD = c;
	SPICONT1 |= 1<<8; /* start exchange */
	while (SPICONT1 & (1<<8))
		;
	
	return SPIRXD;
}

void spi_send_byte(UBYTE c)
{
	(void) sendrecv(c);
}

UBYTE spi_recv_byte(void)
{
	return sendrecv(0xff);
}

#endif

