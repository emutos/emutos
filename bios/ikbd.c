/*
 * ikbd.c - Intelligent keyboard routines
 *
 * Copyright (c) 2001-2013 The EmuTOS development team
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
 * not supported:
 * - mouse move using alt-arrowkeys
 * - alt-help screen hardcopy
 * - KEYTBL.TBL config with _AKP cookie (tos 5.00 and later)
 * - CLRHOME and INSERT in kbshift.
 */

#include "config.h"
#include "country.h"
#include "portab.h"
#include "acia.h"
#include "kprint.h"
#include "tosvars.h"
#include "lineavars.h"
#include "tosvars.h"
#include "iorec.h"
#include "asm.h"
#include "ikbd.h"
#include "sound.h"              /* for keyclick */
#include "delay.h"
#include "bios.h"

#define DBG_KBD 0



/* scancode definitions */
#define KEY_RELEASED 0x80       /* This bit set, when key-release scancode */

#define KEY_LSHIFT  0x2a
#define KEY_RSHIFT  0x36
#define KEY_CTRL    0x1d
#define KEY_ALT     0x38
#define KEY_CAPS    0x3a


/*=== Keymaps handling (xbios) =======================================*/

static struct keytbl current_keytbl;

LONG keytbl(UBYTE* norm, UBYTE* shft, UBYTE* caps)
{
    if (norm != (UBYTE*)-1) {
        current_keytbl.norm = norm;
    }
    if (shft != (UBYTE*)-1) {
        current_keytbl.shft = shft;
    }
    if (caps != (UBYTE*)-1) {
        current_keytbl.caps = caps;
    }
    return (LONG) & current_keytbl;
}

