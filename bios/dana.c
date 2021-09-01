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
#include "spi.h"
#include "serport.h"
#include "../bdos/bdosstub.h"

#ifdef MACHINE_DANA

/* Custom registers */
#define ICR      *(volatile UWORD*)0xfffff302
#define IMR      *(volatile ULONG*)0xfffff304
#define ISR      *(volatile ULONG*)0xfffff30c
#define IVR      *(volatile UBYTE*)0xfffff300
#define LACDRC   *(volatile UBYTE*)0xfffffa23
#define LCKCON   *(volatile UBYTE*)0xfffffa27
#define LGPMR    *(volatile UBYTE*)0xfffffa33
#define LPICF    *(volatile UBYTE*)0xfffffa20
#define LPOLCF   *(volatile UBYTE*)0xfffffa21
#define LPOSR    *(volatile UBYTE*)0xfffffa2d
#define LPXCD    *(volatile UBYTE*)0xfffffa25
#define LRAA     *(volatile UBYTE*)0xfffffa28
#define LRRA     *(volatile UWORD*)0xfffffa28
#define LSSA     *(volatile ULONG*)0xfffffa00
#define LVPW     *(volatile UBYTE*)0xfffffa05
#define LXMAX    *(volatile UWORD*)0xfffffa08
#define LYMAX    *(volatile UWORD*)0xfffffa0a
#define PBDATA   *(volatile UBYTE*)0xfffff409
#define PBDIR    *(volatile UBYTE*)0xfffff408
#define PBPUEN   *(volatile UBYTE*)0xfffff40a
#define PBSEL    *(volatile UBYTE*)0xfffff40b
#define PCPDEN   *(volatile UBYTE*)0xfffff412
#define PCSEL    *(volatile UBYTE*)0xfffff413
#define PDDATA   *(volatile UBYTE*)0xfffff419
#define PDSEL    *(volatile UBYTE*)0xfffff41b
#define PEDATA   *(volatile UBYTE*)0xfffff421
#define PESEL    *(volatile UBYTE*)0xfffff423
#define PFDATA   *(volatile UBYTE*)0xfffff429
#define PFPUEN   *(volatile UBYTE*)0xfffff42a
#define PFSEL    *(volatile UBYTE*)0xfffff42b
#define PGDATA   *(volatile UBYTE*)0xfffff431
#define PJDATA   *(volatile UBYTE*)0xfffff439
#define PJDIR    *(volatile UBYTE*)0xfffff438
#define PJPUEN   *(volatile UBYTE*)0xfffff43a
#define PJSEL    *(volatile UBYTE*)0xfffff43b
#define PKDIR    *(volatile UBYTE*)0xfffff440
#define PKDATA   *(volatile UBYTE*)0xfffff441
#define PKSEL    *(volatile UBYTE*)0xfffff443
#define PLLFSR   *(volatile UWORD*)0xfffff202
#define PWMR     *(volatile UWORD*)0xfffffa36
#define SPICONT1 *(volatile UWORD*)0xfffff704
#define SPICONT2 *(volatile UWORD*)0xfffff802
#define SPIDATA2 *(volatile UWORD*)0xfffff800
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

#define ST_READY (1<<2)
#define ST_RESET (1<<6)
#define ST_CS (1<<3)

#define PEN_IRQ (1<<1) /* port F */
#define PEN_CS (1<<2) /* port G */

#define DELAY_MS(n) delay_loop(loopcount_1_msec * n)

static ULONG clock_frequency;

#define VECTOR(i) (*(volatile PFVOID*)((i)*4))
#define VEC_USER(i) VECTOR(0x40 + i)

static UBYTE keys_pressed[8];
#include "dana_keymap.h"

