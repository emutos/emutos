/*
 * kprint.c - our own printf variants (mostly for debug purposes)
 *
 * Copyright (c) 2001-2015 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "config.h"
#include <stdarg.h>
#include "doprintf.h"
#include "portab.h"
#include "kprint.h"
#include "nls.h"
#include "lineavars.h"
#include "vt52.h"
#include "conout.h"
#include "tosvars.h"
#include "natfeat.h"
#include "processor.h"
#include "chardev.h"
#include "serport.h"
#include "pd.h"
#include "coldfire.h"
#include "asm.h"
#include "vectors.h"
#include "super.h"      /* for Super() and SuperToUser() */
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

#define DISPLAY_INSTRUCTION_AT_PC   0   /* set to 1 for extra info from dopanic() */

#if STONX_NATIVE_PRINT

/* external declarations from kprintasm.S */

extern void printout_stonx(const char *str);

/* this variable is filled by stonx_kprintf_init() */
int stonx_kprintf_available;

#endif /* STONX_NATIVE_PRINT */

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


/*==== kprintf - do formatted output to the port/emulator ======*/

#if MIDI_DEBUG_PRINT
static void kprintf_outc_midi(int c)
{
    /* Raw terminals usually require CRLF */
    if (c == '\n')
        bconout3(3,'\r');

    bconout3(3,c);
}
#endif

#if RS232_DEBUG_PRINT
static void kprintf_outc_rs232(int c)
{
    /* Raw terminals usually require CRLF */
    if (c == '\n')
        bconout1(1,'\r');

    bconout1(1,c);
}
#endif

