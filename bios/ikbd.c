/*
 * ikbd.c - Intelligent keyboard routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  MAD   Martin Doering
 *        Robert de Vries (Linux m68k)
 *        Bjoern Brauel   (Linux m68k)
 *        Roman Hodek     (Linux m68k)
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 * Some code I got from Linux m68k, thanks to the authors! (MAD)
 */

/*
 * LVL: I rewrote this, taking bits from kbd.c and kbq.c,
 * to be more compliant with the iorec stuff.
 *
 * not supported: 
 * - mouse move using alt-arrowkeys
 * - alt-help screen hardcopy
 * - alt-numeric-pad to enter characters by their ASCII code
 * - KEYTBL.TBL config with _AKP cookie (tos 5.00 and later)
 * - CLRHOME and INSERT in kbshift.
 * - key repeat
 */

#include "country.h"

#include "config.h"
#include "portab.h"
#include "bios.h"
#include "acia.h"
#include "kprint.h"
#include "tosvars.h"
#include "lineavars.h"
#include "tosvars.h"
#include "iorec.h"
#include "asm.h"
#include "ikbd.h"
#include "sound.h"              /* for keyclick */


#define DBG_KBD 0



/* scancode definitions */
#define KEY_RELEASED 0x80       /* This bit set, when key-release scancode */

#define KEY_LSHIFT  0x2a
#define KEY_RSHIFT  0x36
#define KEY_CTRL    0x1d
#define KEY_ALT     0x38
#define KEY_CAPS    0x3a


/*
 * These bit flags are set in the "modes" byte based on the state
 * of control keys on the keyboard, like:
 * right shift, left shift, control, alt, ...
 */

/* mode types - these are the different bits */
#define MODE_RSHIFT 0x01        /* right shift keys is down */
#define MODE_LSHIFT 0x02        /* right shift keys is down */
#define MODE_CTRL   0x04        /* CTRL is down. */
#define MODE_ALT    0x08        /* ALT is down.                        */
#define MODE_CAPS   0x10        /* ALPHA LOCK is down.                 */
#define MODE_CLEAR  0x20        /* CLR/HOME mode key is down    */

#define MODE_SHIFT   (MODE_RSHIFT|MODE_LSHIFT)  /* shifted */



/*==== Global variables ===================================================*/
BYTE shifty;                    /* reflect the status up/down of mode keys */


/*=== Keymaps handling (xbios) =======================================*/

static struct keytbl current_keytbl;

LONG keytbl(LONG norm, LONG shft, LONG caps)
{
    if (norm != -1L) {
        current_keytbl.norm = (BYTE *) norm;
    }
    if (shft != -1L) {
        current_keytbl.shft = (BYTE *) shft;
    }
    if (caps != -1L) {
        current_keytbl.caps = (BYTE *) caps;
    }
    return (LONG) & current_keytbl;
}

void bioskeys(void)
{
    struct keytbl *tbl;

    /* ask country.c for the key table */
    get_keytbl(&tbl);
    /* set it as the current table */
    current_keytbl = *tbl;
}

/*=== kbrate (xbios) =====================================================*/

WORD kbrate(WORD initial, WORD repeat)
{
    /* TODO */
    return 0;
}


/*=== kbshift (bios) =====================================================*/

LONG kbshift(WORD flag)
{
    WORD oldy;

    if (flag == -1)
        return (shifty);        /* return bitvector of shift state */

    oldy = shifty;
    shifty = flag;

    return (oldy);
}

/*=== iorec handling (bios) ==============================================*/

LONG bconstat2(void)
{
    if (ikbdiorec.head == ikbdiorec.tail) {
        return 0;               /* iorec empty */
    } else {
        return -1;              /* not empty => input available */
    }
}

LONG bconin2(void)
{
    WORD old_sr;
    LONG value;

    /* keep current value of sr before changing it using STOP */
    old_sr = get_sr();

    while (!bconstat2()) {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        /* set sr = 0x2300 and stop the CPU until the next interrupt.
         * This allows VBL and MFP interrupts, but no HBL interrupts.
         * TODO - A problem could occur if the IPL mask was intentionnally 
         * set to something higher than 3 before calling bconin. If so, 
         * based on the value of old_sr, we should use different stop opcodes.
         */
        stop2300();
#endif
    }
    /* disable interrupts */
    set_sr(0x2700);

    ikbdiorec.head += 4;
    if (ikbdiorec.head >= ikbdiorec.size) {
        ikbdiorec.head = 0;
    }
    value = *(LONG *) (ikbdiorec.buf + ikbdiorec.head);

    /* restore interrupts */
    set_sr(old_sr);
    return value;
}

static void push_ikbdiorec(LONG value)
{
    ikbdiorec.tail += 4;
    if (ikbdiorec.tail >= ikbdiorec.size) {
        ikbdiorec.tail = 0;
    }
    if (ikbdiorec.tail == ikbdiorec.size) {
        /* iorec full */
        return;
    }
    *(LONG *) (ikbdiorec.buf + ikbdiorec.tail) = value;
}

/*=== interrupt routine support ===================================*/

/*
 * kbd_int : called by the interrupt routine for key events.
 */

