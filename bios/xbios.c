/*
 *  biosc.c - C portion of XBIOS initialization and front end
 *
 * Copyright (c) 2001 Martin Doering
 *
 * Authors:
 *  MAD     Martin Doering
 *  THO     Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "abbrev.h"
#include "gemerror.h"
#include "kprint.h"



#define	DBG_XBIOS        1



/*
 *  external declarations
 */

/* extern char	*biosts ;*/	/*  time stamp string */
extern UBYTE *v_bas_ad;



/*
 * xbiosinit - XBIOS's initialization.
 *
 * XBIOS initialization. Must be done before any regular XBIOS calls are
 * performed. It is called from startup.s. This routine will do necessary
 * bios initialization that can be done in hi level lang. startup.s has
 * the rest.
 */

VOID xbiosinit()
{
    /*
     * Later print version string ...
     */

#if DBG_XBIOS
    kprint("XBIOS: Initialization ...\n");
#endif
}



/*
 * xbios_0 - (initmous) Initialize mouse packet handler
 */

void xbios_0(WORD type, LONG param, LONG vec)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x00 ...\n");
#endif
}



/*
 * xbios_1 - (ssbrk) Reserve 'amount' bytes from the top of memory.
 *
 * Returns a long pointing to the base of the allocated memory. This function
 * MUST be called before the OS is initialized.
 * ssbrk() is actually pretty useless. It DOES NOT work after GEMDOS has been
 * brought up, since the TPA has already been set up
 */

LONG xbios_1()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x01 ...\n");
#endif
    return(0);
}



/*
 * xbios_2 - (physBase) Get the screen's physical base address
 *
 * (at the beginning of the next vblank).
 */

ULONG xbios_2()
{
    ULONG addr;

#if DBG_XBIOS
    kprint("XBIOS: Physbase ...\n");
#endif

    addr = *(UBYTE *)0xffff8201;
    addr <<= 8;
    addr += *(UBYTE *)0xffff8203;
    addr <<= 8;
#if 0      /* The low byte only exists on STE, TT and Falcon */
    addr += *(UBYTE *)0xffff820D;  
#endif

    return(addr);
}



/*
 * xbios_3 - (logBase) Get the screen's logical base, right away.
 *
 * This is the location that GSX uses when drawing to the screen.
 */

LONG xbios_3()
{
#if DBG_XBIOS
    kprint("XBIOS: Logbase ...\n");
#endif
    return((ULONG)v_bas_ad);
}



/*
 * xbios_4 - (getRez) Get the screen's current resolution
 *
 * Returns 0, 1 or 2.
 */

WORD xbios_4()
{
#if DBG_XBIOS
    kprint("XBIOS: Getrez ...\n");
#endif

    return( *(UBYTE *)0xffff8260 );
}



/*
 * xbios_5 - (setScreen) Set the screen locations
 *
 * Set the logical screen location (logLoc), the physical screen location
 * (physLoc), and the physical screen resolution. Negative parameters are
 * ignored (making it possible, for instance, to set screen resolution without
 * changing anything else). When resolution is changed, the screen is cleared,
 * the cursor is homed, and the VT52 terminal emulator state is reset.
 */

VOID xbios_5(LONG logLoc, LONG physLoc, WORD rez)
{
#if DBG_XBIOS
    kprintf("XBIOS: SetScreen(log = 0x%08lx, phys = 0x%08lx, rez = 0x%04x)\n",
	   logLoc, physLoc, rez);
#endif
    if(logLoc >= 0) {
      v_bas_ad = (char *)logLoc;
    }
    if(physLoc >= 0) {
      *(UBYTE *)0xffff8201 = physLoc >> 16;
      *(UBYTE *)0xffff8203 = physLoc >> 8;
#if 0
      *(UBYTE *)0xffff820d = physLoc;
#endif
    }
    if(rez >= 0) {
      /* rez ignored for now */
    }
    
}



