/*
 * xbios.c - C portion of XBIOS initialization and front end
 *
 * Copyright (c) 2001-2015 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *  THH     Thomas Huth
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "config.h"
#include "portab.h"
#include "kprint.h"
#include "iorec.h"
#include "tosvars.h"
#include "lineavars.h"
#include "vt52.h"
#include "ikbd.h"
#include "midi.h"
#include "mfp.h"
#include "serport.h"
#include "machine.h"
#include "screen.h"
#include "videl.h"
#include "sound.h"
#include "dmasound.h"
#include "floppy.h"
#include "disk.h"
#include "clock.h"
#include "nvram.h"
#include "mouse.h"
#include "asm.h"
#include "vectors.h"
#include "xbios.h"

#define DBG_XBIOS        0

/*
 * xbios_0 - (initmous) Initialize mouse packet handler
 */

#if DBG_XBIOS
static void xbios_0(WORD type, struct param * param, PFVOID vec)
{
    kprintf("XBIOS: Initmous\n");
    Initmous(type, param, vec);
}
#endif



/*
 * xbios_1 - (ssbrk) Reserve 'amount' bytes from the top of memory.
 *
 * Returns a long pointing to the base of the allocated memory. This function
 * MUST be called before the OS is initialized.
 * ssbrk() is actually pretty useless. It DOES NOT work after GEMDOS has been
 * brought up, since the TPA has already been set up
 */

/* unimplemented */


/*
 * xbios_2 - (physBase) Get the screen's physical base address
 *
 * (at the beginning of the next vblank).
 */

#if DBG_XBIOS
static ULONG xbios_2(void)
{
    kprintf("XBIOS: Physbase ...\n");
    return physbase();
}
#endif



/*
 * xbios_3 - (logBase) Get the screen's logical base, right away.
 *
 * This is the location that GSX uses when drawing to the screen.
 */

#if DBG_XBIOS
static LONG xbios_3(void)
{
    kprintf("XBIOS: Logbase ...\n");
    return logbase();
}
#endif



/*
 * xbios_4 - (getRez) Get the screen's current resolution
 *
 * Returns 0, 1 or 2.
 */

#if DBG_XBIOS
static WORD xbios_4(void)
{
    kprintf("XBIOS: Getrez ...\n");
    return getrez();
}
#endif



/*
 * xbios_5 - (setScreen) Set the screen locations
 *
 * Set the logical screen location (logLoc), the physical screen location
 * (physLoc), and the physical screen resolution. Negative parameters are
 * ignored (making it possible, for instance, to set screen resolution without
 * changing anything else). When resolution is changed, the screen is cleared,
 * the cursor is homed, and the VT52 terminal emulator state is reset.
 */

#if DBG_XBIOS
static void xbios_5(LONG logLoc, LONG physLoc, WORD rez, WORD videlmode)
{
    kprintf("XBIOS: SetScreen(log = 0x%08lx, phys = 0x%08lx, rez = 0x%04x)\n",
           logLoc, physLoc, rez);
    setscreen(logLoc, physLoc, rez, videlmode);
}
#endif



/*
 * xbios_6 - (setPalette) Set the contents of the hardware palette register
 *
 * (all 16 color entries) from the 16 words pointed to by 'palettePtr'.
 * 'paletteptr' MUST be on a word boundary. The palette assignment takes
 * place at the beginning of the next vertical blank interrupt.
 */

#if DBG_XBIOS
static void xbios_6(LONG palettePtr)
{
    kprintf("XBIOS: SetPalette(0x%08lx)\n", palettePtr);
    setpalette(palettePtr);
}
#endif



/*
 * xbios_7 - (setColor) Set the palette number
 *
 * Set the palette number 'colorNum' in the hardware palette table to the
 * specified 'color'. If 'color' is negative, the hardware register is not
 * changed.
 *
 * Return the old color.
 */

#if DBG_XBIOS
static WORD xbios_7(WORD colorNum, WORD color)
{
    kprintf("XBIOS: Setcolor(0x%04x, 0x%04x)\n", colorNum, color);
    return setcolor(colorNum, color);
}
#endif



