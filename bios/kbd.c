/*
 * kbd.c - Intelligent keyboard routines
 *
 * Copyright (c) 2001 Laurent Vogel, Martin Doering
 *
 * Authors:
 *  LAV   Laurent Vogel
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include	"portab.h"
#include	"bios.h"
#include	"kbd.h"
#include	"kbq.h"
#include	"acia.h"
#include	"kprint.h"


#define DBG_KBD 0

/* External declarations */
extern VOID clear_kbdint(VOID);        /* from mfp.c */



/* Defines */
#define ACIA_IKBD_BASE (0xfffffc00)

#define acia (*(volatile struct ACIA*)ACIA_IKBD_BASE)


#define	DEFAULT_KEYB_REP_DELAY	(HZ/4)
#define	DEFAULT_KEYB_REP_RATE	(HZ/25)

#define BREAK_MASK	(0x80)

typedef enum kb_state_t
{
    KEYBOARD,
    AMOUSE,
    RMOUSE,
    JOYSTICK,
    CLOCK,
    RESYNC
} KB_STATE_T;

#define	IS_SYNC_CODE(sc)	((sc) >= 0x04 && (sc) <= 0xfb)

typedef struct keyboard_state
{
    UBYTE       buf[6];
    UBYTE       len;
    KB_STATE_T  state;
} KEYBOARD_STATE;

KEYBOARD_STATE kb_state;

static UBYTE rep_scancode;




#define	MAXUNITS	1
#define	doasr(f,p1,p2,x)	f(p1,p2)
#define	DVR_KEYBD		0L
#define	E_SUCCESS		0L

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

#define MODE_SHIFT   (MODE_RSHIFT|MODE_LSHIFT|MODE_CAPS)      /* shifted */


#define FUNCTION	0x1000
#define SPECIAL		0x2000

/*
 * some defines to eliminate magic numbers...
 */

#define _ESC	0x1b			/* \033 - escape character */

#define _NULL	0x0			/* no character flag */

/*
 *	SPECIAL - special key return values
 */

#define _BREAK		0x05
#define _RESET		0x06		/* REDRAW Concurrent function */
#define _CLEAR		_NULL

#define _TEST		_NULL
#define _HELP		0x7F	/* TEMP KLUDGE - SHOULD BE 0, BUT 0 = NO CH */

#define	_ZERO	0x20
#define	_ONE	0x21
#define	_TWO	0x22
#define	_THREE	0x23
#define	_FOUR	0x24
#define	_FIVE	0x25
#define	_SIX	0x26
#define	_SEVEN	0x27
#define	_EIGHT	0x28
#define	_NINE	0x29
#define	_A	0x2A
#define	_B	0x2B
#define	_C	0x2C
#define	_D	0x2D
#define	_E	0x2E
#define	_F	0x2F
#define	_COMMA	0x31
#define _PERIOD	0x32

	/* Group 0 keys. */

#define _ENTER		0x30
#define _HOME		0x18
#define _UP		0x10
#define _DOWN		0x11
#define _LEFT		0x12
#define _RIGHT		0x13
#define _BACKTAB	_NULL	/* undefined */

/* scancode definitions */
#define KEY_RELEASED 0x80     /* This bit set, when key-release scancode */

#define KEY_LSHIFT  0x2a
#define KEY_RSHIFT  0x36
#define KEY_CTRL    0x1d
#define KEY_ALT     0x38
#define KEY_CAPS    0x3a
;


/*==== Global variables ===================================================*/
BYTE	shifty;          /* reflect the status up/down of mode keys */