/*
 * xbios_6 - (setPallete) Set the contents of the hardware palette register
 *
 * (all 16 color entries) from the 16 words pointed to by 'palettePtr'.
 * 'paletteptr' MUST be on a word boundary. The palette assignment takes
 * place at the beginning of the next vertical blank interrupt.
 */

VOID xbios_6(LONG palettePtr)

{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x06 ...\n");
#endif
}



/*
 * xbios_7 - (setColor) Set the palette number
 *
 * Set the palette number 'colorNum' in the hardware palette table to the
 * specified 'color'. If 'color' is negative, the hardware register is not
 * changed.
 *
 * Return the old color. 
 */

WORD xbios_7(WORD colorNum, WORD color)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x07 ...\n");
#endif
    return(0);
}



/*
 * xbios_8 - (floprd) Read one or more sectors from a floppy disk.
 *
 * filler   - an unused longword.
 * buf      - must point to a word-aligned buffer large enough to contain the
 *            number of sectors requested.
 * devno    - the floppy number (0 or 1).
 * sectno   - the sector number to start reading from (usually 1 through 9).
 * trackno  - the track number to seek to.
 * sideno   - the side number to select.
 * count    - the number of sectors to read (which must be less than or equal
 *            to the number of sectors per track).
 *
 * Returns a status code.
 *  0       - the operation succeeded.
 *  nonzero - the operation failed. Returns an error number.
 */

WORD xbios_8(LONG buf, LONG filler, WORD devno, WORD sectno,
             WORD trackno, WORD sideno, WORD count)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x08 ...\n");
#endif
    return(-1);
}



/*
 * xbios_9 - (flopwr) Write one or more sectors to a floppy disk.
 *
 * filler   - an unused longword.
 * buf      - must point to a word-aligned buffer large enough to contain the
 *            number of sectors requested.
 * devno    - the floppy number (0 or 1).
 * sectno   - the sector number to start reading from (usually 1 through 9).
 * trackno  - the track number to seek to.
 * sideno   - the side number to select.
 * count    - the number of sectors to read (which must be less than or equal
 *            to the number of sectors per track).
 *
 * Writing to the boot sector (sector 1, side 0, track 0) will cause the
 * media to enter the "might have changed" state. This will be reflected
 * on the next rwabs() or mediach() BIOS call.
 *
 * Returns a status code.
 *  0       - the operation succeeded.
 *  nonzero - the operation failed. Returns an error number.
 */


WORD xbios_9(LONG buf, LONG filler, WORD devno, WORD sectno,
             WORD trackno, WORD sideno, WORD count)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x09 ...\n");
#endif
    return(-1);
}



/*
 * xbios_a - (flopfmt) Format a track on a floppy disk
 *
 * buf      - must point to a word-aligned buffer large enough to contain the
 *            number of sectors requested.
 * filler   - an unused longword.
 * devno    - the floppy number (0 or 1).
 * spt      - the number of sectors-per-track to format (usually 9).
 * trackno  - the track number to format (usually 0 to 79).
 * sideno   - the side number to format (0 or 1).
 * interlv  - the sector-interleave factor (usually 1).
 * magic    - a magic number that MUST be the value $87654321.
 * virgin   - a word fill value for new sectors.
 *
 * Returns a status code:
 *  zero    - the operation succeeded.
 *  nonzero - the operation failed, returns an error number).
 *
 * Also returns null-terminated list of bad sector numbers in the buffer.
 *
 */

WORD xbios_a(LONG buf, LONG filler, WORD devno, WORD spt,
             WORD trackno, WORD sideno, WORD interlv, WORD virgin,
             LONG magic)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x0a ...\n");
#endif
    return(-1);
}



/*
 * xbios_b - Used by BIOS - Obsolete function
 */

VOID xbios_b()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x0b ...\n");
#endif
}