/*
 * xbios_8 - (floprd) Read one or more sectors from a floppy disk.
 *
 * buf      - must point to a word-aligned buffer large enough to contain the
 *            number of sectors requested.
 * filler   - an unused longword.
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

#if DBG_XBIOS
static LONG xbios_8(LONG buf, LONG filler, WORD devno, WORD sectno,
                    WORD trackno, WORD sideno, WORD count)
{
    kprintf("XBIOS: Floprd()\n");
    return floprd(buf, filler, devno, sectno, trackno, sideno, count);
}
#endif



/*
 * xbios_9 - (flopwr) Write one or more sectors to a floppy disk.
 *
 * (same parameters and return value as floprd)
 *
 * Writing to the boot sector (sector 1, side 0, track 0) will cause the
 * media to enter the "might have changed" state. This will be reflected
 * on the next rwabs() or mediach() BIOS call.
 *
 */


#if DBG_XBIOS
static LONG xbios_9(LONG buf, LONG filler, WORD devno, WORD sectno,
                    WORD trackno, WORD sideno, WORD count)
{
    kprintf("XBIOS: Flopwr()\n");
    return flopwr(buf, filler, devno, sectno, trackno, sideno, count);
}
#endif



/*
 * xbios_a - (flopfmt) Format a track on a floppy disk
 *
 * buf      - must point to a word-aligned buffer large enough to contain
 *            a write track image.  Atari documentation says this is 8K
 *            for a DD diskette, but the minimum values currently required
 *            are 6250 bytes for DD and 12500 bytes for HD diskettes.
 * skew     - if 'interlv' is negative, points to an array of WORDs with
 *            'spt' entries, containing sector numbers in the sequence
 *            in which they should be created on the track.  if 'interlv'
 *            is not negative, this is ignored.
 * devno    - the floppy number (0 or 1).
 * spt      - the number of sectors-per-track to format: standard values
 *            are 9 for DD and 18 for HD.  values of 1-9 imply formatting
 *            as DD; values of 13-20 as HD.  other values return an error.
 * trackno  - the track number to format (usually 0 to 79).
 * sideno   - the side number to format (0 or 1).
 * interlv  - the sector-interleave factor.  if it is negative, the 'skew'
 *            array is used to generate sector numbers; otherwise a value
 *            of 1 is used for interleave (i.e. no interleave).
 * magic    - a magic number that MUST be the value $87654321.
 * virgin   - a word fill value for new sectors, normally 0xe5e5.  other
 *            values may be used, but the high nybble of each byte must
 *            *not* be $f (these values have special meanings to the FDC).
 *
 * Returns a status code:
 *  zero    - the operation succeeded.
 *  nonzero - the operation failed.  if the status code is EBADSF, a
 *            null-terminated WORD array of bad sector numbers is also
 *            returned in the buffer.
 */

#if DBG_XBIOS
static LONG xbios_a(LONG buf, WORD *skew, WORD devno, WORD spt,
                    WORD trackno, WORD sideno, WORD interlv, WORD virgin,
                    LONG magic)
{
    kprintf("XBIOS: flopfmt()\n");
    return flopfmt(buf, skew, devno, spt, trackno, sideno, interlv,
                   virgin, magic);
}
#endif



/*
 * xbios_b - Used by BIOS - Obsolete function
 */

/* unimplemented */


/*
 * xbios_c - (midiws) Writes a string to the MIDI port.
 *
 * cnt   - the number of characters to write, minus one.
 * ptr   - points to a vector of characters to write.
 */

#if DBG_XBIOS
static void xbios_c(WORD cnt, LONG ptr)
{
    kprintf("XBIOS: Midiws(0x%04x, 0x%08lx)\n", cnt, ptr);
    midiws(cnt, ptr);
}
#endif


#if CONF_WITH_MFP

/*
 * xbios_d - (mfpint) Set the MFP interrupt number
 *
 * Set the MFP interrupt number 'interno' (0 to 15) to 'vector'.
 * The old vector is written over (and thus unrecoverable).
 */


#if DBG_XBIOS
static void xbios_d(WORD interno, LONG vector)
{
    kprintf("XBIOS: Mfpint(0x%x, 0x%08lx)\n", interno, vector);
    mfpint(interno, vector);
}
#endif

