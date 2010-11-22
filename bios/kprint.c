/*
 * kprint.c - our own printf variants (mostly for debug purposes)
 *
 * Copyright (c) 2001-2008 The EmuTOS Development Team
 *
 * Authors:
 *  MAD     Martin Doering
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "bios.h"
#include "kprint.h"
#include "lineavars.h"
#include "vt52.h"
#include "tosvars.h"
#include "natfeat.h"
#include "config.h"
#include "processor.h"
#include "chardev.h"

/* extern declarations */

extern void printout_stonx(char *);    /* in kprintasm.S */


/* doprintf implemented in doprintf.c. 
 * This is an OLD one, and does not support floating point 
 */
#include <stdarg.h>
extern int doprintf(void (*outc)(int), const char *fmt, va_list ap);


/*
 *  globals
 */

/* this variable is filled by function kprint_init(), in kprintasm.S, 
 * called very early just after clearing the BSS.
 */
 
int native_print_kind;


/*==== cprintf - do formatted string output direct to the console ======*/

static void cprintf_outc(int c)
{
    /* add a CR to Unix LF for VT52 convenience */
    if ( c == '\n')
        bconout2(2,'\r');

    bconout2(2,c);
}

static int vcprintf(const char *fmt, va_list ap)
{
    return doprintf(cprintf_outc, fmt, ap);
}

int cprintf(const char *fmt, ...)
{
    int n;
    va_list ap;
    va_start(ap, fmt);
    n = vcprintf(fmt, ap);
    va_end(ap);
    return n;
}


/*==== kprintf - do formatted ouput natively to the emulator ======*/

#if MIDI_DEBUG_PRINT
static void kprintf_outc_midi(int c)
{
    bconout3(3,c);
}
#endif

#if RS232_DEBUG_PRINT
static void kprintf_outc_rs232(int c)
{
    bconout1(1, c);
}
#endif

#if DETECT_NATIVE_FEATURES
static void kprintf_outc_natfeat(int c)
{
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    nfStdErr(buf);
}
#endif

#if STONX_NATIVE_PRINT || DETECT_NATIVE_PRINT
static void kprintf_outc_stonx(int c)
{
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    printout_stonx(buf);
}
#endif


static int vkprintf(const char *fmt, va_list ap)
{
#if DETECT_NATIVE_FEATURES
    if (is_nfStdErr()) {
        return doprintf(kprintf_outc_natfeat, fmt, ap);
    }
#endif

#if STONX_NATIVE_PRINT || DETECT_NATIVE_PRINT
    if (native_print_kind) {
        return doprintf(kprintf_outc_stonx, fmt, ap);
    } 
#endif

#if MIDI_DEBUG_PRINT
    /* use midi port instead of other native debug capabilities */
    return doprintf(kprintf_outc_midi, fmt, ap);
#endif

#if RS232_DEBUG_PRINT
    return doprintf(kprintf_outc_rs232, fmt, ap);
#endif

    /* let us hope nobody is doing 'pretty-print' with kprintf by
     * printing stuff till the amount of characters equals something,
     * for it will generate an endless loop if return value is zero!
     */
    return 0;
}


int kprintf(const char *fmt, ...)
{
    int n;
    va_list ap;
    va_start(ap, fmt);
    n = vkprintf(fmt, ap);
    va_end(ap);
    return n;
}

/*==== kcprintf - do both cprintf and kprintf ======*/

static int vkcprintf(const char *fmt, va_list ap)
{
    if(vt52_initialized) {
        vkprintf(fmt, ap);
        return vcprintf(fmt, ap);
    } else {
        return vkprintf(fmt, ap);
    }
}

int kcprintf(const char *fmt, ...)
{
    int n;
    va_list ap;
    va_start(ap, fmt);
    n = vkcprintf(fmt, ap);
    va_end(ap);
    return n;
}

/*==== doassert ======*/

void doassert(const char *file, long line, const char *func, const char *text)
{
    kprintf("assert failed in %s:%ld (function %s): %s\n", file, line, func, text);
}

/*==== dopanic - display information found in 0x380 and halt ======*/


static const char *exc_messages[] = {
    "", "",
#ifdef __mcoldfire__
    "access error",
#else
    "bus error",
#endif
    "address error",
    "illegal exception", "divide by zero",
    "datatype overflow (CHK)",
    "trapv overflow bit error",
    "privilege violation", "Trace", "LineA", "LineF"
};

