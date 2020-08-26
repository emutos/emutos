/*
 * ikbd.c - Intelligent keyboard routines
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
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
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "country.h"
#include "acia.h"
#include "tosvars.h"
#include "biosext.h"
#include "lineavars.h"
#include "iorec.h"
#include "asm.h"
#include "ikbd.h"
#include "sound.h"              /* for keyclick */
#include "delay.h"
#include "bios.h"
#include "coldfire.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif


/* forward declarations */
static WORD convert_scancode(UBYTE *scancodeptr);

/* scancode definitions */
#define KEY_RELEASED 0x80       /* This bit set, when key-release scancode */

#define KEY_LSHIFT  0x2a        /* modifiers */
#define KEY_RSHIFT  0x36
#define KEY_CTRL    0x1d
#define KEY_ALT     0x38
#define KEY_CAPS    0x3a

#define KEY_ESCAPE  0x01        /* invariant keys, unaffected by modifiers */
#define KEY_BACKSPACE 0x0e
#define KEY_TAB     0x0f
#define KEY_UNDO    0x61

#define KEY_RETURN  0x1c        /* semi-invariant, ctrl changes ascii to newline */
#define KEY_ENTER   0x72

#define KEY_F1      0x3b        /* function keys F1 - F10 */
#define KEY_F10     0x44

#define KEY_CTRL_HOME 0x77      /* scancode values set when ctrl modifies scancode */
#define KEY_CTRL_LTARROW 0x73
#define KEY_CTRL_RTARROW 0x74

#define TOPROW_START 0x02       /* numeric keys, minus, equals */
#define TOPROW_END  0x0d

#define KEYPAD_START 0x67       /* numeric keypad: 7 8 9 4 5 6 1 2 3 0 */
#define KEYPAD_END  0x70

/* standard ascii */
#define LF          0x0a

/*
 * support for mouse emulation:
 *  alt-insert, alt-home => mouse buttons (standard, but Amiga differs, see below)
 *  alt-arrowkeys: mouse movement
 */
#define KEY_HELP    0x62
#define KEY_DELETE  0x53
#define KEY_INSERT  0X52
#define KEY_HOME    0X47
#define KEY_UPARROW 0x48
#define KEY_LTARROW 0x4b
#define KEY_RTARROW 0x4d
#define KEY_DNARROW 0x50
#ifdef MACHINE_AMIGA
#define KEY_EMULATE_LEFT_BUTTON     KEY_DELETE
#define KEY_EMULATE_RIGHT_BUTTON    KEY_HELP
#else
#define KEY_EMULATE_LEFT_BUTTON     KEY_INSERT
#define KEY_EMULATE_RIGHT_BUTTON    KEY_HOME
#endif

#define MOUSE_REL_POS_REPORT    0xf8    /* values for mouse_packet[0] */
#define RIGHT_BUTTON_DOWN       0x01    /* these values are OR'ed in */
#define LEFT_BUTTON_DOWN        0x02

/*
 * the following stores the packet that is passed to mousevec();
 * a non-zero value in mouse_packet[0] indicates we are currently
 * in mouse emulation mode.
 */
SBYTE mouse_packet[3];                  /* passed to mousevec() */

/*
 * the following is a count of the number of arrow keys currently down;
 * it is used in mouse emulation mode.
 */
static WORD kb_arrowkeys;

/*=== Keymaps handling (xbios) =======================================*/

static struct keytbl current_keytbl;

LONG keytbl(const UBYTE* norm, const UBYTE* shft, const UBYTE* caps)
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
    WORD oldshifty = shifty;

    if (flag != -1)
        shifty = flag;

    return oldshifty;
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

    if (!(conterm & 8))         /* shift status not wanted? */
        value &= 0x00ffffffL;   /* true, so clean it out */

    return value;
}