#endif /* CONF_WITH_MFP */


/*
 * xbios_e - (iorec) Returns pointer to a serial device's input buffer record.
 */

static LONG iorec(WORD devno)
{
    switch(devno) {
    case 0:
        return (LONG) rs232iorecptr;
    case 1:
        return (LONG) &ikbdiorec;
    case 2:
        return (LONG) &midiiorec;
    default:
        return -1;
    }
}

#if DBG_XBIOS
static LONG xbios_e(WORD devno)
{
    LONG ret;
    kprintf("XBIOS: Iorec(%d)\n", devno);
    ret = iorec(devno);
    if(ret == -1) {
        kprintf("Iorec(%d) : bad input device\n", devno);
    }
    return ret;
}
#endif

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
static ULONG rsconf(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    return (*rsconfptr)(baud, ctrl, ucr, rsr, tsr, scr);
}

#if DBG_XBIOS
static ULONG xbios_f(WORD speed, WORD flowctl, WORD ucr, WORD rsr, WORD tsr, WORD scr)
{
    kprintf("XBIOS: Rsconf(...)\n");
    return rsconf(speed, flowctl, ucr, rsr, tsr, scr);
}
#endif



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

#if DBG_XBIOS
static LONG xbios_10(UBYTE* unshift, UBYTE* shift, UBYTE* capslock)
{
    kprintf("XBIOS: Keytbl(0x%08lx, 0x%08lx, 0x%08lx)\n",
            (LONG)unshift, (LONG)shift, (LONG)capslock);
    return keytbl(unshift, shift, capslock);
}
#endif



/*
 * xbios_11 - (random) Returns a 24-bit pseudo-random number
 *
 * Bits 24..31 will be zero.
 */

static ULONG rseed;

static LONG random(void)
{
    if(rseed == 0) {
        rseed = hz_200 << 16;
        rseed += hz_200;
    }
    rseed *= 3141592621UL;
    rseed ++;
    return((rseed >> 8) & 0xFFFFFF);
}

#if DBG_XBIOS
static LONG xbios_11(void)
{
    kprintf("XBIOS: Random()\n");
    return random();
}
#endif


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

#if DBG_XBIOS
static void xbios_12(LONG buf, LONG serialno, WORD disktype, WORD execflag)
{
    kprintf("XBIOS: Protobt()\n");
    protobt(buf, serialno, disktype, execflag);
}
#endif



/*
 * xbios_13 - (flopver) Verify sectors from a floppy disk.
 */

#if DBG_XBIOS
static LONG xbios_13(LONG buf, LONG filler, WORD devno, WORD sectno,
                     WORD trackno, WORD sideno, WORD count)
{
    kprintf("XBIOS: Flopver()\n");
    return flopver(buf, filler, devno, sectno, trackno, sideno, count);
}
#endif



/*
 * xbios_14 - (scrdmp) Dump screen to printer.
 */

/* unimplemented */



/*
 * xbios_15 - (cursconf) Configure the cursor
 */

#if DBG_XBIOS
static WORD xbios_15(WORD function, WORD operand)
{
    kprintf("XBIOS: Cursconf( 0x%04x, 0x%04x )\n", function, operand);
    return cursconf(function, operand);
}
#endif



/*
 * xbios_16 - (settime) Sets the intelligent keyboard's the time and date.
 *
 * 'datetime' is a 32-bit DOS-format date and time (time in the low word,
 * date in the high word).
 */

#if DBG_XBIOS
static void xbios_16(ULONG datetime)
{
    kprintf("XBIOS: Settime()\n");
    settime(datetime);
}
#endif



/*
 * xbios_17 - (gettime) Gets intelligent keyboard's time and date
 *
 * Returns that value (in DOS format) as a 32-bit word.
 * (Time in the low word, date in the high word).
 */

#if DBG_XBIOS
static ULONG xbios_17(void)
{
    kprintf("XBIOS: Gettime()\n");
    return gettime();
}
#endif



/*
 * xbios_18 - (bioskeys) Restores powerup settings of keyboard
 *
 * Restores powerup settings of keyboard translation tables.
 */

#if DBG_XBIOS
static void xbios_18(void)
{
    kprintf("XBIOS: Bioskeys()\n");
    bioskeys();
}
#endif