void kbd_int(WORD scancode)
{
    LONG value;                 /* the value to push into iorec */
    UBYTE ascii = 0;


#if DBG_KBD
    kprint("================\n ");
    kprintf("Key-scancode: 0x%02x\n", scancode & 0xff);

    kprintf("Key-shift bits: 0x%02x\n", shifty);
#endif

    /* keyboard warm/cold start */
    if ((scancode == 0x53)  /* Del key and shifty is Alt+Ctrl but not LShift */
        && ((shifty & (MODE_ALT|MODE_CTRL|MODE_LSHIFT)) == (MODE_ALT|MODE_CTRL))) {
        if (shifty & MODE_RSHIFT) {
            /* Alt+Ctrl+RShift means cold start */
            memvalid = 0;   /* enforce cold start by resetting memvalid */
        }
        os_entry();  /* restart system */
    }

    if (scancode & KEY_RELEASED) {
        scancode &= ~KEY_RELEASED;      /* get rid of release bits */
        switch (scancode) {
        case KEY_RSHIFT:
            shifty &= ~MODE_RSHIFT;     /* clear bit */
            break;
        case KEY_LSHIFT:
            shifty &= ~MODE_LSHIFT;     /* clear bit */
            break;
        case KEY_CTRL:
            shifty &= ~MODE_CTRL;       /* clear bit */
            break;
        case KEY_ALT:
            shifty &= ~MODE_ALT;        /* clear bit */
            break;
        }
        /* The TOS does not return when ALT is set, to emulate
         * mouse movement using alt keys. This feature is not 
         * currently supported by EmuTOS.
         */
#if 0
        if (!(shifty & KEY_ALT))
#endif
            return;
    }

    switch (scancode) {
    case KEY_RSHIFT:
        shifty |= MODE_RSHIFT;  /* set bit */
        return;
    case KEY_LSHIFT:
        shifty |= MODE_LSHIFT;  /* set bit */
        return;
    case KEY_CTRL:
        shifty |= MODE_CTRL;    /* set bit */
        return;
    case KEY_ALT:
        shifty |= MODE_ALT;     /* set bit */
        return;
    case KEY_CAPS:
        shifty ^= MODE_CAPS;    /* toggle bit */
        if (conterm & 1)
            keyclick();
        return;
    }

    if (shifty & MODE_ALT) {
        BYTE *a;

        if (shifty & MODE_SHIFT) {
            a = current_keytbl.altshft;
        } else if (shifty & MODE_CAPS) {
            a = current_keytbl.altcaps;
        } else {
            a = current_keytbl.altnorm;
        }
        while (*a && *a != scancode) {
            a += 2;
        }
        if (*a++) {
            ascii = *a;
        }
    } else {
        if (shifty & MODE_SHIFT) {
            if (scancode >= 0x3B && scancode <= 0x44) {
                scancode += 0x19;
                goto push_value;
            }
            ascii = current_keytbl.shft[scancode];
        } else if (shifty & MODE_CAPS) {
            ascii = current_keytbl.caps[scancode];
        } else {
            ascii = current_keytbl.norm[scancode];
        }
    }

    if (shifty & MODE_CTRL) {
        /* More complicated in TOS, but is it really necessary ? */
        ascii &= 0x1F;
    }


  push_value:
    if (conterm & 1)
        keyclick();
    value = ((LONG) scancode & 0xFF) << 16;
    value += ascii;
    if (conterm & 0x8) {
        value += ((LONG) shifty) << 24;
    }
#if DBG_KBD
    kprintf("KBD iorec: Pushing value 0x%08lx\n", value);
#endif
    push_ikbdiorec(value);
}


/*=== ikbd acia stuff ==================================================*/

/* can we send a byte to the ikbd ? */
LONG bcostat4(void)
{
    if (ikbd_acia.ctrl & ACIA_TDRE) {
        return -1;              /* OK */
    } else {
        /* Data register not empty */
        return 0;               /* not OK */
    }
}

/* send a byte to the IKBD */
void bconout4(WORD dev, WORD c)
{
    while (!bcostat4());
    ikbd_acia.data = c;
}

/* cnt = number of bytes to send less one */
void ikbdws(WORD cnt, PTR ptr)
{
    UBYTE *p = (UBYTE *) ptr;
    while (cnt-- >= 0)
        bconout4(0, *p++);
}

/* send a byte to the IKBD - for general use */
void ikbd_writeb(BYTE b)
{
    while (!bcostat4());
    ikbd_acia.data = b;
}

/* send a word to the IKBD as two bytes - for general use */
void ikbd_writew(WORD w)
{
    while (!bcostat4());
    ikbd_acia.data = (w>>8);

    while (!bcostat4());
    ikbd_acia.data = (w&0xff);
}



/*
 *      FUNCTION:  This routine resets the keyboard,
 *        configures the MFP so we can get interrupts
 */

void kbd_init(void)
{
    /* initialize ikbd ACIA */
    ikbd_acia.ctrl = ACIA_RESET;        /* master reset */

    ikbd_acia.ctrl = ACIA_RIE | /* enable interrupts */
        ACIA_RLTID |            /* RTS low, TxINT disabled */
        ACIA_DIV64 |            /* clock/64 */
        ACIA_D8N1S;             /* 8 bit, 1 stop, no parity */

    /* initialize the IKBD */
    ikbd_writeb(0x80);            /* Reset */
    ikbd_writeb(0x01);

    ikbd_writeb(0x1A);            /* disable joystick */
    ikbd_writeb(0x12);            /* disable mouse */

    bioskeys();
}