/*
 * GPIO pin usage that we're aware of, and init time SEL:
 *
 * B: 60, 01100000
 * 		7
 * 		6 ST micontroller reset
 * 		5
 * 		4
 * 		3
 * 		2
 * 		1
 * 		0
 * D: 10, 00010000
 * 		7 battery?
 * 		6
 * 		5
 * 		4 ST microcontroller IRQ, IRQ1
 * 		3
 * 		2
 * 		1
 * 		0
 * E: c8, 11001000
 * 		7
 * 		6
 * 		5
 * 		4
 * 		3 ST microcontroller chip select
 * 		2
 * 		1
 * 		0 battery?
 * F: c5, 11000101
 * 		7
 * 		6 LCD contrast data bit
 * 		5
 * 		4
 * 		3
 * 		2 
 * 		1 pen interrupt
 * 		0
 * G:
 * 		7
 * 		6
 * 		5
 * 		4
 * 		3
 * 		2 pen + battery
 * 		1 battery?
 * 		0
 * J:
 * 		7
 * 		6 /charger enable
 * 		5
 * 		4
 * 		3
 * 		2
 * 		1
 * 		0
 * K: fd, 11111101
 * 		7 LCD power?
 * 		6 LCD contrast clock
 * 		5
 * 		4 backlight on/off
 * 		3 LCD contrast start/stop bit
 * 		2 ST microcontroller
 * 		1
 * 		0
 */

static void delay_us(ULONG us)
{
	ULONG ticks = ((us>>8) + (us>>4) + us)>>4;
	UWORD then = PLLFSR & 0x8000;
	while (ticks--)
	{
		UWORD now;
		do
			now = PLLFSR & 0x8000;
		while (now == then);
		then = now;
	}
}

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
  	
	phystop = (UBYTE*) (4L*1024L*1024L);
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

	VEC_USER(6) = dana_int_6;

	/* Assuming the system clock is at 16MHz. */
	TPRER = prescale - 1;
	TCMP1 = cv;

	TCTL1 = 0x33;
	IMR &= 0xfffffd;
}