/*
 * xbios_19 - (ikbdws) Writes a string to the intelligent keyboard.
 */

#if DBG_XBIOS
static void xbios_19(WORD cnt, LONG ptr)
{
    kprintf("XBIOS: Ikbdws(0x%04x, 0x%08lx)\n", cnt, ptr);
    ikbdws(cnt, (PTR) ptr);
}
#endif


#if CONF_WITH_MFP

/*
 * xbios_1a - (jdisint) Disable interrupt number 'intno' on the 68901
 */

#if DBG_XBIOS
static void xbios_1a(WORD intno)
{
    kprintf("XBIOS: Jdisint(0x%x)\n", intno);
    jdisint(intno);
}
#endif



/*
 * xbios_1b - (jenabint) Enable interrupt number 'intno' on the 68901.
 */

#if DBG_XBIOS
static void xbios_1b(WORD intno)
{
    kprintf("XBIOS: Jenabint(0x%x)\n", intno);
    jenabint(intno);
}
#endif

#endif /* CONF_WITH_MFP */


/*
 * xbios_1c - (giaccess) Read or write a register on the sound chip
 */

#if DBG_XBIOS
static BYTE xbios_1c(BYTE data, WORD regno)
{
    kprintf("XBIOS: Giaccess()\n");
    return giaccess(data, regno);
}
#endif



/*
 * xbios_1d - (offgibit) Atomically set a bit in the PORT A register to zero.
 */

#if DBG_XBIOS
static void xbios_1d(WORD bitno)
{
    kprintf("XBIOS: Offgibit(%d)\n", bitno);
    offgibit(bitno);
}
#endif



/*
 * xbios_1e - (ongibit) Atomically set a bit in the PORT A register to one
 */

#if DBG_XBIOS
static void xbios_1e(WORD bitno)
{
    kprintf("XBIOS: Ongibit(%d)\n", bitno);
    ongibit(bitno);
}
#endif


#if CONF_WITH_MFP

/*
 * xbios_1f - (xbtimer)
 */

#if DBG_XBIOS
static void xbios_1f(WORD timer, WORD control, WORD data, LONG vec)
{
    kprintf("XBIOS: xbtimer(%d, 0x%02x, 0x%02x, 0x%08lx)\n",
            timer, control, data, vec);
    xbtimer(timer, control, data, vec);
}
#endif

#endif /* CONF_WITH_MFP */


/*
 * xbios_20 - (dosound) Set sound daemon's "program counter" to 'ptr'.
 *
 * 'ptr' points to a set of commands organized as bytes.
 */

#if DBG_XBIOS
static void xbios_20(LONG ptr)
{
    kprintf("XBIOS: Dosound()\n");
    dosound(ptr);
}
#endif



/*
 * xbios_21 - (setprt) Set/get printer configuration byte.
 *
 * If 'config' is -1 ($FFFF) return the current printer configuration
 * byte. Otherwise set the byte and return it's old value.
 */

/* unimplemented */


/*
 * xbios_22 - (kbdvbase) Returns pointer to a kbdvecs structure
 *
 */

static LONG kbdvbase(void)
{
    return (LONG) &kbdvecs;
}

#if DBG_XBIOS
static LONG xbios_22(void)
{
    kprintf("XBIOS: Kbdvbase()\n");
    return kbdvbase();
}
#endif



/*
 * xbios_23 - (kbrate) Get/set the keyboard's repeat rate
 */

#if DBG_XBIOS
static WORD xbios_23(WORD initial, WORD repeat)
{
    kprintf("XBIOS: kbrate(%d, %d)\n", initial, repeat);
    return kbrate(initial, repeat);
}
#endif



/*
 * xbios_24 - (prtblk) Prtblk() primitive
 */

/* unimplemented */


/*
 * xbios_25 - (vsync) Waits until the next vertical-blank interrupt
 */

#if DBG_XBIOS
static void xbios_25(void)
{
    kprintf("XBIOS: Vsync()\n");
    vsync();
}
#endif