/*
 * xbios_c - (midiws) Writes a string to the MIDI port.
 *
 * cnt   - the number of characters to write, minus one.
 * ptr   - points to a vector of characters to write.
 */

VOID xbios_c(WORD cnt, LONG ptr)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x0c ...\n");
#endif
}



/*
 * xbios_d - (mfpint) Set the MFP interrupt number
 *
 * Set the MFP interrupt number 'interno' (0 to 15) to 'vector'.
 * The old vector is written over (and thus unrecoverable).
 */


VOID xbios_d(WORD interno, LONG vector)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x0d ...\n");
#endif
}



/*
 * xbios_e - (iorec) Returns pointer to a serial device's input buffer record.
 */

LONG xbios_e(WORD devno)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x0e ...\n");
#endif
    return(-1);
}



/*
 * xbios_f - (rsconf) Configure RS-232 port.
 *
 * If any parameter is -1 ($FFFF), the corresponding hardware register
 * is not set.
 *
 * speed  - the port's baud rate
 * flow   - the flow control
 * ucr    - 68901 register
 * rsr    - 68901 register
 * tsr    - 68901 register
 * scr    - 68901 register
 */

VOID xbios_f(WORD speed, WORD flowctl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x0f ...\n");
#endif
}



/*
 * xbios_10 - (keytbl) Sets pointers to the keyboard translation tables
 *
 * Sets pointers to the keyboard translation tables for un-shifted keys,
 * shifted keys, and keys in caps-lock mode.
 *
 * Returns a pointer to the beginning of a keytab structure. Each pointer
 * in the structure should point to a table 128 bytes in length. A scancode
 * is converted to Ascii by indexing into the table and taking the byte there.
 */

LONG xbios_10(LONG unshift, LONG shift, LONG capslock)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x10 ...\n");
#endif
    return(-1);
}



/*
 * xbios_11 - (random) Returns a 24-bit pseudo-random number
 *
 * Bits 24..31 will be zero.
 */

LONG xbios_11()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x11 ...\n");
#endif
    return(0);
}



/*
 * xbios_12 - (protobt) Prototype an image of a boot sector.
 *
 * Once the boot sector image has been constructed with this function,
 * write it to the volume's boot sector.
 *
 * buf       - points to a 512-byte buffer (which may contain garbage,
 *             or already contain a boot sector image).
 * serialno  - a serial number to stamp into the boot sector. If serialno' is -1,
 *             the boot sector's serial number is not changed. If 'serialno'
 *             is greater than or equal to $01000000, a random serial number
 *             is generated and placed in the boot sector.
 * disktype  - is either -1 (to leave the disk type information alone) or one
 *             of 0,1,2,3
 * execflag  - if 1, the boot sector is made executable. If 'execflag' is 0,
 *             the boot sector is made non-executable. If 'execflag' is -1,
 *             the boot sector remains executable or non-executable depending
 *             on the way it was originally.
 */

VOID xbios_12(LONG buf, LONG serialno, WORD disktype, WORD execflag)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x12 ...\n");
#endif
}



/*
 * xbios_13 - (flopver) Verify sectors from a floppy disk.
 */

WORD xbios_13(LONG buf, LONG filler, WORD devno, WORD sectno,
              WORD trackno, WORD sideno, WORD count)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x13 ...\n");
#endif
    return(-1);
}



/*
 * xbios_14 - (scrdmp) Dump screen to printer.
 */

VOID xbios_14()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x14 ...\n");
#endif
}



/*
 * xbios_15 - (cursconf) Configure the cursor
 */

WORD xbios_15(WORD function, WORD operand)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x15 ...\n");
#endif
    return(0);
}



/*
 * xbios_16 - (settime) Sets the intelligent keyboard's the time and date.
 *
 * 'datetime' is a 32-bit DOS-format date and time (time in the low word,
 * date in the high word).
 */

VOID xbios_16(LONG datetime)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x16 ...\n");
#endif
}