/*==== Scancode table unshifted ===========================================*/
static BYTE ascii_norm [] = {
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



/*==== Scancode table shifted =============================================*/
static BYTE ascii_shift [] = {
    0x00, 0x1b, 0x21, 0x22, 0xdd, 0x24, 0x25, 0x26,
    0x2f, 0x28, 0x29, 0x3d, 0x3f, 0x60, 0x08, 0x09,
    0x51, 0x57, 0x45, 0x52, 0x54, 0x5a, 0x55, 0x49,
    0x4f, 0x50, 0x9a, 0x2a, 0x0d, 0x00, 0x41, 0x53,
    0x44, 0x46, 0x47, 0x48, 0x4a, 0x4b, 0x4c, 0x99,
    0x8e, 0x5e, 0x00, 0x3e, 0x59, 0x58, 0x43, 0x56,
    0x42, 0x4e, 0x4d, 0x3b, 0x3a, 0x5f, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37,
    0x38, 0x00, 0x2d, 0x34, 0x00, 0x36, 0x2b, 0x00,
    0x32, 0x00, 0x30, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3e, 0x00, 0x00, 0x28, 0x29, 0x2f, 0x2a, 0x37,
    0x38, 0x39, 0x34, 0x35, 0x36, 0x31, 0x32, 0x33,
    0x30, 0x2e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*==== Scancode table with caps lock ======================================*/
static BYTE ascii_caps [] = {
    0x00, 0x1b, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x30, 0x9e, 0x27, 0x08, 0x09,
    0x51, 0x57, 0x45, 0x52, 0x54, 0x5a, 0x55, 0x49,
    0x4f, 0x50, 0x9a, 0x2b, 0x0d, 0x00, 0x41, 0x53,
    0x44, 0x46, 0x47, 0x48, 0x4a, 0x4b, 0x4c, 0x99,
    0x8e, 0x23, 0x00, 0x3c, 0x59, 0x58, 0x43, 0x56,
    0x42, 0x4e, 0x4d, 0x2c, 0x2e, 0x2d, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x2D, 0x00, 0x00, 0x00, 0x2B, 0x00,
    0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3C, 0x00, 0x00, 0x28, 0x29, 0x2F, 0x2A, 0x37,
    0x38, 0x39, 0x34, 0x35, 0x36, 0x31, 0x32, 0x33,
    0x30, 0x2E, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



/*==== Scancode table control =============================================*/
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



/*==== Scancode table alt (not yet implemented) ===========================*/
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



/*
 * Send Byte to keyboard ACIA
 */

VOID    send_ikbd_acia(UBYTE c)
{
    while((acia.ctrl & ACIA_TDRE));     /* Wait till Data Register empty*/
    acia.data = c;
}



/*
 * kbd_decode - decode a scancode from the keyboard queue
 */

KBCHAR	kbd_decode(KBQENTRY kbqe , UWORD *kbstatus )
{
    KBSCN scancode; /* scancode (just key-codes) from ikbd */
    KBCHAR rv;      /* return value: scancode + ascii representation */
    
    *kbstatus = (kbqe>>8) & 0x00ff ;	/*  get status word part of entry	*/
    scancode = (BYTE)kbqe & 0x00ff ;	/*  get scan code part of entry	*/

    rv=(KBCHAR)scancode;        /* put the scancode value to ... */
    rv<<=16;                    /* ... low byte of high word of rv */

    *kbstatus = NOCHAR;      /* initial setting */

#if DBG_KBD
    kprint ("================\n ");
    kprint ("Key-scancode: ");
    kputp(scancode);
    kprint ("\n");

    kprint ("Key-shift bits: ");
    kputp(shifty);
    kprint ("\n");
#endif

    if (scancode&KEY_RELEASED) {

        scancode&=~KEY_RELEASED;         /* get rid of release bits */
        switch (scancode) {


        case KEY_RSHIFT:
            shifty&=~MODE_RSHIFT;        /* clear bit */
            break;

        case KEY_LSHIFT:
            shifty&=~MODE_LSHIFT;        /* clear bit */
            break;

        case KEY_CTRL:
            shifty&=~MODE_CTRL;          /* clear bit */
            break;

        case KEY_ALT:
            shifty&=~MODE_ALT;           /* clear bit */
            break;

        case KEY_CAPS:
            shifty&=~MODE_CAPS;          /* clear bit */
            break;
        }
    }
    else {

        switch (scancode) {

        case KEY_RSHIFT:
            shifty|=MODE_RSHIFT;         /* set bit */
            break;

        case KEY_LSHIFT:
            shifty|=MODE_LSHIFT;         /* set bit */
            break;

        case KEY_CTRL:
            shifty|=MODE_CTRL;           /* set bit */
            break;

        case KEY_ALT:
            shifty|=MODE_ALT;            /* set bit */
            break;

        case KEY_CAPS:
            shifty|=MODE_CAPS;           /* set bit */
            break;

        default:
            if (shifty&MODE_CTRL)
                rv|=ascii_ctrl[(int)scancode];
            else if (shifty&MODE_ALT)
                rv|=ascii_alt[(int)scancode];
            else if (shifty&MODE_CAPS)
                rv|=ascii_caps[(int)scancode];
            else if (shifty&(MODE_LSHIFT|MODE_RSHIFT))
                rv|=ascii_shift[(int)scancode];
            else
                rv|=ascii_norm[(int)scancode];

            *kbstatus = IN_CHAR;
        }
    }
    
#if DBG_KBD
    kprint ("Key: ");
    kputp(rv);
    kprint ("\n");
#endif
    return(rv); /* return scancode (high word) and ascii code (low word) */
}



/*==== kbint - keyboard interrupt service routine =========================*/
/*
 *	keyboard Interrupt Service Routine for
 *	this routine is invoked upon receipt of an interrupt
 *	from the keyboard by the system.  it retrieves the
 *	scan code and, if no error was detected,
 *	puts the scan code into a queue to be retrieved at a more
 *	convenient time.
 */
 
VOID	kbd_int(VOID)
{
    UBYTE scancode;         /* scan code received from keyboard */
    UBYTE break_flag;
    UBYTE acia_stat;


    /* Loop, till no more bytes to get from ACIA */
    for (;;) {
        acia_stat = acia.ctrl;

        /* return, if no interrupt from this ACIA */
        if (!((acia_stat | acia.ctrl) & ACIA_IRQ)){
#if DEBUG
            kprint("BIOS: No further keyboard interrupt ...\n");
#endif
            break;      /* No more data to get -> leave for loop */
        }

        if (acia_stat & (ACIA_FE | ACIA_PE))
        {
            kprint("BIOS: Error in keyboard communication\n");
            break;      /* No more useful thing to do -> leave for loop */
        }

        scancode = acia.data;            /* fetch the code */

        if (acia_stat & ACIA_OVRN)
        {
            /* a very fast typist or a slow system, give a warning */
            /* ...happens often if interrupts were disabled for too long */
            kprint( "BIOS: Keyboard overrun\n");

            /* Turn off autorepeating in case a break code has been lost */
            rep_scancode = 0;
            if (IS_SYNC_CODE(scancode)) {
                /* This code seem already to be the start of a new packet or a
                 * single scancode */
                kb_state.state = KEYBOARD;
            }
            else {
                /* Go to RESYNC state and skip this byte */
                kb_state.state = RESYNC;
                kb_state.len = 1; /* skip max. 1 another byte */
            }
        }

        if (acia_stat & ACIA_RDRF)	/* received a character */
        {
            //    interpret_scancode:
            switch (kb_state.state)
            {
            case KEYBOARD:
                switch (scancode)
                {
                case 0xF7:      /* absolute mouse position */
                    kb_state.state = AMOUSE;
                    kb_state.len = 0;
                    break;

                case 0xF8:      /* relative mouse position */
                case 0xF9:
                case 0xFA:
                case 0xFB:
                    kb_state.state = RMOUSE;
                    kb_state.len = 1;
                    kb_state.buf[0] = scancode;
                    break;

                case 0xFC:      /* clock data */
                    kb_state.state = CLOCK;
                    kb_state.len = 0;
                    break;

                case 0xFE:      /* joystick data */
                case 0xFF:
                    kb_state.state = JOYSTICK;
                    kb_state.len = 1;
                    kb_state.buf[0] = scancode;
                    break;

                default:        /* keyboard scancodes */
                    break_flag = scancode & BREAK_MASK;

                    if (break_flag) {
                        rep_scancode = 0;
                    }
                    else {
                        rep_scancode = scancode;
                    }
#if DEBUG
                    kprint("BIOS: Key event happened, scancode: ");
                    kputb(&scancode);
                    kprint("\n");
#endif
                    kbq_add((KBQENTRY)(scancode&=0x00ff));/* put into queue as WORD */
                    break;
                }
                break;

            case AMOUSE:
                kb_state.buf[kb_state.len++] = scancode;
                if (kb_state.len == 5)
                {
                    kb_state.state = KEYBOARD;
                    /* not yet used */
                    /* wake up someone waiting for this */
                }
                break;

            case RMOUSE:
                kb_state.buf[kb_state.len++] = scancode;
                if (kb_state.len == 3)
                {
                    kb_state.state = KEYBOARD;
                    //                    if (atari_mouse_interrupt_hook)
                    //                        atari_mouse_interrupt_hook(kb_state.buf);
                }
                break;

            case JOYSTICK:
                kb_state.buf[1] = scancode;
                kb_state.state = KEYBOARD;
                //                atari_joystick_interrupt(kb_state.buf);
                break;

            case CLOCK:
                kb_state.buf[kb_state.len++] = scancode;
                if (kb_state.len == 6)
                {
                    kb_state.state = KEYBOARD;
                    /* wake up someone waiting for this.
                     But will this ever be used, as Linux keeps its own time.
                     Perhaps for synchronization purposes? */
                    /* wake_up_interruptible(&clock_wait); */
                }
                break;

            case RESYNC:
                if (kb_state.len <= 0 || IS_SYNC_CODE(scancode)) {
                    kb_state.state = KEYBOARD;
                    //                goto interpret_scancode;
                } else {
                    kb_state.len--;
                    break;
                }
            } /* end of switch status */
        } /* end character received */
    } /* end for loop */
    clear_kbdint();
}



/*==== kbd_read - get a character from the keyboard queue. ================*/
/*
 *	Someone wants to read the keyboard.  So we see if there is any
 *	chars in the keyboard queue.  If so, we decode it and give it
 *	to the caller.  Otherwise, we return -1.
 */

KBCHAR	kbd_read(VOID)
{
    KBQENTRY	kbqent ;
    KBCHAR	kbchar ;
    UWORD	status ;

    while( kbq_del( &kbqent ) )
    {
        kbchar = kbd_decode( kbqent , &status ) ;
        if( status == IN_CHAR )
            return( kbchar ) ;
    }
    /*  no valid characters available  */

    return( -1L ) ;
}



/*==== kbd_delay - just a delay loop ======================================*/
VOID    kbd_delay(REG LONG i)
{
	while(--i) ;
}



/*==== kbd_init - initialize the keyboard =================================*/
/*
 *	FUNCTION:  This routine resets the keyboard,
 *	  configures the MFP so we can get interrupts
 */
 
VOID	kbd_init(VOID)
{

    cputs("[    ] IKBD ACIA initialized ...\r");

    /* initialize ikbd ACIA */
    acia.ctrl =
        ACIA_RESET;     /* master reset */

    acia.ctrl =
        ACIA_RIE|       /* enable interrupts */
        ACIA_RLTID|     /* RTS low, TxINT disabled */
        ACIA_DIV64|     /* clock/64 */
        ACIA_D8N1S;  /* 8 bit, 1 stop, no parity */

    /* initialize the IKBD */
#if IMPLEMENTED
    send_ikbd_acia(0x80);   /* reset IKBD */
    send_ikbd_acia(0x01);   /* also... */

    send_ikbd_acia(0x12);  /* disable mouse */
    send_ikbd_acia(0x1A);  /* disable joystick */
#endif

    cstatus(SUCCESS);
}