static void push_ikbdiorec(ULONG value)
{
    short tail;

    KDEBUG(("KBD iorec: Pushing value 0x%08lx\n", value));

    tail = ikbdiorec.tail + 4;
    if (tail >= ikbdiorec.size) {
        tail = 0;
    }
    if (tail == ikbdiorec.head) {
        /* iorec full */
        return;
    }
    *(ULONG_ALIAS *) (ikbdiorec.buf + tail) = value;
    ikbdiorec.tail = tail;
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

    value = MAKE_ULONG(scancode, ascii);
    value |= (ULONG)mode << 24;

    push_ikbdiorec(value);
}

#endif /* CONF_SERIAL_CONSOLE */

/*
 * emulated mouse support (alt-arrowkey support)
 */

/*
 * check if we should switch into or out of mouse emulation mode
 * if so, send the relevant mouse packet
 *
 * returns TRUE iff we're in mouse mode on exit
 */
static BOOL handle_mouse_mode(WORD newkey)
{
    SBYTE distance;
    BOOL button = FALSE;

    /*
     * check if we should be in emulation mode or not
     */
    if ((shifty&MODE_ALT) && (kb_arrowkeys > 0))
    {
        /* we should be, so ensure that mouse_packet is valid */
        if (!mouse_packet[0])
        {
            KDEBUG(("Entering mouse emulation mode\n"));
            mouse_packet[0] = MOUSE_REL_POS_REPORT;
        }
    } else {
        if (mouse_packet[0])    /* emulating, need to clean up */
        {
            /* we send a packet with all buttons up & no movement */
            mouse_packet[0] = MOUSE_REL_POS_REPORT;
            mouse_packet[1] = mouse_packet[2] = 0;
            KDEBUG(("Sending mouse packet %02x%02x%02x\n",
                    (UBYTE)mouse_packet[0],(UBYTE)mouse_packet[1],(UBYTE)mouse_packet[2]));
            call_mousevec(mouse_packet);
            KDEBUG(("Exiting mouse emulation mode\n"));
            mouse_packet[0] = 0;
        }
        return FALSE;
    }

    /*
     * set movement distance according to the Shift key
     */
    if (shifty&MODE_SHIFT)
        distance = 1;
    else distance = 8;

    switch(newkey) {
    case KEY_EMULATE_LEFT_BUTTON:
        mouse_packet[0] |= LEFT_BUTTON_DOWN;
        button = TRUE;
        break;
    case KEY_EMULATE_LEFT_BUTTON | KEY_RELEASED:
        mouse_packet[0] &= ~LEFT_BUTTON_DOWN;
        break;
    case KEY_EMULATE_RIGHT_BUTTON:
        mouse_packet[0] |= RIGHT_BUTTON_DOWN;
        button = TRUE;
        break;
    case KEY_EMULATE_RIGHT_BUTTON | KEY_RELEASED:
        mouse_packet[0] &= ~RIGHT_BUTTON_DOWN;
        break;
    case KEY_UPARROW:
        distance = -distance;
        FALLTHROUGH;
    case KEY_DNARROW:
        mouse_packet[1] = 0;        /* Atari TOS only allows one direction at a time */
        mouse_packet[2] = distance;
        break;
    case KEY_LTARROW:
        distance = -distance;
        FALLTHROUGH;
    case KEY_RTARROW:
        mouse_packet[1] = distance;
        mouse_packet[2] = 0;        /* Atari TOS only allows one direction at a time */
        break;
    default:        /* user pressed a modifier: update distances */
        if (mouse_packet[1] < 0)
            mouse_packet[1] = -distance;
        else if (mouse_packet[1])
            mouse_packet[1] = distance;
        if (mouse_packet[2] < 0)
            mouse_packet[2] = -distance;
        else if (mouse_packet[2])
            mouse_packet[2] = distance;
    }

    /*
     * if the packet is for an emulated mouse button press,
     * reset the x,y distance variables
     */
    if (button)
        mouse_packet[1] = mouse_packet[2] = 0;

    /*
     * for compatibility with Atari TOS, the mouse does not move while Control
     * is pressed, although the keyboard remains in mouse emulation mode
     */
    if (!(shifty&MODE_CTRL))
    {
        KDEBUG(("Sending mouse packet %02x%02x%02x\n",
                (UBYTE)mouse_packet[0],(UBYTE)mouse_packet[1],(UBYTE)mouse_packet[2]));
        call_mousevec(mouse_packet);
    }

    return TRUE;
}

