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
 * - alt keys for non-us keyboards
 * - KEYTBL.TBL config with _AKP cookie (tos 5.00 and later)
 * - CLRHOME and INSERT in kbshift.
 * - key repeat
 */
 
#include "config.h"

#include "portab.h"
#include "bios.h"
#include "acia.h"
#include "kprint.h"
#include "tosvars.h"
#include "iorec.h"
#include "asm.h"
#include "ikbd.h"
#include "sound.h"  /* for keyclick */


#define DBG_KBD 0



/* scancode definitions */
#define KEY_RELEASED 0x80     /* This bit set, when key-release scancode */

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
#define MODE_RSHIFT 0x01      /* right shift keys is down */
#define MODE_LSHIFT 0x02      /* right shift keys is down */
#define MODE_CTRL   0x04      /* CTRL is down.*/
#define MODE_ALT    0x08      /* ALT is down.			     */
#define MODE_CAPS   0x10      /* ALPHA LOCK is down. 		     */
#define MODE_CLEAR  0x20      /* CLR/HOME mode key is down    */

#define MODE_SHIFT   (MODE_RSHIFT|MODE_LSHIFT)      /* shifted */



/*==== Global variables ===================================================*/
BYTE	shifty;          /* reflect the status up/down of mode keys */


/*==== Keyboard layouts ===================================================*/

/* To add a keyboard, please do the following:
 * - create a file bios/keyb_xx.h 
 *   (supplied tools dumpkbd.prg and keytbl2c may help)
 * - add a #if DEFAULT_KEYBOARD == n ... #endif test below
 * - add a #include "keyb_xx.h" below
 * - add a line in config.h giving the correspondance between
 *   the keyboard and its number.
 */

/* How it works:
 *
 * The DEFAULT_KEYBOARD macro is set in config.h.
 * Depending on its value, the code below will generate the 
 * right combination of KEYB_xx and DFLT_KEYB_xx:
 * - KEYB_xx means that keyboard xx is available 
 * - DFLT_KEYB_xx means that it is the default keyboard
 * (It is done this way to avoid changing all keyboard definition 
 * files if EmuTOS does one day support multiple keyboards in the
 * same ROM.)
 */

#if DEFAULT_KEYBOARD == 1
#define KEYB_US 1
#define DFLT_KEYB_US 1
#endif
#if DEFAULT_KEYBOARD == 2
#define KEYB_DE 1
#define DFLT_KEYB_DE 1
#endif
#if DEFAULT_KEYBOARD == 3
#define KEYB_FR 1
#define DFLT_KEYB_FR 1
#endif

/* include here all available keyboard definitions */

#include "keyb_us.h"
#include "keyb_de.h"
#include "keyb_fr.h"


#ifndef dflt_keytbl
#error "Invalid Keyboard Configuration."
#endif