/*
 * xbios_26 - (supexec)
 *
 * 'codeptr' points to a piece of code, ending in an RTS, that is
 * executed in supervisor mode. The idential code cannot perform
 * BIOS or GEMDOS calls. This function is meant to allow programs
 * to hack hardware and protected locations without having to fiddle
 * with GEMDOS get/set supervisor mode call.
 */

static LONG supexec(LONG codeptr)
{
    return ((LONG(*)(void))codeptr)();
}

#if DBG_XBIOS
static LONG xbios_26(LONG codeptr)
{
    kprintf("XBIOS: Supexec(0x%08lx)\n", codeptr);
    return supexec(codeptr);
}
#endif



/*
 * xbios_27 - (puntaes) Throws away the AES, freeing up any memory it used.
 *
 * If the AES is still resident, it will be discarded and the system
 * will reboot. If the AES is not resident (if it was discarded earlier)
 * the function will return.
 */

/* unimplemented */

/*
 * xbios_29 - (Floprate)
 */

#if DBG_XBIOS
static LONG xbios_29(WORD dev, WORD rate)
{
    kprintf("XBIOS: Floprate\n");
    return floprate(dev, rate);
}
#endif

/*
 * xbios_2a - (DMAread)
 */

#if DBG_XBIOS
static LONG xbios_2a(LONG sector, WORD count, PTR buf, WORD major)
{
    kprintf("XBIOS: DMAread\n");
    return DMAread(sector, count, buf, major);
}
#endif

/*
 * xbios_2b - (DMAwrite)
 */

#if DBG_XBIOS
static LONG xbios_2b(LONG sector, WORD count, PTR buf, WORD major)
{
    kprintf("XBIOS: DMAwrite\n");
    return DMAwrite(sector, count, buf, major);
}
#endif

/*
 * xbios_2c - (Bconmap)
 */

#if DBG_XBIOS
static LONG xbios_2c(WORD devno)
{
    kprintf("XBIOS: Bconmap\n");
    return bconmap(devno);
}
#endif



/*
 * xbios_2e - (NVMaccess)
 */

#if DBG_XBIOS && CONF_WITH_NVRAM
static WORD xbios_2e(WORD op, WORD start, WORD count, PTR buffer)
{
    kprintf("XBIOS: NVMaccess\n");
    return nvmaccess(op, start, count, buffer);
}
#endif


/*
 * xbios_40 - (Blitmode)
 *
 * this is a minimalist implementation: we ignore any requests to use
 * the blitter, we just return hardware status
 */
#if CONF_WITH_BLITTER
static WORD blitmode(WORD mode)
{
    return has_blitter ? 0x0002 : 0x0000;
}

#if DBG_XBIOS
static WORD xbios_40(WORD mode)
{
    kprintf("XBIOS: Blitmode\n");
    return blitmode(mode);
}
#endif
#endif

/*
 * TT video
 */
#if DBG_XBIOS
static WORD xbios_50(WORD mode)
{
    kprintf("XBIOS: EsetShift\n");
    return esetshift(mode);
}
#endif

#if DBG_XBIOS
static WORD xbios_51(void)
{
    kprintf("XBIOS: EgetShift\n");
    return egetshift();
}
#endif

#if DBG_XBIOS
static WORD xbios_52(WORD bank)
{
    kprintf("XBIOS: EsetBank\n");
    return esetbank(bank);
}
#endif

#if DBG_XBIOS
static WORD xbios_53(WORD index, WORD color)
{
    kprintf("XBIOS: EsetColor\n");
    return esetcolor(index, color);
}
#endif

#if DBG_XBIOS
static void xbios_54(WORD index,WORD count,WORD *rgb)
{
    kprintf("XBIOS: EsetPalette\n");
    esetpalette(index, count, rgb);
}
#endif

#if DBG_XBIOS
static void xbios_55(WORD index, WORD count, WORD *rgb)
{
    kprintf("XBIOS: EgetPalette\n");
    egetpalette(index, count, rgb);
}
#endif

#if DBG_XBIOS
static WORD xbios_56(WORD mode)
{
    kprintf("XBIOS: EsetGray\n");
    return esetgray(mode);
}
#endif

#if DBG_XBIOS
static WORD xbios_57(WORD mode)
{
    kprintf("XBIOS: EsetSmear\n");
    return esetsmear(mode);
}
#endif