void bioskeys(void)
{
    const struct keytbl *tbl;

    /* ask country.c for the key table */
    get_keytbl(&tbl);
    /* set it as the current table */
    current_keytbl = *tbl;
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
#if CONF_SERIAL_CONSOLE
    return bconstat(1);
#endif

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

#if CONF_SERIAL_CONSOLE
    value = bconin(1);

    /* We need to emulate these scancodes if we want to launch EmuCon
     * from EmuDesk by using the keyboard shortcuts.
     */
    if (value == 0x11) /* ^Q */
        value |= 0x100000; /* Scancode of Q */
    if (value == 0x1a) /* ^Z */
        value |= 0x2c0000; /* Scancode of Z */

    return value;
#endif

    while (!bconstat2()) {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        stop_until_interrupt();
#endif
    }
    /* disable interrupts */
    old_sr = set_sr(0x2700);

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

/*=== kbrate (xbios) =====================================================*/

/*
 * key repeat is handled as follows:
 * As a key is hit and enters the buffer, it is recorded as kb_last_key;
 * at the same time a downward counter kb_ticks is set to the first delay,
 * kb_initial.
 * Every fourth timer C interrupt (50 Hz ticks), kb_tick is decremented if
 * not null (kb_tick null means no more key repeat for this key). If kb_tick
 * reaches zero, the key is re-emited and kb_tick is now set to the second
 * delay kb_repeat.
 * Finally, any kind of key depress IKBD event stops the repeat handling
 * by setting kb_tick back to zero.
 * There is no need to disable interrupts when modifying kb_tick, kb_initial
 * and kb_repeat since:
 * - each routine not in interrupt routines only accesses kb_tick once;
 * - the interrupt routines (ACIA and timer C) cannot happen at the same
 *   time (they both have IPL level 6);
 * - interrupt routines do not modify kb_initial and kb_repeat.
 */

static WORD kb_initial;
static WORD kb_repeat;
static WORD kb_ticks;
static LONG kb_last_key;
static PFVOID kb_last_ikbdsys; /* ikbdsys when kb_last_key was set */

WORD kbrate(WORD initial, WORD repeat)
{
    WORD ret = (kb_initial << 8) | (kb_repeat & 0xFF);
    if(initial > 1)
        kb_initial = initial;
    if(repeat > 1)
        kb_repeat = repeat;
    return ret;
}

static void do_key_repeat(void)
{
    if (kb_ticks > 0 && kbdvecs.ikbdsys != kb_last_ikbdsys)
    {
        /*
         * There is a key repeat in progress, but the ikbdsys handler has
         * changed since the key was pressed. The new handler may handle
         * the key repeat totally differently, so stop the current key repeat
         * right now.
         *
         * Concretely, this scenario happens when starting FreeMiNT on ARAnyM.
         * When the user presses a key at the EmuTOS welcome screen,
         * FreeMiNT installs its new ikbdsys handler before the user has time
         * to release the key. So this EmuTOS key repeat routine keeps running
         * indefinitely, filling FreeMiNT's keyboard buffer, but without
         * producing keyboard events. As soon as the user presses any key,
         * he sees the ghost repeating keys.
         *
         * This trick fixes the problem.
         */
        kb_ticks = 0;
        return;
    }

    if (!(conterm & 2))
    {
        /* Key repeat is globally disabled */
        return;
    }

    if (kb_ticks == 0)
    {
        /* No key is currently repeating */
        return;
    }

    /* Decrease delay */
    if (--kb_ticks > 0)
    {
        /* Nothing to do this time */
        return;
    }

    /* Play the key click sound */
    if (conterm & 1)
        keyclick((UBYTE)((kb_last_key & 0x00ff0000) >> 16));

    /* Simulate a key press */
    push_ikbdiorec(kb_last_key);

    /* The key will repeat again until some key up */
    kb_ticks = kb_repeat;
}

/* Keyboard timer interrupt handler.
 * It is called at 50 Hz (each 4th Timer C call).
 */
void kb_timerc_int(void)
{
    do_key_repeat();
}


/*=== interrupt routine support ===================================*/

/* the number of the current dead key if any, else -1. */
static int kb_dead;

/* the decimal number collected in an alt_keypad sequence, or -1 */
static int kb_altnum;

/*
 * kbd_int : called by the interrupt routine for key events.
 */

void kbd_int(UBYTE scancode)
{
    LONG value;                 /* the value to push into iorec */
    UBYTE ascii = 0;
    UBYTE scancode_only = scancode & ~KEY_RELEASED;  /* get rid of release bits */


#if DBG_KBD
    kprintf("================\n ");
    kprintf("Key-scancode: 0x%02x\n", scancode);

    kprintf("Key-shift bits: 0x%02x\n", shifty);
#endif

    /* keyboard warm/cold reset */
    if ((scancode == 0x53)  /* Del key and shifty is Alt+Ctrl but not LShift */
        && ((shifty & (MODE_ALT|MODE_CTRL|MODE_LSHIFT)) == (MODE_ALT|MODE_CTRL))) {
        if (shifty & MODE_RSHIFT) {
            /* Ctrl+Alt+RShift+Del means cold reset */
            cold_reset();
        }
        else {
            /* Ctrl+Alt+Del means warm reset */
            warm_reset();
        }
    }

    /* the additional mouse buttons use a separate vector */
    if (   scancode_only == 0x37  /* Mouse button 3 */
        || scancode_only == 0x5e  /* Mouse button 4 */
        || scancode_only == 0x5f  /* Mouse button 5 */
        || scancode_only == 0x59  /* Wheel up */
        || scancode_only == 0x5a  /* Wheel down */
        || scancode_only == 0x5c  /* Wheel left */
        || scancode_only == 0x5d) /* Wheel right */
    {
        mousexvec(scancode);
        return;
    }

    if (scancode & KEY_RELEASED) {
        /* stop key repeat */
        kb_ticks = 0;

        scancode = scancode_only;
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
            if(kb_altnum >= 0) {
                ascii = kb_altnum & 0xFF;
                kb_altnum = -1;
                goto push_value;
            }
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
        if (conterm & 1) {
            keyclick(KEY_CAPS);
        }
        shifty ^= MODE_CAPS;    /* toggle bit */
        return;
    }


    if (shifty & MODE_ALT) {
        const UBYTE *a;

        /* ALT-keypad means that char number */
        if (scancode >= 103 && scancode <= 112) {
            if (kb_altnum < 0) {
                kb_altnum = 0;
            }
            kb_altnum *= 10;
            kb_altnum += "\7\10\11\4\5\6\1\2\3\0" [scancode - 103];
            return;
        }

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
    } else if (shifty & MODE_SHIFT) {
        /* Fonction keys F1 to F10 */
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

    if (shifty & MODE_CTRL) {
        /* More complicated in TOS, but is it really necessary ? */
        ascii &= 0x1F;
    } else if(kb_dead >= 0) {
        const UBYTE *a = current_keytbl.dead[kb_dead];
        while (*a && *a != ascii) {
            a += 2;
        }
        if (*a++) {
            ascii = *a;
        }
        kb_dead = -1;
    } else if(ascii <= DEADMAX && ascii >= DEADMIN) {
        kb_dead = ascii - DEADMIN;
        return;
    }

  push_value:
    if (conterm & 1)
        keyclick(scancode);
    value = ((LONG) scancode) << 16;
    value += ascii;
    if (conterm & 0x8) {
        value += ((LONG) shifty) << 24;
    }
#if DBG_KBD
    kprintf("KBD iorec: Pushing value 0x%08lx\n", value);
#endif
    push_ikbdiorec(value);

    /* set initial delay for key repeat */
    kb_last_key = value;
    kb_last_ikbdsys = kbdvecs.ikbdsys;
    kb_ticks = kb_initial;
}


/*=== ikbd acia stuff ==================================================*/

/* can we send a byte to the ikbd ? */
LONG bcostat4(void)
{
#if CONF_WITH_IKBD_ACIA
    if (ikbd_acia.ctrl & ACIA_TDRE) {
        return -1;              /* OK */
    } else {
        /* Data register not empty */
        return 0;               /* not OK */
    }
#else
    return -1; /* OK (but output will be ignored) */
#endif
}

/* send a byte to the IKBD */
LONG bconout4(WORD dev, WORD c)
{
    ikbd_writeb((UBYTE)c);
    return 1L;
}

/* cnt = number of bytes to send less one
 * Workaround: New Beat's 4kB Falcon demo "Blue" calls Ikbdws() with a
 * ridiculously small stack (sp == 0x22), so keep stack usage as small as
 * possible here.
 */
void ikbdws(WORD cnt, PTR ptr)
{
    UBYTE *p = (UBYTE *) ptr;
    while (cnt-- >= 0)
        ikbd_writeb(*p++);
}

/* send a byte to the IKBD - for general use */
void ikbd_writeb(UBYTE b)
{
#if DBG_KBD
    kprintf("ikbd_writeb(0x%02x)\n", (UBYTE)b);
#endif

    while (!bcostat4());
#if CONF_WITH_IKBD_ACIA
    ikbd_acia.data = b;
#endif
}

/* send a word to the IKBD as two bytes - for general use */
void ikbd_writew(WORD w)
{
    ikbd_writeb(w>>8);
    ikbd_writeb(w&0xff);
}

/* Read a byte from the IKBD.
 * This must not be used when interrupts are enabled.
 */
static UBYTE ikbd_readb(void)
{
#if CONF_WITH_IKBD_ACIA
    unsigned int i;

    /* We have to use a timeout to avoid waiting forever
     * if the keyboard is unplugged. The standard value is 300 ms.
     */
    for (i = 0; i < 300; i++)
    {
        if (ikbd_acia.ctrl & ACIA_RDRF)
            return ikbd_acia.data;

        delay_loop(loopcount_1_msec);
    }

    return 0; /* bogus value when timeout */
#else
    return 0; /* bogus value */
#endif
}

/*
 *      FUNCTION:  This routine resets the keyboard,
 *        configures the MFP so we can get interrupts
 */

void kbd_init(void)
{
    UBYTE ikbd_version;

#if CONF_WITH_IKBD_ACIA
    /* initialize ikbd ACIA */
    ikbd_acia.ctrl = ACIA_RESET;        /* master reset */

    ikbd_acia.ctrl = ACIA_RIE | /* enable interrupts */
        ACIA_RLTID |            /* RTS low, TxINT disabled */
        ACIA_DIV64 |            /* clock/64 */
        ACIA_D8N1S;             /* 8 bit, 1 stop, no parity */
#endif /* CONF_WITH_IKBD_ACIA */

    /* initialize the IKBD */
    ikbd_writeb(0x80);            /* Reset */
    ikbd_writeb(0x01);

    /* The IKBD answers to Reset command with a version byte.
     * It is *mandatory* to wait for that byte before sending further commands,
     * otherwise the IKBD will enter an undefined state. Concretely, it will
     * stop working.
     * This is particularly important on real hardware, and with Hatari which
     * has very accurate IKBD emulation.
     */
    ikbd_version = ikbd_readb(); /* Usually 0xf1, or 0xf0 for antique STs */
#if DBG_KBD
    kprintf("ikbd_version = 0x%02x\n", ikbd_version);
#endif
    UNUSED(ikbd_version);

    ikbd_writeb(0x1A);            /* disable joystick */
    ikbd_writeb(0x12);            /* disable mouse */

    /* initialize the key repeat stuff */
    kb_ticks = 0;
    kb_initial = 25;
    kb_repeat = 5;

    kb_dead = -1;      /* not in a dead key sequence */
    kb_altnum = -1;    /* not in an alt-numeric sequence */

    conterm = 7;       /* keyclick and autorepeat on by default */
    conterm |= 0x8;    /* add Kbshift state to Bconin value */

    bioskeys();
}