/*=== kbrate (xbios) =====================================================*/

/*
 * key repeat is handled as follows:
 * As a key is hit and enters the buffer, the scancode, ascii equivalent,
 * and the state of 'shifty' are recorded in kb_last.  At the same time a
 * downward counter kb_ticks is set to the first delay, kb_initial.
 *
 * Every fourth timer C interrupt (50 Hz ticks), kb_tick is decremented if
 * not zero (zero means no more key repeat for this key).  If kb_tick
 * reaches zero, the value of 'shifty' is compared to the saved value.  If
 * they are the same, the value in kb_last is re-emitted; otherwise:
 *  . the scancode is re-evaluated using the new value of 'shifty'
 *  . a (probably new) key is emitted
 *  . kb_last is updated
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
static union {
    ULONG key;                  /* combined value */
    struct {
        UBYTE shifty;           /* state of 'shifty' */
        UBYTE scancode;         /* possibly-converted scancode */
        UWORD ascii;            /* derived ascii value */
    } k;
} kb_last;
static UBYTE kb_last_actual;    /* actual last scancode */
static PFVOID kb_last_ikbdsys;  /* ikbdsys when kb_last was set */

WORD kbrate(WORD initial, WORD repeat)
{
    WORD ret = MAKE_UWORD(kb_initial, kb_repeat);

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
        keyclick(kb_last.k.scancode);

    /*
     * changing the modifier keys no longer stops key repeat, so when
     * they change, we must do the scancode conversion again
     */
    if (shifty != kb_last.k.shifty) {
        /* get actual last scancode because convert_scancode() can change it */
        kb_last.k.scancode = kb_last_actual;
        kb_last.k.ascii = convert_scancode(&kb_last.k.scancode);
        kb_last.k.shifty = shifty;
        kb_last_ikbdsys = kbdvecs.ikbdsys;
    }

    /*
     * Simulate a key press or a mouse action
     *
     * for compatibility with Atari TOS, the mouse does not move while Control
     * is pressed, although the keyboard remains in mouse emulation mode
     */
    if (mouse_packet[0])
    {
        if (!(shifty&MODE_CTRL))
        {
            KDEBUG(("Repeating mouse packet %02x%02x%02x\n",
                    (UBYTE)mouse_packet[0],(UBYTE)mouse_packet[1],(UBYTE)mouse_packet[2]));
            call_mousevec(mouse_packet);
        }
    } else push_ikbdiorec(kb_last.key);

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
 * for certain keys, we also update the scancode:
 *  shifted function keys
 *  some keys when modified by ctrl
 */
static WORD convert_scancode(UBYTE *scancodeptr)
{
    UBYTE scancode = *scancodeptr;
    WORD ascii = 0;
    const UBYTE *a;

    /*
     * do special processing for some keys that are in the same position
     * on all keyboards:
     * (a) invariant scancodes (unaffected by modifier keys)
     * (b) return/enter (unaffected except that ctrl causes LF to be returned)
     */
    switch(scancode) {
    case KEY_RETURN:
    case KEY_ENTER:
        if (shifty & MODE_CTRL)
            return LF;
        FALLTHROUGH;
    case KEY_ESCAPE:
    case KEY_BACKSPACE:
    case KEY_TAB:
    case KEY_UNDO:
        return current_keytbl.norm[scancode];
    }

    /*
     * do special processing for alt-arrow, alt-help, alt-keypad,
     * alt-number & shift-function keys, then return
     */
    if (shifty & MODE_ALT) {
        /*
         * the alt key is down: check if the user has pressed an arrow key
         * and, if so, send the appropriate mouse packet
         */
        if (handle_mouse_mode(scancode))    /* we sent a packet, */
            return 0;                       /* so we're done     */

        if (scancode == KEY_HELP) {
            dumpflg++;      /* tell VBL to call scrdmp() function */
            return 0;
        }

        /* ALT-keypad means that char number */
        if ((scancode >= KEYPAD_START) && (scancode <= KEYPAD_END)) {
            if (kb_altnum < 0)
                kb_altnum = 0;
            else kb_altnum *= 10;
            kb_altnum += "\7\10\11\4\5\6\1\2\3\0" [scancode-KEYPAD_START];
            return -1;
        }
        if ((scancode >= TOPROW_START) && (scancode <= TOPROW_END)) {
            *scancodeptr += 0x76;
            return 0;
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
     * we obtain a default ascii value by using the standard keyboard
     * tables.  if the DUAL_KEYBOARD feature is not present, we may then
     * override that value via a lookup using the 'alternate' keyboard
     * tables, which are set up as (scancode,ascii) pairs.  finally we
     * zero out alphabetic ascii values.
     *
     * All other key handling:
     * if 'kb_switched' is set (only set if a keyboard table has the
     * DUAL_KEYBOARD feature AND the tables have been switched via the
     * hotswitch key), the alternate keyboard tables are used; otherwise
     * the main keyboard tables are used (these are all 128-byte direct
     * scancode lookup tables).
     */
    if (shifty & MODE_ALT) {
        if (shifty & MODE_SHIFT) {
            a = current_keytbl.shft;
        } else if (shifty & MODE_CAPS) {
            a = current_keytbl.caps;
        } else {
            a = current_keytbl.norm;
        }
        ascii = a[scancode];
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
        if (((ascii >= 'A') && (ascii <= 'Z'))
         || ((ascii >= 'a') && (ascii <= 'z')))
            ascii = 0;
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

    /*
     * Ctrl key handling is mostly straightforward, but there are a few warts
     */
    if (shifty & MODE_CTRL) {
        switch(ascii) {
        case '-':
            ascii = 0x1f;
            break;
        case '2':
            ascii = 0x00;
            break;
        case '6':
            ascii = 0x1e;
        }
        switch(scancode) {
        case KEY_HOME:
            *scancodeptr = KEY_CTRL_HOME;
            break;
        case KEY_LTARROW:
            *scancodeptr = KEY_CTRL_LTARROW;
            break;
        case KEY_RTARROW:
            *scancodeptr = KEY_CTRL_RTARROW;
            break;
        default:
            ascii &= 0x1F;
        }
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
 * kbd_int : called by the interrupt routine for key events.
 */

void kbd_int(UBYTE scancode)
{
    WORD ascii = 0;
    UBYTE scancode_only = scancode & ~KEY_RELEASED;  /* get rid of release bits */
    BOOL modifier, arrowkey = FALSE;

    KDEBUG(("================\n"));
    KDEBUG(("Key-scancode: 0x%02x, key-shift bits: 0x%02x\n", scancode, shifty));

    /* keyboard warm/cold reset */
    if ((scancode == KEY_DELETE)
        && ((shifty & (MODE_ALT|MODE_CTRL|MODE_LSHIFT)) == (MODE_ALT|MODE_CTRL))) {
        /* Del key and shifty is Alt+Ctrl but not LShift */
        if (shifty & MODE_RSHIFT) {
            /* Ctrl+Alt+RShift+Del means cold reset */
            cold_reset();
        }
        else {
            /* Ctrl+Alt+Del means warm reset */
            warm_reset();
        }
    }

#if CONF_WITH_EXTENDED_MOUSE
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
#endif

    /*
     * check for possible mouse emulation key
     */
    switch(scancode_only) {
    case KEY_UPARROW:
    case KEY_DNARROW:
    case KEY_LTARROW:
    case KEY_RTARROW:
    case KEY_EMULATE_LEFT_BUTTON:
    case KEY_EMULATE_RIGHT_BUTTON:
        arrowkey = TRUE;
        break;
    }

    if (scancode & KEY_RELEASED) {
        switch (scancode_only) {
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
            if (mouse_packet[0])
                kb_ticks = 0;           /* stop key repeat */
            if (kb_altnum >= 0) {
                ascii = LOBYTE(kb_altnum);
                kb_altnum = -1;
                kb_ticks = 0;           /* stop key repeat */
                goto push_value;
            }
            break;
#ifdef MACHINE_AMIGA
        case KEY_CAPS:
            if (conterm & 1) {
                keyclick(KEY_CAPS);
            }
            shifty &= ~MODE_CAPS;       /* clear bit */
            break;
#endif
        case KEY_HOME:
            shifty &= ~MODE_HOME;       /* clear bit */
            kb_ticks = 0;               /* stop key repeat */
            break;
        case KEY_INSERT:
            shifty &= ~MODE_INSERT;     /* clear bit */
            kb_ticks = 0;               /* stop key repeat */
            break;
        default:                    /* non-modifier keys: */
            if (scancode_only == kb_last_actual)    /* key-up matches last key-down: */
                kb_ticks = 0;                       /*  stop key repeat */
        }
        if (arrowkey && (kb_arrowkeys > 0))
            kb_arrowkeys--;
        handle_mouse_mode(scancode);    /* exit mouse mode if appropriate */
        return;
    }

    /*
     * a key has been pressed
     */
    if (arrowkey)
        kb_arrowkeys++;
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
#ifdef MACHINE_AMIGA
        shifty |= MODE_CAPS;    /* set bit */
#else
        shifty ^= MODE_CAPS;    /* toggle bit */
#endif
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
        handle_mouse_mode(scancode);
        return;
    }

    /*
     * a non-modifier key has been pressed
     */
    kb_last_actual = scancode;  /* save because 'scancode' may get changed */
    if (shifty & MODE_ALT) {    /* only if the Alt key is down ... */
        switch(scancode) {
        case KEY_HOME:
            shifty |= MODE_HOME;    /* set bit */
            break;
        case KEY_INSERT:
            shifty |= MODE_INSERT;  /* set bit */
            break;
        }
    }

    ascii = convert_scancode(&scancode);
    if (ascii < 0)      /* dead key (including alt-keypad) processing */
        return;

    kb_ticks = kb_initial;

  push_value:
    if (conterm & 1)
        keyclick(scancode);

    /*
     * save last key info for do_key_repeat()
     */
    kb_last.k.shifty = shifty;
    kb_last.k.scancode = scancode;
    kb_last.k.ascii = ascii;
    kb_last_ikbdsys = kbdvecs.ikbdsys;

    /*
     * if we're not sending mouse packets, send a real key
     */
    if (!mouse_packet[0])
        push_ikbdiorec(kb_last.key);
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
#elif CONF_WITH_FLEXCAN
    return -1; /* Always OK */
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
 *
 * Note: this effectively treats the cnt argument as unsigned, just like
 * Atari TOS does.
 *
 * Workaround: New Beat's 4kB Falcon demo "Blue" calls Ikbdws() with a
 * ridiculously small stack (sp == 0x22), so keep stack usage as small as
 * possible here.
 */
void ikbdws(WORD cnt, const UBYTE *ptr)
{
    do
    {
        ikbd_writeb(*ptr++);
    } while (cnt--);
}

/* send a byte to the IKBD - for general use */
void ikbd_writeb(UBYTE b)
{
    KDEBUG(("ikbd_writeb(0x%02x)\n", (UBYTE)b));

    while (!bcostat4());
#if CONF_WITH_IKBD_ACIA
    ikbd_acia.data = b;
#elif CONF_WITH_FLEXCAN
    coldfire_flexcan_ikbd_writeb(b);
#elif defined (MACHINE_AMIGA)
    amiga_ikbd_writeb(b);
#endif
}

/* send a word to the IKBD as two bytes - for general use */
void ikbd_writew(WORD w)
{
    ikbd_writeb(HIBYTE(w));
    ikbd_writeb(LOBYTE(w));
}

/* Read a byte from the IKBD, with a timeout in msec.
 * This must not be used when interrupts are enabled.
 */
static UBYTE ikbd_readb(WORD timeout)
{
#if CONF_WITH_IKBD_ACIA
    WORD i;

    /* We have to use a timeout to avoid waiting forever
     * if the keyboard is unplugged.
     */
    for (i = 0; i < timeout; i++)
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
 * reset the IKBD
 *
 * According to Atari documentation, the IKBD answers to the Reset command
 * with a version byte when the reset is complete.  At one time we just
 * waited for a byte before sending further commands.  However, if we are
 * rebooting because of Ctrl+Alt+Del, and we keep Ctrl+Alt+Del held down,
 * a real IKBD transmits the following within about 100msec of the reset
 * command (info courtesy Christian Zietz):
 *  0x9d    Ctrl released
 *  0xf1    version byte
 *  0x1d    Ctrl pressed
 *  0x38    Alt pressed
 *  0x53    Del pressed
 * As a result, EmuTOS sees another reboot request, and will reboot
 * continually while the keys are held down.  Atari TOS does not have this
 * problem, because it just delays for approx 312msec after the reset and
 * does not see the additional keys.
 *
 * It is difficult to do exactly the same as Atari TOS, because the only
 * timing available on all systems at this point is the loop counter, and
 * this is not yet calibrated.  If the delay is too short, we may see the
 * spurious bytes; if the delay is too long, it not only slows down the
 * boot process, it may also cause problems with IKBD replacement hardware.
 * For example, the QWERTYX hardware will not respond if there is too great
 * a delay between the reset and data from the OS (the 'Set date' command).
 *
 * The following code is supposed to handle all this ...
 */
static void ikbd_reset(void)
{
    UBYTE version;

    ikbd_writew(0x8001);            /* reset */

    /* first, wait for the version byte */
    while(1)
    {
        version = ikbd_readb(300);  /* 'standard' 300msec timeout */
        if (version == 0)           /* timeout, we give up */
            return;

        if ((version&0xf0) == 0xf0) /* version byte, usually 0xf1 */
        {
            KDEBUG(("ikbd_version = 0x%02x\n", version));
            break;
        }
    }

    /* eat any pending keyboard bytes */
    while(ikbd_readb(5))    /* timeout long enough for pending data */
        ;
}

/*
 *      FUNCTION:  This routine resets the keyboard,
 *        configures the MFP so we can get interrupts
 */

void kbd_init(void)
{
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

#if CONF_WITH_FLEXCAN
    /* On ColdFire machines, an Eiffel adapter may be present on the CAN bus. */
    coldfire_init_flexcan();
#endif

#ifdef MACHINE_AMIGA
    amiga_kbd_init();
#endif

    /* initialize the IKBD */
    ikbd_reset();

    /* initialize the key repeat stuff */
    kb_ticks = 0;
    kb_initial = KB_INITIAL;
    kb_repeat = KB_REPEAT;

    kb_arrowkeys = 0;  /* no arrow keys pressed initially */
    kb_dead = -1;      /* not in a dead key sequence */
    kb_altnum = -1;    /* not in an alt-numeric sequence */
    kb_switched = 0;   /* not switched initially */

    conterm = 7;       /* keyclick, key repeat & system bell on by default */

    shifty = 0;        /* initial state of modifiers */

    mouse_packet[0] = 0;    /* not doing mouse emulation */

    bioskeys();
}