/*
 * Falcon video
 */
#if DBG_XBIOS & CONF_WITH_VIDEL
static WORD xbios_58(WORD mode)
{
    kprintf("XBIOS: Vsetmode\n");
    return vsetmode(mode);
}
static WORD xbios_59(void)
{
    kprintf("XBIOS: VgetMonitor\n");
    return vmontype();
}
static void xbios_5a(WORD external)
{
    kprintf("XBIOS: VsetSync\n");
    vsetsync(external);
}
static LONG xbios_5b(WORD mode)
{
    kprintf("XBIOS: VgetSize\n");
    return vgetsize(mode);
}
static void xbios_5d(WORD index,WORD count,LONG *rgb)
{
    kprintf("XBIOS: VsetRGB\n");
    vsetrgb(index,count,rgb);
}
static void xbios_5e(WORD index,WORD count,LONG *rgb)
{
    kprintf("XBIOS: VgetRGB\n");
    vgetrgb(index,count,rgb);
}
#endif

#if DBG_XBIOS & CONF_WITH_DMASOUND
static LONG xbios_80(void)
{
    kprintf("XBIOS: locksnd\n");
    return locksnd();
}
static LONG xbios_81(void)
{
     kprintf("XBIOS: unlocksnd\n");
   return unlocksnd();
}
static LONG xbios_82(WORD mode, WORD data)
{
    kprintf("XBIOS: soundcmd\n");
    return soundcmd(mode, data);
}
static LONG xbios_83(UWORD mode, ULONG startaddr, ULONG endaddr)
{
    kprintf("XBIOS: setbuffer\n");
    return setbuffer(mode, startaddr, endaddr);
}
static LONG xbios_84(UWORD mode)
{
    kprintf("XBIOS: setsndmode\n");
    return setsndmode(mode);
}
static LONG xbios_85(UWORD playtracks, UWORD rectracks)
{
    kprintf("XBIOS: settracks\n");
    return settracks(playtracks, rectracks);
}
static LONG xbios_86(UWORD track)
{
    kprintf("XBIOS: setsndmode\n");
    return setmontracks(track);
}
static LONG xbios_87(UWORD mode, WORD cause)
{
    kprintf("XBIOS: setinterrupt\n");
    return setinterrupt(mode, cause);
}
static LONG xbios_88(WORD mode)
{
    kprintf("XBIOS: buffoper\n");
    return buffoper(mode);
}
static LONG xbios_89(WORD dspxmit, WORD dsprec)
{
    kprintf("XBIOS: dsptristate\n");
    return dsptristate(dspxmit, dsprec);
}
static LONG xbios_8a(UWORD mode, UWORD data)
{
    kprintf("XBIOS: gpio\n");
    return gpio(mode, data);
}
static LONG xbios_8b(WORD source, WORD dest, WORD clk, WORD prescale, WORD protocol)
{
    kprintf("XBIOS: devconnect\n");
    return devconnect(source, dest, clk, prescale, protocol);
}
static LONG xbios_8c(WORD reset)
{
    kprintf("XBIOS: sndstatus\n");
    return sndstatus(reset);
}
static LONG xbios_8d(LONG sptr)
{
    kprintf("XBIOS: buffptr\n");
    return buffptr(sptr);
}

#endif

/*
 * xbios_unimpl
 *
 * ASM function _xbios_unimpl will call xbios_do_unimpl(WORD number);
 * with the function number passed as parameter.
 */

LONG xbios_do_unimpl(WORD number)
{
#if DBG_XBIOS
    kprintf("unimplemented XBIOS function 0x%02x\n", number);
#endif
    return number;
}

extern LONG xbios_unimpl(void);


/*
 * xbios_vecs - the table of xbios command vectors.
 */

/* PFLONG defined in bios/vectors.h */

#if DBG_XBIOS
#define VEC(wrapper, direct) (PFLONG) wrapper
#else
#define VEC(wrapper, direct) (PFLONG) direct
#endif

