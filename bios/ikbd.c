/*
 * ikbd.c - Intelligent keyboard routines
 *
 * Copyright (c) 2001-2015 The EmuTOS development team
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
 * - alt-help screen hardcopy
 * - KEYTBL.TBL config with _AKP cookie (tos 5.00 and later)
 * - CLRHOME and INSERT in kbshift.
 */

/* #define ENABLE_KDEBUG */

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
#include "coldfire.h"


/* forward declarations */
static ULONG combine_scancode_ascii(UBYTE scancode,WORD ascii);
static WORD convert_scancode(UBYTE *scancodeptr);

/* scancode definitions */
#define KEY_RELEASED 0x80       /* This bit set, when key-release scancode */

#define KEY_LSHIFT  0x2a        /* modifiers */
#define KEY_RSHIFT  0x36
#define KEY_CTRL    0x1d
#define KEY_ALT     0x38
#define KEY_CAPS    0x3a

#define KEY_F1      0x3b        /* function keys F1 - F10 */
#define KEY_F10     0x44

#define KEYPAD_START 0x67       /* numeric keypad: 7 8 9 4 5 6 1 2 3 0 */
#define KEYPAD_END  0x70

/*
 * support for mouse emulation:
 *  alt-insert, alt-home => mouse buttons
 *  alt-arrowkeys: mouse movement
 */
#define KEY_INSERT  0X52
#define KEY_HOME    0X47
#define KEY_UPARROW 0x48
#define KEY_LTARROW 0x4b
#define KEY_RTARROW 0x4d
#define KEY_DNARROW 0x50

#define MOUSE_REL_POS_REPORT    0xf8    /* values for mouse_packet[0] */
#define RIGHT_BUTTON_DOWN       0x01    /* these values are OR'ed in */
#define LEFT_BUTTON_DOWN        0x02

/*
 * the following stores the packet that is passed to mousevec();
 * a non-zero value in mouse_packet[0] indicates we are currently
 * in mouse emulation mode.
 */
BYTE mouse_packet[3];                   /* passed to mousevec() */

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
    /* ask country.c for the default key table of the current country */
    current_keytbl = *get_keytbl();
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
    ULONG value;

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
    value = *(ULONG_ALIAS *) (ikbdiorec.buf + ikbdiorec.head);

    /* restore interrupts */
    set_sr(old_sr);
    return value;
}

static void push_ikbdiorec(ULONG value)
{
    KDEBUG(("KBD iorec: Pushing value 0x%08lx\n", value));

    ikbdiorec.tail += 4;
    if (ikbdiorec.tail >= ikbdiorec.size) {
        ikbdiorec.tail = 0;
    }
    if (ikbdiorec.tail == ikbdiorec.size) {
        /* iorec full */
        return;
    }
    *(ULONG_ALIAS *) (ikbdiorec.buf + ikbdiorec.tail) = value;
}

#if CONF_SERIAL_CONSOLE

/* Find a scancode in a keyboard table */
static UBYTE scancode_from_ascii(UBYTE ascii, const UBYTE *table)
{
    int i;

    for (i = 0; i < 128; i++)
    {
        if (table[i] == ascii)
            return i;
    }

    return 0;
}

/* Emulate a key press from an ASCII character */
void push_ascii_ikbdiorec(UBYTE ascii)
{
    UBYTE scancode = 0;
    UBYTE mode = 0;
    ULONG value;

    /* We have to look in the keyboard tables to find a matching scancode.
     * First, look in Normal table. */
    scancode = scancode_from_ascii(ascii, current_keytbl.norm);

    /* If not found, look in Shift table. */
    if (scancode == 0)
    {
        scancode = scancode_from_ascii(ascii, current_keytbl.shft);
        if (scancode != 0)
            mode = MODE_LSHIFT;
    }

    /* Then check for Control codes from ^A to ^Z. */
    if (scancode == 0 && (ascii >= ('a'-'`') && ascii <= ('z'-'`')))
    {
        scancode = scancode_from_ascii('`'+ascii, current_keytbl.norm);
        if (scancode != 0)
            mode = MODE_CTRL;
    }

    value = ((ULONG)scancode << 16) | ascii;

    if (conterm & 0x8)
        value |= (ULONG)mode << 24;

    push_ikbdiorec(value);
}

#endif /* CONF_SERIAL_CONSOLE */

/*
 * emulated mouse support (alt-arrowkey support)
 */