void dana_rs232_init(void)
{
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

void dana_rs232_interrupt(UBYTE b)
{
#if CONF_SERIAL_CONSOLE && !CONF_SERIAL_CONSOLE_POLLING_MODE
	push_ascii_ikbdiorec(b);
#else
	push_serial_iorec(b);
#endif
}

void dana_screen_init(void)
{
	PCSEL = 0;
	PCPDEN = 0;
	PFSEL |= 0x01; /* enable LCONTRAST */
	PKSEL |= 0x90; /* LCD power, backlight GPIO */
	PKDATA |= 0x80; /* backlight, LCD on */

	LCKCON = 0; /* LCD controller off */
	LPICF = 8; /* Four-bit bus, black and white mode */
	LVPW = DANA_SCREEN_WIDTH / 16;
    LXMAX = DANA_SCREEN_WIDTH;
    LYMAX = DANA_SCREEN_HEIGHT - 1;
    LPOLCF = 0;
    LACDRC = 0;
    LPXCD = 3;
    LCKCON = 0x80;
    LRRA = 0x50;
    LPOSR = 0;

	dana_set_lcd_contrast(100); /* reasonable initial contrast */
}

ULONG dana_initial_vram_size(void)
{
	return (DANA_SCREEN_WIDTH * DANA_SCREEN_HEIGHT) / 8;
};

void dana_setphys(const UBYTE *addr)
{
    LSSA = (ULONG) addr;
}

const UBYTE* dana_physbase(void)
{
    return (UBYTE*) LSSA;
}

static UBYTE st_send_recv(UBYTE b)
{
	SPICONT2 = 0x2200; /* enable SPI2 */
	PEDATA &= ~ST_CS;
	SPIDATA2 = b;
	SPICONT2 = 0x2307; /* send/recv byte */
	while (!(SPICONT2 & (1<<7)))
		;
	PEDATA |= ST_CS;
	return SPIDATA2;
}

static void st_reset(void)
{
	PKSEL |= ST_READY; /* ready pin is GPIO */
	PBSEL |= ST_RESET; /* reset pin is GPIO */
	PESEL |= ST_CS;    /* CS pin is GPIO */

	for (;;)
	{
		PKDATA &= ~ST_READY;
		PKDIR |= ST_READY; /* set to output */
		delay_us(32);
		PKDIR &= ~ST_READY; /* set to input */
		if (!(PKDATA & ST_READY))
		{
			/* Reset ST controller */

			PBDATA |= ST_RESET;
			delay_us(15); /* short */
			PBDATA &= ~ST_RESET;
			delay_us(1000);
		}
		else
		{
			delay_us(7000);

			if (st_send_recv(0) == 0xaa)
				break;
		}
	}

	memset(keys_pressed, 0, sizeof(keys_pressed));
}

static UWORD pen_send_recv(UBYTE b)
{
	WORD old_sr = set_sr(0x2700); /* interrupts off */

	SPICONT2 = 0x2247; /* enable SPI2 */
	SPIDATA2 = b;
	PGDATA &= ~PEN_CS;
	SPICONT2 = 0x2347; /* send/recv byte */
	while (!(SPICONT2 & (1<<7)))
		;

	SPIDATA2 = 0;
	SPICONT2 = 0x224f;
	SPICONT2 = 0x234f; /* send/recv sixteen bits */
	while (!(SPICONT2 & (1<<7)))
		;
	PGDATA |= PEN_CS;
	WORD data = SPIDATA2;
	set_sr(old_sr);

	return data;
}

void dana_kbd_init(void)
{
	KDEBUG(("dana_kbd_init\n"));

	st_reset();

	PFPUEN &= ~PEN_CS; /* no pull-up resistor */
	PFSEL |= PEN_CS; /* pen interrupt is GPIO */

#if 0
	for (;;)
	{
		WORD x = pen_send_recv(0x90);
		WORD y = pen_send_recv(0xd0);
		KDEBUG(("pen said %04x %04x %02x\n", x, y, PFDATA));
	}
#endif

	#if 0
	/* Interrupts don't work how I expect, so we're polling instead. */
	VEC_USER(1) = dana_int_1;
	IMR &= ~(1L<<16); /* unmask IRQ1 */
	PDSEL &= ~0x10; /* connect up ST microntroller interrupt line */
	#endif
}

void dana_ikbd_interrupt(void)
{
	SPICONT2 = 0x2200; /* enable SPI2 */
	PEDATA &= ~ST_CS;
	SPIDATA2 = 0;
	SPICONT2 = 0x2307; /* send/recv byte */
	while (!(SPICONT2 & (1<<7)))
		;
	PEDATA |= ST_CS;

	UBYTE b = SPIDATA2;
	KDEBUG(("controller said %02x\n", b));
}

static void press_release_key(UBYTE scancode, BOOL pressed)
{
	UBYTE atari = dana_to_atari_keymap[scancode & 0x7f];
	if (!atari)
		return;

	call_ikbdraw(atari | (pressed ? 0 : 0x80));
}

static BOOL find_key(UBYTE* buffer, UBYTE scancode)
{
	int i;
	for (i=0; i<8; i++)
		if (buffer[i] == scancode)
			return TRUE;
	return FALSE;
}

void dana_poll_keyboard(void)
{
	UBYTE opcode = st_send_recv(0);
	if (!opcode)
		return;

	if (opcode & 0x40) /* keyboard command */
	{
		if (opcode & 0x10) /* dunno */
			return;

		UBYTE new_keys[8] = {0};

		int key_count = opcode & 0x0f;
		UBYTE* k = new_keys;
		while (key_count--)
		{
			delay_us(32);
			*k++ = st_send_recv(0);
		}

		int i;
		for (i=0; i<8; i++)
		{
			UBYTE k = keys_pressed[i];
			if (k && !find_key(new_keys, k))
				press_release_key(k, FALSE);

			k = new_keys[i];
			if (k && !find_key(keys_pressed, k))
				press_release_key(k, TRUE);
		}

		memcpy(keys_pressed, new_keys, 8);
	}
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
	SPICONT1 = (SPICONT1 & 0x1fff) | (0<<13); /* divide by 4 */
}

void spi_clock_mmc(void)
{
	/* 8MHz */
	SPICONT1 = (SPICONT1 & 0x1fff) | (0<<13); /* divide by 4 */
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