/*
 * xbios_17 - (gettime) Gets intelligent keyboard's time and date
 *
 * Returns that value (in DOS format) as a 32-bit word.
 * (Time in the low word, date in the high word).
 */

LONG xbios_17()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x17 ...\n");
#endif
    return(0);
}



/*
 * xbios_18 - (bioskeys) Restores powerup settings of keyboard
 *
 * Restores powerup settings of keyboard translation tables.
 */

VOID xbios_18()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x18 ...\n");
#endif
}



/*
 * xbios_19 - (ikbdws) Writes a string to the intelligent keyboard.
 */

VOID xbios_19(WORD cnt, LONG ptr)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x19 ...\n");
#endif
}



/*
 * xbios_1a - (jdisint) Disable interrupt number 'intno' on the 68901
 */

VOID xbios_1a(WORD intno)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x1a ...\n");
#endif
}



/*
 * xbios_1b - (jenabint) Enable interrupt number 'intno' on the 68901.
 */

VOID xbios_1b(WORD intno)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x1b ...\n");
#endif
}



/*
 * xbios_1c - (giaccess) Read or write a register on the sound chip
 */

BYTE xbios_1c(BYTE data, WORD regno)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x1c ...\n");
#endif
    return(0);
}



/*
 * xbios_1d - (offgibit) Atomically set a bit in the PORT A register to zero.
 */

VOID xbios_1d(WORD bitno)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x1d ...\n");
#endif
}



/*
 * xbios_1e - (ongibit) Atomically set a bit in the PORT A register to one
 */

VOID xbios_1e(WORD bitno)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x1e ...\n");
#endif
}



/*
 * xbios_1f - (xbtimer)
 */

VOID xbios_1f(WORD timer, WORD control, WORD data, LONG vec)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x1f ...\n");
#endif
}



/*
 * xbios_20 - (dosound) Set sound daemon's "program counter" to 'ptr'.
 *
 * 'ptr' points to a set of commands organized as bytes.
 */

VOID xbios_20(LONG ptr)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x20 ...\n");
#endif
}



/*
 * xbios_21 - (setprt) Set/get printer configuration byte.
 *
 * If 'config' is -1 ($FFFF) return the current printer configuration
 * byte. Otherwise set the byte and return it's old value.
 */

WORD xbios_21(WORD config)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x21 ...\n");
#endif
    return(0);
}



/*
 * xbios_22 - (kbdvbase) Returns pointer to a kbdvecs structure
 *
 */

LONG xbios_22()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x22 ...\n");
#endif
    return(0);
}



/*
 * xbios_23 - (kbrate) Get/set the keyboard's repeat rate
 */

WORD xbios_23(WORD initial, WORD repeat)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x23 ...\n");
#endif
    return(0);
}



/*
 * xbios_24 - (prtblk) Prtblk() primitive
 */

VOID xbios_24()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x24 ...\n");
#endif
}



/*
 * xbios_25 - (vsync) Waits until the next vertical-blank interrupt
 */

VOID xbios_25()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x25 ...\n");
#endif
}



/*
 * xbios_26 - (supexec)
 *
 * 'codeptr' points to a piece of code, ending in an RTS, that is
 * executed in supervisor mode. The idential code cannot perform
 * BIOS or GEMDOS calls. This function is meant to allow programs
 * to hack hardware and protected locations without having to fiddle
 * with GEMDOS get/set supervisor mode call.
 */

VOID xbios_26(LONG codeptr)
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x26 ...\n");
#endif
}



/*
 * xbios_27 - (puntaes) Throws away the AES, freeing up any memory it used.
 *
 * If the AES is still resident, it will be discarded and the system
 * will reboot. If the AES is not resident (if it was discarded earlier)
 * the function will return.
 */

VOID xbios_27()
{
#if DBG_XBIOS
    kprint("XBIOS: Unimplemented function 0x27 ...\n");
#endif
}