#if SCC_DEBUG_PRINT
static void kprintf_outc_sccB(int c)
{
    /* Raw terminals usually require CRLF */
    if (c == '\n')
        bconoutB(1,'\r');

    bconoutB(1,c);
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

#if STONX_NATIVE_PRINT
static void kprintf_outc_stonx(int c)
{
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    printout_stonx(buf);
}
#endif

#if COLDFIRE_DEBUG_PRINT
static void kprintf_outc_coldfire_rs232(int c)
{
    /* Raw terminals usually require CRLF */
    if ( c == '\n')
        coldfire_rs232_write_byte('\r');

    coldfire_rs232_write_byte((char)c);
}
#endif

static int vkprintf(const char *fmt, va_list ap)
{
#if CONF_WITH_UAE
    return doprintf(kprintf_outc_uae, fmt, ap);
#endif

#if DETECT_NATIVE_FEATURES
    if (is_nfStdErr()) {
        return doprintf(kprintf_outc_natfeat, fmt, ap);
    }
#endif

#if STONX_NATIVE_PRINT
    if (stonx_kprintf_available) {
        return doprintf(kprintf_outc_stonx, fmt, ap);
    }
#endif

#if MIDI_DEBUG_PRINT
    /* use midi port instead of other native debug capabilities */
    if (boot_status&MIDI_AVAILABLE) {   /* no MIDI, no message */
        int rc;
        char *stacksave = NULL;

        if (boot_status&DOS_AVAILABLE)  /* if Super() is available, */
            if (!Super(1L))             /* check for user state.    */
                stacksave = (char *)Super(0L);  /* if so, switch to super   */
        rc = doprintf(kprintf_outc_midi, fmt, ap);
        if (stacksave)                  /* if we switched, */
            SuperToUser(stacksave);     /* switch back.    */
        return rc;
    }
#endif

#if RS232_DEBUG_PRINT
    if (boot_status&RS232_AVAILABLE) {  /* no RS232, no message */
        int rc;
        char *stacksave = NULL;

        if (boot_status&DOS_AVAILABLE)  /* if Super() is available, */
            if (!Super(1L))             /* check for user state.    */
                stacksave = (char *)Super(0L);  /* if so, switch to super   */
        rc = doprintf(kprintf_outc_rs232, fmt, ap);
        if (stacksave)                  /* if we switched, */
            SuperToUser(stacksave);     /* switch back.    */
        return rc;
    }
#endif

#if SCC_DEBUG_PRINT
    if (boot_status&SCC_AVAILABLE) {    /* no SCC, no message */
        int rc;
        char *stacksave = NULL;

        if (boot_status&DOS_AVAILABLE)  /* if Super() is available, */
            if (!Super(1L))             /* check for user state.    */
                stacksave = (char *)Super(0L);  /* if so, switch to super   */
        rc = doprintf(kprintf_outc_sccB, fmt, ap);
        if (stacksave)                  /* if we switched, */
            SuperToUser(stacksave);     /* switch back.    */
        return rc;
    }
#endif

#if COLDFIRE_DEBUG_PRINT
    return doprintf(kprintf_outc_coldfire_rs232, fmt, ap);
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
  vkprintf(fmt, ap);
  return vcprintf(fmt, ap);
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

#if CONF_WITH_ASSERT

/*==== doassert ======*/

void doassert(const char *file, long line, const char *func, const char *text)
{
    kprintf("assert failed in %s:%ld (function %s): %s\n", file, line, func, text);
}

#endif /* CONF_WITH_ASSERT */

/*==== dopanic - display information found in 0x380 and halt ======*/


static const char *exc_messages[] = {
    "", /* Reset: Initial SSP */
    "", /* Reset: Initial PC */
#ifdef __mcoldfire__
    "Access Error",
#else
    "Bus Error",
#endif
    "Address Error",
    "Illegal Instruction",
    "Zero Divide",
    "CHK Instruction",
    "TRAPV Instruction",
    "Privilege Violation",
    "Trace",
    "Line A Emulator",
    "Line F Emulator"
};

void dopanic(const char *fmt, ...)
{
    LONG pc = 0;
    BOOL wrap;
    BYTE *start;

    /* hide cursor, new line, new line */
    cprintf("\033f\033v\n\n");
    /* TODO use sane screen settings (color, address) */

    if (proc_lives != 0x12345678) {
        kcprintf("No saved info in dopanic: halted\n");
        halt();
    }
    if (proc_enum == 0) { /* Call to panic(const char *fmt, ...) */
        struct {
            LONG pc;
        } *s = (void *)proc_stk;

        va_list ap;
        va_start(ap, fmt);
        vkcprintf(fmt, ap);
        va_end(ap);

        kcprintf("pc=%08lx\n",
                 s->pc);
#ifdef __mcoldfire__
    } else {
        /* On ColdFire, the exception frame is the same for all exceptions. */
        struct {
            WORD format_word;
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;

        pc = s->pc;

        if (proc_enum >= 2 && proc_enum < ARRAY_SIZE(exc_messages)) {
            kcprintf("Panic: %s\n",
                     exc_messages[proc_enum]);
        } else {
            kcprintf("Panic: Exception number %d\n",
                     (int) proc_enum);
        }

        kcprintf("fw=%04x (fmt=%d vec=%d fault=%d)\n",
                 s->format_word,
                 (s->format_word & 0xf000) >> 12,
                 (s->format_word & 0x03fc) >> 2,
                 (s->format_word & 0x0c00) >> 8 | (s->format_word & 0x0003));
        kcprintf("sr=%04x pc=%08lx\n",
                 s->sr, s->pc);
    }
#else
    } else if (mcpu == 0 && (proc_enum == 2 || proc_enum == 3)) {
        /* 68000 Bus or Address Error */
        struct {
            WORD misc;
            LONG address;
            WORD opcode;
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;

        pc = s->pc;

        kcprintf("Panic: %s\n",
                 exc_messages[proc_enum]);
        kcprintf("misc=%04x opcode=%04x\n",
                 s->misc, s->opcode);
        kcprintf("addr=%08lx sr=%04x pc=%08lx\n",
                 s->address, s->sr, s->pc);
    } else if (mcpu == 10 && (proc_enum == 2 || proc_enum == 3)) {
        /* 68010 Bus or Address Error */
        struct {
            WORD sr;
            LONG pc;
            WORD format_word;
            WORD special_status_word;
            LONG fault_address;
            WORD unused_reserved_1;
            WORD data_output_buffer;
            WORD unused_reserved_2;
            WORD data_input_buffer;
            WORD unused_reserved_3;
            WORD instruction_input_buffer;
            /* ... 29 words in the stack frame, but only 16 ones saved */
        } *s = (void *)proc_stk;

        pc = s->pc;

        kcprintf("Panic: %s\n",
                 exc_messages[proc_enum]);
        kcprintf("fw=%04x ssw=%04x\n",
                 s->format_word, s->special_status_word);
        kcprintf("addr=%08lx sr=%04x pc=%08lx\n",
                 s->fault_address, s->sr, s->pc);
    } else if ((mcpu == 20 || mcpu == 30) && (proc_enum == 2 || proc_enum == 3)) {
        /* 68020/68030 Bus or Address Error */
        struct {
            WORD sr;
            LONG pc;
            WORD format_word;
            WORD internal_register;
            WORD special_status_register;
            WORD instruction_pipe_stage_c;
            WORD instruction_pipe_stage_b;
            LONG data_cycle_fault_address;
            WORD internal_register_1;
            WORD internal_register_2;
            LONG data_output_buffer;
            WORD internal_register_3;
            WORD internal_register_4;
        } *s = (void *)proc_stk;

        pc = s->pc;

        kcprintf("Panic: %s\n",
                 exc_messages[proc_enum]);
        kcprintf("fw=%04x ir=%04x ssr=%04x\n",
                 s->format_word, s->internal_register, s->special_status_register);
        kcprintf("addr=%08lx sr=%04x pc=%08lx\n",
                 s->data_cycle_fault_address, s->sr, s->pc);
    } else if (mcpu == 40 && proc_enum == 2) {
        /* 68040 Bus Error */
        struct {
            WORD sr;
            LONG pc;
            WORD format_word;
            LONG effective_address;
            WORD special_status_word;
            WORD wb3s, wb2s, wb1s;
            LONG fault_address;
            /* ... 30 words in the stack frame, but only 16 ones saved */
        } *s = (void *)proc_stk;

        pc = s->pc;

        kcprintf("Panic: %s\n",
                 exc_messages[proc_enum]);
        kcprintf("fw=%04x ea=%08lx ssw=%04x\n",
                 s->format_word, s->effective_address, s->special_status_word);
        kcprintf("addr=%08lx sr=%04x pc=%08lx\n",
                 s->fault_address, s->sr, s->pc);
    } else if ((mcpu == 40 && proc_enum == 3)
               || (mcpu == 60 && (proc_enum == 2 || proc_enum == 3))) {
        /* 68040 Address Error, or 68060 Bus or Address Error */
        struct {
            WORD sr;
            LONG pc;
            WORD format_word;
            LONG address;
        } *s = (void *)proc_stk;

        pc = s->pc;

        kcprintf("Panic: %s\n",
                 exc_messages[proc_enum]);
        kcprintf("fw=%04x\n",
                 s->format_word);
        kcprintf("addr=%08lx sr=%04x pc=%08lx\n",
                 s->address, s->sr, s->pc);
    } else if (proc_enum < ARRAY_SIZE(exc_messages)) {
        struct {
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;

        pc = s->pc;

        kcprintf("Panic: %s\n",
                 exc_messages[proc_enum]);
        kcprintf("sr=%04x pc=%08lx\n",
                 s->sr, s->pc);
    } else {
        struct {
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;

        pc = s->pc;

        kcprintf("Panic: Exception number %d\n",
                 (int) proc_enum);
        kcprintf("sr=%04x pc=%08lx\n",
                 s->sr, s->pc);
    }
#endif
#if DISPLAY_INSTRUCTION_AT_PC
    /*
     * we optionally display the instruction pointed to by the PC.
     * this is optional because:
     * (a) it is probably only useful for illegal instruction exceptions
     * (b) it could cause a recursive error
     */
    if ((pc&1) == 0)    /* precaution if running on 68000 */
    {
        WORD *instr = (WORD *)pc;

        kcprintf("Instruction at PC=%04x %04x %04x\n",
                 *instr, *(instr+1), *(instr+2));
    }
#endif

    /* improve display in ST Low */
    start = (v_cel_mx == 39) ? "" : " ";
    wrap = v_stat_0 & M_CEOL;       /* remember line wrap status */
    v_stat_0 &= ~M_CEOL;            /*  & disable it             */

    kcprintf("\nD0-3:%s%08lx %08lx %08lx %08lx\n",
             start, proc_dregs[0], proc_dregs[1], proc_dregs[2], proc_dregs[3]);
    kcprintf("D4-7:%s%08lx %08lx %08lx %08lx\n",
             start, proc_dregs[4], proc_dregs[5], proc_dregs[6], proc_dregs[7]);
    kcprintf("A0-3:%s%08lx %08lx %08lx %08lx\n",
             start, proc_aregs[0], proc_aregs[1], proc_aregs[2], proc_aregs[3]);
    kcprintf("A4-7:%s%08lx %08lx %08lx %08lx\n",
             start, proc_aregs[4], proc_aregs[5], proc_aregs[6], proc_aregs[7]);
    kcprintf(" USP:%s%08lx\n\n",
             start,proc_usp);

    if (wrap)
        v_stat_0 |= M_CEOL;         /* restore line wrap status */

    if (run)
    {
        kcprintf("basepage=%08lx\n",
                 (ULONG)run);
        kcprintf("text=%08lx data=%08lx bss=%08lx\n",
                 run->p_tbase, run->p_dbase, run->p_bbase);
        if (pc && pc >= run->p_tbase && pc < run->p_tbase + run->p_tlen)
            kcprintf("Crash at text+%08lx\n", pc - run->p_tbase);
    }

    /* allow interrupts so we get keypresses */
#if CONF_WITH_ATARI_VIDEO
    set_sr(0x2300);
#else
    set_sr(0x2000);
#endif
    while(bconstat2())      /* eat any pending ones */
        bconin2();
    cprintf(_("\n*** Press any key to continue ***"));
    bconin2();

    savptr = (LONG) trap_save_area; /* in case running program has altered it */

    /*
     * we are still running with the PD of the failing program,
     * so calling Pterm() now should clean up and return control
     * to the caller
     */
    kill_program();         /* never returns */
}