/*
 * is the key related to mouse emulation?
 */
static BOOL is_mouse_key(WORD key)
{
    switch(key) {
    case KEY_INSERT:
    case KEY_HOME:
    case KEY_UPARROW:
    case KEY_DNARROW:
    case KEY_LTARROW:
    case KEY_RTARROW:
        return TRUE;
    }

    return FALSE;
}

/*
 * initialise mouse packet
 */
static void init_mouse_packet(BYTE *packet)
{
    packet[0] = MOUSE_REL_POS_REPORT;
    packet[1] = packet[2] = 0;
}

/*
 * check if we should switch into or out of mouse emulation mode
 * if so, send the relevant mouse packet
 *
 * returns TRUE iff we're in mouse mode on exit
 */
static BOOL handle_mouse_mode(WORD newkey)
{
    BYTE distance;

    /*
     * if we shouldn't be in emulation mode, but we are, send an
     * appropriate mouse packet and exit
     */
    if (!(shifty&MODE_ALT) || !is_mouse_key(newkey)) {
        if (mouse_packet[0]) {  /* emulating, need to clean up */
            init_mouse_packet(mouse_packet);
            call_mousevec(mouse_packet);
            mouse_packet[0] = '\0';
            KDEBUG(("Exiting mouse emulation mode\n"));
        }
        return FALSE;
    }

    /*
     * we should be, so ensure that mouse_packet is valid
     */
    if (!mouse_packet[0]) {
        KDEBUG(("Entering mouse emulation mode\n"));
        init_mouse_packet(mouse_packet);
    }

    distance = (shifty&MODE_SHIFT) ? 1 : 8;

    switch(newkey) {
    case KEY_INSERT:
        mouse_packet[0] |= LEFT_BUTTON_DOWN;
        break;
    case KEY_HOME:
        mouse_packet[0] |= RIGHT_BUTTON_DOWN;
        break;
    case KEY_UPARROW:
        distance = -distance;
        /* drop through */
    case KEY_DNARROW:
        mouse_packet[1] = 0;        /* Atari TOS only allows one direction at a time */
        mouse_packet[2] = distance;
        break;
    case KEY_LTARROW:
        distance = -distance;
        /* drop through */
    case KEY_RTARROW:
        mouse_packet[1] = distance;
        mouse_packet[2] = 0;        /* Atari TOS only allows one direction at a time */
        break;
    }

    KDEBUG(("Sending mouse packet %02x%02x%02x\n",(UBYTE)mouse_packet[0],(UBYTE)mouse_packet[1],(UBYTE)mouse_packet[2]));
    call_mousevec(mouse_packet);

    return TRUE;
}

/*=== kbrate (xbios) =====================================================*/

/*
 * key repeat is handled as follows:
 * As a key is hit and enters the buffer, it is recorded as kb_last_key;
 * the scancode and state of 'shifty' are also saved in kb_last_scancode
 * and kb_last_shifty.  At the same time a downward counter kb_ticks is
 * set to the first delay, kb_initial.
 *
 * Every fourth timer C interrupt (50 Hz ticks), kb_tick is decremented if
 * not zero (zero means no more key repeat for this key).  If kb_tick
 * reaches zero, the value of 'shifty' is compared to the saved value.  If
 * they are the same, kb_last_key is re-emitted; otherwise:
 *  . the scancode is re-evaluated using the new value of 'shifty'
 *  . a (probably new) key is emitted
 *  . kb_last_key and kb_last_shifty are updated
 * In either case, kb_tick is now set to the second delay kb_repeat.
 *
 * Any release of a *non-modifier* key stops the repeat handling by
 * setting kb_tick back to zero.  It's also set back to zero by the release
 * of the alt key during alt-numpad processing.  Allowing the repeat to
 * continue when e.g. the state of the shift key changes provides the
 * same functionality as Atari TOS.
 *
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
static UBYTE kb_last_shifty;
static UBYTE kb_last_scancode;
static ULONG kb_last_key;
static PFVOID kb_last_ikbdsys; /* ikbdsys when kb_last_key was set */