#define numberof(a) (sizeof(a)/sizeof(*a))

void dopanic(const char *fmt, ...)
{
    /* wrap the cursor */
    cprintf("\033v\r\n");
    /* TODO use sane screen settings (color, address) */
    
    if (proc_lives != 0x12345678) {
        kcprintf("No saved info in dopanic; halted.\n");
        halt();
    }
    if (proc_enum == 0) {
        va_list ap;
        va_start(ap, fmt);
        vkcprintf(fmt, ap);
        va_end(ap);
#ifdef __mcoldfire__
    } else {
        /* On ColdFire, the exception frame is the same for all exceptions. */
        struct {
            WORD fv; /* Format/Vector Word */
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;

        if (proc_enum >= 2 && proc_enum < numberof(exc_messages)) {
            kcprintf("Panic: %s. fv = 0x%04x, sr = 0x%04x, pc = 0x%08lx\n",
                     exc_messages[proc_enum], s->fv, s->sr, s->pc);
        } else {
            kcprintf("Panic: Exception number %d. fv = 0x%04x, sr = 0x%04x, pc = 0x%08lx\n",
                    (int) proc_enum, s->fv, s->sr, s->pc);
        }
        kcprintf("       format = %d, vector = %d, fault = %d\n",
            (s->fv & 0xf000) >> 12,
            (s->fv & 0x03fc) >> 2,
            (s->fv & 0x0c00) >> 8 | (s->fv & 0x0002));
    }
#else
    } else if (proc_enum == 2 || proc_enum == 3) {
        struct {
            WORD misc;
            LONG address;
            WORD opcode;
            WORD sr;
            LONG pc;
        } *s000 = (void *)proc_stk;

        struct {
            WORD sr;
            LONG pc;
            WORD format_word;
            LONG effective_address;
            WORD misc;
            WORD wb3s, wb2s, wb1s;
            LONG address;
        } *s040 = (void *)proc_stk;

        WORD misc, sr, opcode;
        LONG address, pc;

        switch(mcpu) {
            case 40:
               misc = s040->misc;
               sr = s040->sr;
               address = s040->address;
               pc = s040->pc;
               /* we could read the opcode from *pc, like below
               opcode = *(WORD *)pc;
               but it would not work on real 68040 most probably due to 
               instruction pre-fetch and could also cause a recursive death
               if the pc pointed to an illegal memory location */
               opcode = 0x0000;
               break;

            default:
               misc = s000->misc;
               sr = s000->sr;
               opcode = s000->opcode;
               pc = s000->pc;
               address = s000->address;
        }
        kcprintf("Panic: %s. misc = 0x%04x, address = 0x%08lx\n",
                 exc_messages[proc_enum], misc, address);
        if (mcpu == 40) {
            kcprintf("sr = 0x%04x, pc = 0x%08lx\n", sr, pc);
        }
        else {
            kcprintf("opcode = 0x%04x, sr = 0x%04x, pc = 0x%08lx\n",
                     opcode, sr, pc);
        }
    } else if (proc_enum >= 4 && proc_enum < numberof(exc_messages)) {
        struct {
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;
        kcprintf("Panic: %s. sr = 0x%04x, pc = 0x%08lx\n",
                 exc_messages[proc_enum], s->sr, s->pc);
    } else {
        struct {
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;
        kcprintf("Panic: Exception number %d. sr = 0x%04x, pc = 0x%08lx\n",
                (int) proc_enum, s->sr, s->pc);
    }
#endif
    kcprintf("Aregs: %08lx %08lx %08lx %08lx  %08lx %08lx %08lx %08lx\n",
             proc_aregs[0], proc_aregs[1], proc_aregs[2], proc_aregs[3], 
             proc_aregs[4], proc_aregs[5], proc_aregs[6], proc_aregs[7]);
    kcprintf("Dregs: %08lx %08lx %08lx %08lx  %08lx %08lx %08lx %08lx\n",
             proc_dregs[0], proc_dregs[1], proc_dregs[2], proc_dregs[3], 
             proc_dregs[4], proc_dregs[5], proc_dregs[6], proc_dregs[7]);
    kcprintf("Processor halted.\n");
    halt();
}