/*==== Scancode table control (not yet implemented) =======================*/
#if 0
static BYTE ascii_ctrl [] = {
    0x00, 0x00, 0x00, 0x00, 0x1b, 0x1c, 0x1d, 0x1e,
    0x1f, 0x7f, 0x00, 0x00, 0x7f, 0x00, 0x08, 0x00,
    0x11, 0x17, 0x05, 0x12, 0x14, 0x19, 0x15, 0x09,
    0x0f, 0x10, 0x1b, 0x1d, 0x01, 0x02, 0x01, 0x13,
    0x04, 0x06, 0x07, 0x08, 0x0a, 0x0b, 0x0c, 0x00,
    0x07, 0x00, 0x00, 0x1c, 0x1a, 0x18, 0x03, 0x16,
    0x02, 0x0e, 0x0d, 0x00, 0x00, 0x7f, 0x00, 0x00,
    0x03, 0x00, 0x07, 0x00, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x00, 0x14,
    0x03, 0x00, 0x0b, 0x01, 0x00, 0x02, 0x0a, 0x00,
    0x00, 0x00, 0x15, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0x02, 0x12, 0x13, 0x0d, 0x0c, 0x07,
    0x08, 0x09, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03,
    0x00, 0x10, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif


/*==== Scancode table alt (not yet implemented) ===========================*/
#if 0
static BYTE ascii_alt [] = {
    0x00, 0x1b, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x30, 0x9e, 0x27, 0x08, 0x09,
    0x71, 0x77, 0x65, 0x72, 0x74, 0x7a, 0x75, 0x69,
    0x6f, 0x70, 0x81, 0x2b, 0x0d, 0x00, 0x61, 0x73,
    0x64, 0x66, 0x67, 0x68, 0x6a, 0x6b, 0x6c, 0x94,
    0x84, 0x23, 0x00, 0x7e, 0x79, 0x78, 0x63, 0x76,
    0x62, 0x6e, 0x6d, 0x2c, 0x2e, 0x2d, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x2b, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x28, 0x29, 0x2f, 0x2a, 0x37,
    0x38, 0x39, 0x34, 0x35, 0x36, 0x31, 0x32, 0x33,
    0x30, 0x2e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

/*=== Keymaps handling (xbios) =======================================*/

static struct keytbl current_keytbl;

LONG keytbl(LONG norm, LONG shft, LONG caps)
{
  if(norm != -1L) {
    current_keytbl.norm = (BYTE *)norm;
  }
  if(shft != -1L) {
    current_keytbl.shft = (BYTE *)shft;
  }
  if(caps != -1L) {
    current_keytbl.caps = (BYTE *)caps;
  }
  return (LONG) &current_keytbl;
}

VOID bioskeys(VOID)
{
    current_keytbl = dflt_keytbl;
}

/*=== kbshift (bios) =====================================================*/

LONG kbshift(WORD flag)
{
    WORD oldy;

    if(flag == -1)
        return(shifty);         /* return bitvector of shift state */

    oldy = shifty;
    shifty=flag;

    return(oldy);
}

/*=== iorec handling (bios) ==============================================*/

LONG bconstat2(VOID)
{
  if(ikbdiorec.head == ikbdiorec.tail) {
    return 0;   /* iorec empty */
  } else {
    return -1;  /* not empty => input available */
  }
}

LONG bconin2(VOID)
{
  WORD old_sr;
  LONG value;

  while(!bconstat2()) 
    ;
  /* disable interrupts */
  old_sr = set_sr(0x2700);
  
  ikbdiorec.head += 4;
  if(ikbdiorec.head >= ikbdiorec.size) {
    ikbdiorec.head = 0;
  }
  value = *(LONG *)(ikbdiorec.buf+ikbdiorec.head);
  
  /* restore interrupts */
  set_sr(old_sr);
  return value;
}

static VOID push_ikbdiorec(LONG value)
{
  ikbdiorec.tail += 4;
  if(ikbdiorec.tail >= ikbdiorec.size) {
    ikbdiorec.tail = 0;
  }
  if(ikbdiorec.tail == ikbdiorec.size) {
    /* iorec full */
    return;
  }
  *(LONG *)(ikbdiorec.buf+ikbdiorec.tail) = value;
}

/*=== interrupt routine support ===================================*/

/*
 * kbd_int : called by the interrupt routine for key events.
 */

VOID kbd_int(WORD scancode)
{
  LONG value;        /* the value to push into iorec */
  UBYTE ascii = 0;
    
    
#if DBG_KBD
  kprint ("================\n ");
  kprintf ("Key-scancode: 0x%02x\n", scancode & 0xff);
  
  kprintf ("Key-shift bits: 0x%02x\n", shifty);
#endif

  if (scancode & KEY_RELEASED) {
    scancode &= ~KEY_RELEASED;       /* get rid of release bits */
    switch (scancode) {
    case KEY_RSHIFT:
      shifty &= ~MODE_RSHIFT;        /* clear bit */
      break;
    case KEY_LSHIFT:
      shifty &= ~MODE_LSHIFT;        /* clear bit */
      break;
    case KEY_CTRL:
      shifty &= ~MODE_CTRL;          /* clear bit */
      break;
    case KEY_ALT:
      shifty &= ~MODE_ALT;           /* clear bit */
      break;
    }
    /* The TOS does not return when ALT is set, to emulate
     * mouse movement using alt keys. This feature is not 
     * currently supported by EmuTOS.
     */
#if 0
    if(! (shifty & KEY_ALT))
#endif
    return;
  }

  switch (scancode) {
    case KEY_RSHIFT:
      shifty |= MODE_RSHIFT;         /* set bit */
      return;
    case KEY_LSHIFT:
      shifty |= MODE_LSHIFT;         /* set bit */
      return;
    case KEY_CTRL:
      shifty |= MODE_CTRL;           /* set bit */
      return;
    case KEY_ALT:
      shifty |= MODE_ALT;            /* set bit */
      return;
    case KEY_CAPS:
      shifty ^= MODE_CAPS;           /* toggle bit */
      if(conterm & 1) keyclick();
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
    while(*a && *a != scancode) {
      a += 2;
    }
    if(*a++) {
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
  if(conterm & 1) keyclick();
  value = ((LONG)scancode & 0xFF)<<16;
  value += ascii;
  if (conterm & 0x8) {
    value += ((LONG)shifty) << 24;
  }
#if DBG_KBD
  kprintf ("KBD iorec: Pushing value 0x%08lx\n", value);
#endif
  push_ikbdiorec(value);
}


/*=== ikbd acia stuff ==================================================*/

/* can we send a byte to the ikbd ? */
LONG bcostat4(VOID)
{
  if(ikbd_acia.ctrl & ACIA_TDRE) {
    return -1;  /* OK */
  } else {
    /* Data register not empty */
    return 0;   /* not OK */
  }
}

/* send a byte to the IKBD */
VOID bconout4(WORD dev, WORD c)
{
  while(! bcostat4())
    ;
  ikbd_acia.data = c;
}

/* cnt = number of bytes to send less one */
VOID ikbdws(WORD cnt, UBYTE * ptr)
{
  while(cnt-- >= 0) {
    bconout4(0, *ptr++);
  }
}


/* Reset (without touching the clock) */
VOID ikbd_reset(VOID)
{
    UBYTE cmd[2] = { 0x80, 0x01 };
    
    ikbdws(2, cmd);

    /* if all's well code 0xF1 is returned, else the break codes of
       all keys making contact */
}

/* Set mouse button action */
VOID ikbd_mouse_button_action(int mode)
{
    UBYTE cmd[2] = { 0x07, mode };

    ikbdws(2, cmd);
}

/* Set relative mouse position reporting */
VOID ikbd_mouse_rel_pos(VOID)
{
    UBYTE cmd[1] = { 0x08 };

    ikbdws(1, cmd);
}

/* Set absolute mouse position reporting */
VOID ikbd_mouse_abs_pos(int xmax, int ymax)
{
    BYTE cmd[5] = { 0x09, xmax>>8, xmax&0xFF, ymax>>8, ymax&0xFF };

    ikbdws(5, cmd);
}

/* Set mouse keycode mode */
VOID ikbd_mouse_kbd_mode(int dx, int dy)
{
    BYTE cmd[3] = { 0x0A, dx, dy };

    ikbdws(3, cmd);
}

/* Set mouse threshold */
VOID ikbd_mouse_thresh(int x, int y)
{
    BYTE cmd[3] = { 0x0B, x, y };

    ikbdws(3, cmd);
}

/* Set mouse scale */
VOID ikbd_mouse_scale(int x, int y)
{
    BYTE cmd[3] = { 0x0C, x, y };

    ikbdws(3, cmd);
}

/* Interrogate mouse position */
VOID ikbd_mouse_pos_get(int *x, int *y)
{
    UBYTE cmd[1] = { 0x0D };

    ikbdws(1, cmd);

    /* wait for returning bytes */
}

/* Load mouse position */
VOID ikbd_mouse_pos_set(int x, int y)
{
    BYTE cmd[6] = { 0x0E, 0x00, x>>8, x&0xFF, y>>8, y&0xFF };

    ikbdws(6, cmd);
}

/* Set Y=0 at bottom */
VOID ikbd_mouse_y0_bot(VOID)
{
    UBYTE cmd[1] = { 0x0F };

    ikbdws(1, cmd);
}

/* Set Y=0 at top */
VOID ikbd_mouse_y0_top(VOID)
{
    UBYTE cmd[1] = { 0x10 };

    ikbdws(1, cmd);
}

/* Resume */
VOID ikbd_resume(VOID)
{
    UBYTE cmd[1] = { 0x11 };

    ikbdws(1, cmd);
}

/* Disable mouse */
VOID ikbd_mouse_disable(VOID)
{
    UBYTE cmd[1] = { 0x12 };

    ikbdws(1, cmd);
}

/* Pause output */
VOID ikbd_pause(VOID)
{
    UBYTE cmd[1] = { 0x13 };

    ikbdws(1, cmd);
}

/* Set joystick event reporting */
VOID ikbd_joystick_event_on(VOID)
{
    UBYTE cmd[1] = { 0x14 };

    ikbdws(1, cmd);
}

/* Set joystick interrogation mode */
VOID ikbd_joystick_event_off(VOID)
{
    UBYTE cmd[1] = { 0x15 };

    ikbdws(1, cmd);
}

/* Joystick interrogation */
VOID ikbd_joystick_get_state(VOID)
{
    UBYTE cmd[1] = { 0x16 };

    ikbdws(1, cmd);
}

/* some joystick routines not in yet (0x18-0x19) */

/* Disable joysticks */
VOID ikbd_joystick_disable(VOID)
{
    UBYTE cmd[1] = { 0x1A };

    ikbdws(1, cmd);
}

/* Time-of-day clock set */
VOID ikbd_clock_set(int year, int month, int day, int hour, int minute, int second)
{
    BYTE cmd[7] = { 0x1B, year, month, day, hour, minute, second };

    ikbdws(7, cmd);
}

/* Interrogate time-of-day clock */
VOID ikbd_clock_get(int *year, int *month, int *day, int *hour, int *minute, int second)
{
    UBYTE cmd[1] = { 0x1C };

    ikbdws(1, cmd);
}

/* Memory load */
VOID ikbd_mem_write(int address, int size, BYTE *data)
{
    kprintf("Attempt to write data into keyboard memory");
    while(1);
}

/* Memory read */
VOID ikbd_mem_read(int address, BYTE data[6])
{
    BYTE cmd[3] = { 0x21, address>>8, address&0xFF };

    ikbdws(3, cmd);

    /* receive data and put it in data */
}

/* Controller execute */
VOID ikbd_exec(int address)
{
    BYTE cmd[3] = { 0x22, address>>8, address&0xFF };

    ikbdws(3, cmd);
}

/* Status inquiries (0x87-0x9A) not yet implemented */

/* Set the state of the caps lock led. */
VOID atari_kbd_leds (unsigned int leds)
{
    BYTE cmd[6] = {32, 0, 4, 1, 254 + ((leds & 4) != 0), 0};

    ikbdws(6, cmd);
}



/*
 *	FUNCTION:  This routine resets the keyboard,
 *	  configures the MFP so we can get interrupts
 */
 
VOID kbd_init(VOID)
{
    /* initialize ikbd ACIA */
    ikbd_acia.ctrl =
        ACIA_RESET;     /* master reset */

    ikbd_acia.ctrl =
        ACIA_RIE|       /* enable interrupts */
        ACIA_RLTID|     /* RTS low, TxINT disabled */
        ACIA_DIV64|     /* clock/64 */
        ACIA_D8N1S;  /* 8 bit, 1 stop, no parity */

    /* initialize the IKBD */
    ikbd_reset();

    ikbd_joystick_disable();
    ikbd_mouse_disable();

    bioskeys();
}