const PFLONG xbios_vecs[] = {
    VEC(xbios_0, Initmous),
    xbios_unimpl,   /*  1 ssbrk */
    VEC(xbios_2, physbase),
    VEC(xbios_3, logbase),
    VEC(xbios_4, getrez),
    VEC(xbios_5, setscreen),
    VEC(xbios_6, setpalette),
    VEC(xbios_7, setcolor),
    VEC(xbios_8, floprd),
    VEC(xbios_9, flopwr),
    VEC(xbios_a, flopfmt),
    xbios_unimpl,   /*  b used_by_bios */
    VEC(xbios_c, midiws),
#if CONF_WITH_MFP
    VEC(xbios_d, mfpint),
#else
    xbios_unimpl,   /* d */
#endif
    VEC(xbios_e, iorec),
    VEC(xbios_f, rsconf),
    VEC(xbios_10, keytbl),
    VEC(xbios_11, random),
    VEC(xbios_12, protobt),
    VEC(xbios_13, flopver),
    xbios_unimpl,   /* 14 scrdmp */
    VEC(xbios_15, cursconf),
    VEC(xbios_16, settime),
    VEC(xbios_17, gettime),
    VEC(xbios_18, bioskeys),
    VEC(xbios_19, ikbdws),
#if CONF_WITH_MFP
    VEC(xbios_1a, jdisint),
    VEC(xbios_1b, jenabint),
#else
    xbios_unimpl,   /* 1a */
    xbios_unimpl,   /* 1b */
#endif
    VEC(xbios_1c, giaccess),
    VEC(xbios_1d, offgibit),
    VEC(xbios_1e, ongibit),
#if CONF_WITH_MFP
    VEC(xbios_1f, xbtimer),
#else
    xbios_unimpl,   /* 1f */
#endif
    VEC(xbios_20, dosound),
    xbios_unimpl,   /* 21 setprt */
    VEC(xbios_22, kbdvbase),
    VEC(xbios_23, kbrate),
    xbios_unimpl,   /* 24 prtblk */
    VEC(xbios_25, vsync),
    VEC(xbios_26, supexec),
    xbios_unimpl,   /* 27 puntaes */
    xbios_unimpl,   /* 28 */
    VEC(xbios_29, floprate),
    VEC(xbios_2a, DMAread),
    VEC(xbios_2b, DMAwrite),
    VEC(xbios_2c, bconmap),
    xbios_unimpl,   /* 2d */
#if CONF_WITH_NVRAM
    VEC(xbios_2e, nvmaccess),  /* 2e */
#else
    xbios_unimpl,   /* 2e */
#endif
    xbios_unimpl,   /* 2f */
    xbios_unimpl,   /* 30 */
    xbios_unimpl,   /* 31 */
    xbios_unimpl,   /* 32 */
    xbios_unimpl,   /* 33 */
    xbios_unimpl,   /* 34 */
    xbios_unimpl,   /* 35 */
    xbios_unimpl,   /* 36 */
    xbios_unimpl,   /* 37 */
    xbios_unimpl,   /* 38 */
    xbios_unimpl,   /* 39 */
    xbios_unimpl,   /* 3a */
    xbios_unimpl,   /* 3b */
    xbios_unimpl,   /* 3c */
    xbios_unimpl,   /* 3d */
    xbios_unimpl,   /* 3e */
    xbios_unimpl,   /* 3f */
#if CONF_WITH_BLITTER
    VEC(xbios_40, blitmode),  /* 40 */
#else
    xbios_unimpl,   /* 40 */
#endif
#if TOS_VERSION >= 0x200
    xbios_unimpl,   /* 41 */
    xbios_unimpl,   /* 42 */
    xbios_unimpl,   /* 43 */
    xbios_unimpl,   /* 44 */
    xbios_unimpl,   /* 45 */
    xbios_unimpl,   /* 46 */
    xbios_unimpl,   /* 47 */
    xbios_unimpl,   /* 48 */
    xbios_unimpl,   /* 49 */
    xbios_unimpl,   /* 4a */
    xbios_unimpl,   /* 4b */
    xbios_unimpl,   /* 4c */
    xbios_unimpl,   /* 4d */
    xbios_unimpl,   /* 4e */
    xbios_unimpl,   /* 4f */
#if CONF_WITH_TT_SHIFTER
    VEC(xbios_50, esetshift),   /* 50 */
    VEC(xbios_51, egetshift),   /* 51 */
    VEC(xbios_52, esetbank),    /* 52 */
    VEC(xbios_53, esetcolor),   /* 53 */
    VEC(xbios_54, esetpalette), /* 54 */
    VEC(xbios_55, egetpalette), /* 55 */
    VEC(xbios_56, esetgray),    /* 56 */
    VEC(xbios_57, esetsmear),   /* 57 */
#else
    xbios_unimpl,   /* 50 */
    xbios_unimpl,   /* 51 */
    xbios_unimpl,   /* 52 */
    xbios_unimpl,   /* 53 */
    xbios_unimpl,   /* 54 */
    xbios_unimpl,   /* 55 */
    xbios_unimpl,   /* 56 */
    xbios_unimpl,   /* 57 */
#endif
#if CONF_WITH_VIDEL
    VEC(xbios_58, vsetmode),   /* 58 */
    VEC(xbios_59, vmontype),   /* 59 */
    VEC(xbios_5a, vsetsync),   /* 5a */
    VEC(xbios_5b, vgetsize),   /* 5b */
#else
    xbios_unimpl,   /* 58 */
    xbios_unimpl,   /* 59 */
    xbios_unimpl,   /* 5a */
    xbios_unimpl,   /* 5b */
#endif
    xbios_unimpl,   /* 5c */
#if CONF_WITH_VIDEL
    VEC(xbios_5d, vsetrgb),   /* 5d */
    VEC(xbios_5e, vgetrgb),   /* 5e */
#else
    xbios_unimpl,   /* 5d */
    xbios_unimpl,   /* 5e */
#endif
#if CONF_WITH_DMASOUND
    xbios_unimpl,   /* 5f */
    xbios_unimpl,   /* 60 */
    xbios_unimpl,   /* 61 */
    xbios_unimpl,   /* 62 */
    xbios_unimpl,   /* 63 */
    xbios_unimpl,   /* 64 */
    xbios_unimpl,   /* 65 */
    xbios_unimpl,   /* 66 */
    xbios_unimpl,   /* 67 */
    xbios_unimpl,   /* 68 */
    xbios_unimpl,   /* 69 */
    xbios_unimpl,   /* 6a */
    xbios_unimpl,   /* 6b */
    xbios_unimpl,   /* 6c */
    xbios_unimpl,   /* 6d */
    xbios_unimpl,   /* 6e */
    xbios_unimpl,   /* 6f */
    xbios_unimpl,   /* 70 */
    xbios_unimpl,   /* 71 */
    xbios_unimpl,   /* 72 */
    xbios_unimpl,   /* 73 */
    xbios_unimpl,   /* 74 */
    xbios_unimpl,   /* 75 */
    xbios_unimpl,   /* 76 */
    xbios_unimpl,   /* 77 */
    xbios_unimpl,   /* 78 */
    xbios_unimpl,   /* 79 */
    xbios_unimpl,   /* 7a */
    xbios_unimpl,   /* 7b */
    xbios_unimpl,   /* 7c */
    xbios_unimpl,   /* 7d */
    xbios_unimpl,   /* 7e */
    xbios_unimpl,   /* 7f */
    VEC(xbios_80, locksnd),     /* 80 */
    VEC(xbios_81, unlocksnd),   /* 81 */
    VEC(xbios_82, soundcmd),    /* 82 */
    VEC(xbios_83, setbuffer),   /* 83 */
    VEC(xbios_84, setsndmode),  /* 84 */
    VEC(xbios_85, settracks),   /* 85 */
    VEC(xbios_86, setmontracks), /* 86 */
    VEC(xbios_87, setinterrupt), /* 87 */
    VEC(xbios_88, buffoper),    /* 88 */
    VEC(xbios_89, dsptristate), /* 89 */
    VEC(xbios_8a, gpio),        /* 8a */
    VEC(xbios_8b, devconnect),  /* 8b */
    VEC(xbios_8c, sndstatus),   /* 8c */
    VEC(xbios_8d, buffptr),     /* 8d */
#endif /* CONF_WITH_DMASOUND */

#endif /* TOS_VERSION >= 0x200 */
};

const UWORD xbios_ent = sizeof(xbios_vecs) / sizeof(PFLONG);