WORD kbrate(WORD initial, WORD repeat)
{
    WORD ret = (kb_initial << 8) | (kb_repeat & 0xFF);

    if (initial >= 0)
        kb_initial = initial & 0x00ff;
    if (repeat >= 0)
        kb_repeat = repeat & 0x00ff;

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
        keyclick(kb_last_scancode);

    /*
     * changing the modifier keys no longer stops key repeat, so when
     * they change, we must do the scancode conversion again
     */
    if (shifty != kb_last_shifty) {
        UBYTE scancode;
        WORD ascii;

        scancode = kb_last_scancode;            /* make a copy so we don't change  */
        ascii = convert_scancode(&scancode);    /* kb_last_scancode inadvertently! */
        kb_last_key = combine_scancode_ascii(kb_last_scancode,ascii);
        kb_last_shifty = shifty;
    }

    /* Simulate a key press or a mouse action */
    if (mouse_packet[0]) {
        KDEBUG(("Repeating mouse packet %02x%02x%02x\n",(UBYTE)mouse_packet[0],(UBYTE)mouse_packet[1],(UBYTE)mouse_packet[2]));
        call_mousevec(mouse_packet);
    } else push_ikbdiorec(kb_last_key);

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

/* a boolean value, non-zero iff the scancode lookup table has been switched */
/* (this can only happen for keyboards with the DUAL_KEYBOARD feature)       */
static UBYTE kb_switched;

/*
 * convert a scancode to an ascii character
 *
 * for shifted function keys, we also update the scancode
 */
static WORD convert_scancode(UBYTE *scancodeptr)
{
    UBYTE scancode = *scancodeptr;
    WORD ascii = 0;
    const UBYTE *a;

    /*
     * do special processing for alt-arrow, alt-keypad, shift-function
     * keys, then return
     */
    if (shifty & MODE_ALT) {
        /*
         * the alt key is down: check if the user has pressed an arrow key
         * and, if so, send the appropriate mouse packet
         */
        if (handle_mouse_mode(scancode))    /* we sent a packet, */
            return 0;                       /* so we're done     */

        /* ALT-keypad means that char number */
        if ((scancode >= KEYPAD_START) && (scancode <= KEYPAD_END)) {
            if (kb_altnum < 0)
                kb_altnum = 0;
            else kb_altnum *= 10;
            kb_altnum += "\7\10\11\4\5\6\1\2\3\0" [scancode-KEYPAD_START];
            return -1;
        }
    } else if (shifty & MODE_SHIFT) {
        /* function keys F1 to F10 => F11 to F20 */
        if ((scancode >= KEY_F1) && (scancode <= KEY_F10)) {
            *scancodeptr += 0x19;
            return 0;
        }
    }

    /*
     * which keyboard table to use, and how to handle it, depends on
     * the presence or absence of the DUAL_KEYBOARD feature
     *
     * Alt-X handling:
     * if the DUAL_KEYBOARD feature is present, Alt-X always generates
     * an ascii value of zero; otherwise, Alt-X performs an ascii
     * lookup using the 'alternate' keyboard tables, which are set up
     * as (scancode,ascii) pairs.
     *
     * All other key handling:
     * if 'kb_switched' is set (only set if a keyboard table has the
     * DUAL_KEYBOARD feature AND the tables have been switched via the
     * hotswitch key), the alternate keyboard tables are used; otherwise
     * the main keyboard tables are used (these are all 128-byte direct
     * scancode lookup tables).
     */
    if (shifty & MODE_ALT) {
        if ((current_keytbl.features & DUAL_KEYBOARD) == 0) {
            if (shifty & MODE_SHIFT) {
                a = current_keytbl.altshft;
            } else if (shifty & MODE_CAPS) {
                a = current_keytbl.altcaps;
            } else {
                a = current_keytbl.altnorm;
            }
            while (*a && (*a != scancode)) {
                a += 2;
            }
            if (*a++) {
                ascii = *a;
            }
        }
    } else {
        if (shifty & MODE_SHIFT) {
            a = kb_switched ? current_keytbl.altshft : current_keytbl.shft;
        } else if (shifty & MODE_CAPS) {
            a = kb_switched ? current_keytbl.altcaps : current_keytbl.caps;
        } else {
            a = kb_switched ? current_keytbl.altnorm : current_keytbl.norm;
        }
        ascii = a[scancode];
    }

    if (shifty & MODE_CTRL) {
        /* More complicated in TOS, but is it really necessary ? */
        ascii &= 0x1F;
    } else if (kb_dead >= 0) {
        a = current_keytbl.dead[kb_dead];
        while (*a && *a != ascii) {
            a += 2;
        }
        if (*a++) {
            ascii = *a;
        }
        kb_dead = -1;
    } else if ((ascii >= DEADMIN) && (ascii <= DEADMAX)) {
        kb_dead = ascii - DEADMIN;
        return -1;
    }

    return ascii;
}

/*
 * combine the scancode and the ascii equivalent in a ULONG
 */
static ULONG combine_scancode_ascii(UBYTE scancode,WORD ascii)
{
    ULONG value;

    value = ((ULONG) scancode) << 16;
    value += ascii;
    if (conterm & 0x8)
        value += ((ULONG) shifty) << 24;

    return value;
}

/*
 * kbd_int : called by the interrupt routine for key events.
 */

void kbd_int(UBYTE scancode)
{
    ULONG value;                /* the value to push into iorec */
    WORD ascii = 0;
    UBYTE scancode_only = scancode & ~KEY_RELEASED;  /* get rid of release bits */
    BOOL modifier;

    KDEBUG(("================\n"));
    KDEBUG(("Key-scancode: 0x%02x, key-shift bits: 0x%02x\n", scancode, shifty));

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
            if (kb_altnum >= 0) {
                ascii = kb_altnum & 0xFF;
                kb_altnum = -1;
                kb_ticks = 0;           /* stop key repeat */
                goto push_value;
            }
            break;
        default:                    /* non-modifier keys: */
            kb_ticks = 0;               /*  stop key repeat */
        }
        handle_mouse_mode(kb_last_key); /* exit mouse mode if appropriate */
        return;
    }

    /*
     * a key has been pressed
     */
    modifier = TRUE;
    switch (scancode) {
    case KEY_RSHIFT:
        shifty |= MODE_RSHIFT;  /* set bit */
        break;
    case KEY_LSHIFT:
        shifty |= MODE_LSHIFT;  /* set bit */
        break;
    case KEY_CTRL:
        shifty |= MODE_CTRL;    /* set bit */
        break;
    case KEY_ALT:
        shifty |= MODE_ALT;     /* set bit */
        break;
    case KEY_CAPS:
        if (conterm & 1) {
            keyclick(KEY_CAPS);
        }
        shifty ^= MODE_CAPS;    /* toggle bit */
        break;
    default:
        modifier = FALSE;
        break;
    }
    if (modifier) {     /* the user has pressed a modifier key */
        /*
         * first, check for keyboard hot switch and if so, do the switch
         */
        if (current_keytbl.features&DUAL_KEYBOARD)
            if ((shifty&MODE_SCA) == HOTSWITCH_MODE)
                kb_switched ^= 0x01;
        /*
         * check if an arrow key was already down and, if so, send the appropriate mouse packet
         */
        handle_mouse_mode(kb_last_key);
        return;
    }

    /*
     * a non-modifier key has been pressed
     */
    ascii = convert_scancode(&scancode);
    if (ascii < 0)      /* dead key (including alt-keypad) processing */
        return;

    kb_ticks = kb_initial;

  push_value:
    if (conterm & 1)
        keyclick(scancode);

    value = combine_scancode_ascii(scancode,ascii);

    /*
     * if we're not sending mouse packets, send a real key
     */
    if (!mouse_packet[0])
        push_ikbdiorec(value);

    kb_last_scancode = scancode;    /* save for do_key_repeat() */
    kb_last_shifty = shifty;
    kb_last_key = value;
    kb_last_ikbdsys = kbdvecs.ikbdsys;
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
    KDEBUG(("ikbd_writeb(0x%02x)\n", (UBYTE)b));

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

#if CONF_SERIAL_CONSOLE
# ifdef __mcoldfire__
    coldfire_rs232_enable_interrupt();
# else
    /* FIXME: Enable interrupts on other hardware. */
# endif
#endif /* CONF_SERIAL_CONSOLE */

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
    KDEBUG(("ikbd_version = 0x%02x\n", ikbd_version));
    UNUSED(ikbd_version);

    /* initialize the key repeat stuff */
    kb_ticks = 0;
    kb_initial = 25;
    kb_repeat = 5;

    kb_dead = -1;      /* not in a dead key sequence */
    kb_altnum = -1;    /* not in an alt-numeric sequence */
    kb_switched = 0;   /* not switched initially */

    conterm = 7;       /* keyclick and autorepeat on by default */
    conterm |= 0x8;    /* add Kbshift state to Bconin value */

    mouse_packet[0] = 0;    /* not doing mouse emulation */

    bioskeys();
}
