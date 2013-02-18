/*
 * kprint.c - our own printf variants (mostly for debug purposes)
 *
 * Copyright (c) 2001-2008 The EmuTOS development team
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
#include "lineavars.h"
#include "vt52.h"
#include "tosvars.h"
#include "natfeat.h"
#include "processor.h"
#include "chardev.h"
#include "serport.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

#undef Super
/* Standard Super() binding */
#define Super(ptr)                          \
__extension__                               \
({                                          \
    register long retvalue __asm__("d0");   \
    long  _ptr = (long) (ptr);              \
                                            \
    __asm__ volatile                        \
    (                                       \
        "move.l %1,-(sp)\n\t"               \
        "move.w #0x20,-(sp)\n\t"             \
        "trap   #1\n\t"                     \
        "addq.l #6,sp"                      \
    : "=r"(retvalue)                    /* outputs */       \
    : "r"(_ptr)                         /* inputs  */       \
    : __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2"   \
      AND_MEMORY                            \
    );                                      \
    retvalue;                               \
})

#undef SuperToUser      /* prevent "redefined" warning message */
/*
 * Safe binding to switch back from supervisor to user mode.
 * On TOS or EmuTOS, if the stack pointer has changed between Super(0)
 * and Super(oldssp), the resulting user stack pointer is wrong.
 * This bug does not occur with FreeMiNT.
 * So the safe way to return from supervisor to user mode is to backup
 * the stack pointer then restore it after the trap.
 * Sometimes, GCC optimizes the stack usage, so this matters.
 *
 * Binding originally by Vincent Rivière, from MiNTlib's osbind.h
 */
#define SuperToUser(ptr)                    \
(void)__extension__                         \
({                                          \
    register long retvalue __asm__("d0");   \
    register long sp_backup;                \
                                            \
    __asm__ volatile                        \
    (                                       \
        "move.l  sp,%1\n\t"                 \
        "move.l  %2,-(sp)\n\t"              \
        "move.w  #0x20,-(sp)\n\t"           \
        "trap    #1\n\t"                    \
        "move.l  %1,sp"                     \
    : "=r"(retvalue), "=&r"(sp_backup)  /* outputs */       \
    : "g"((long)(ptr))                  /* inputs */        \
    : __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2"   \
    );                                      \
})


/* extern declarations */

extern void printout_stonx(char *);    /* in kprintasm.S */


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
    bconout1(1,c);
}
#endif

#if SCC_DEBUG_PRINT
static void kprintf_outc_sccB(int c)
{
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
#if CONF_WITH_UAE
    return doprintf(kprintf_outc_uae, fmt, ap);
#endif

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
    if (proc_enum == 0) { /* Call to panic(const char *fmt, ...) */
        struct {
            LONG pc;
        } *s = (void *)proc_stk;

        va_list ap;
        va_start(ap, fmt);
        vkcprintf(fmt, ap);
        va_end(ap);

        kcprintf("pc = %08lx\n",
                 s->pc);
#ifdef __mcoldfire__
    } else {
        /* On ColdFire, the exception frame is the same for all exceptions. */
        struct {
            WORD format_word;
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;

        if (proc_enum >= 2 && proc_enum < numberof(exc_messages)) {
            kcprintf("Panic: %s.\n",
                     exc_messages[proc_enum]);
        } else {
            kcprintf("Panic: Exception number %d.\n",
                     (int) proc_enum);
        }

        kcprintf("fw = %04x (format = %d, vector = %d, fault = %d), sr = %04x, pc = %08lx\n",
                 s->format_word,
                 (s->format_word & 0xf000) >> 12,
                 (s->format_word & 0x03fc) >> 2,
                 (s->format_word & 0x0c00) >> 8 | (s->format_word & 0x0002),
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

        kcprintf("Panic: %s.\n",
                 exc_messages[proc_enum]);
        kcprintf("misc = %04x, address = %08lx, opcode = %04x, sr = %04x, pc = %08lx\n",
                 s->misc, s->address, s->opcode, s->sr, s->pc);
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
            /* ... 29 words in the stack frame, but only 16 ones backuped */
        } *s = (void *)proc_stk;

        kcprintf("Panic: %s.\n",
                 exc_messages[proc_enum]);
        kcprintf("sr = %04x, pc = %08lx, fw = %04x, ssw = %04x, address = %08lx\n",
                 s->sr, s->pc, s->format_word, s->special_status_word, s->fault_address);
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

        kcprintf("Panic: %s.\n",
                 exc_messages[proc_enum]);
        kcprintf("sr = %04x, pc = %08lx, fw = %04x, ir = %04x, ssr = %04x, address = %08lx\n",
                 s->sr, s->pc, s->format_word, s->internal_register, s->special_status_register, s->data_cycle_fault_address);
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
            /* ... 30 words in the stack frame, but only 16 ones backuped */
        } *s = (void *)proc_stk;

        kcprintf("Panic: %s.\n",
                 exc_messages[proc_enum]);
        kcprintf("sr = %04x, pc = %08lx, fw = %04x, ea = %08lx, ssw = %04x, fa = %08lx\n",
                 s->sr, s->pc, s->format_word, s->effective_address, s->special_status_word, s->fault_address);
    } else if ((mcpu == 40 && proc_enum == 3)
               || (mcpu == 60 && (proc_enum == 2 || proc_enum == 3))) {
        /* 68040 Address Error, or 68060 Bus or Address Error */
        struct {
            WORD sr;
            LONG pc;
            WORD format_word;
            LONG address;
        } *s = (void *)proc_stk;

        kcprintf("Panic: %s.\n",
                 exc_messages[proc_enum]);
        kcprintf("sr = %04x, pc = %08lx, fw = %04x, address = %08lx\n",
                 s->sr, s->pc, s->format_word, s->address);
    } else if (proc_enum < numberof(exc_messages)) {
        struct {
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;

        kcprintf("Panic: %s.\n",
                 exc_messages[proc_enum]);
        kcprintf("sr = %04x, pc = %08lx\n",
                 s->sr, s->pc);
    } else {
        struct {
            WORD sr;
            LONG pc;
        } *s = (void *)proc_stk;

        kcprintf("Panic: Exception number %d.\n",
                 (int) proc_enum);
        kcprintf("sr = %04x, pc = %08lx\n",
                 s->sr, s->pc);
    }
#endif
    kcprintf("Dregs: %08lx %08lx %08lx %08lx  %08lx %08lx %08lx %08lx\n",
             proc_dregs[0], proc_dregs[1], proc_dregs[2], proc_dregs[3],
             proc_dregs[4], proc_dregs[5], proc_dregs[6], proc_dregs[7]);
    kcprintf("Aregs: %08lx %08lx %08lx %08lx  %08lx %08lx %08lx %08lx\n",
             proc_aregs[0], proc_aregs[1], proc_aregs[2], proc_aregs[3],
             proc_aregs[4], proc_aregs[5], proc_aregs[6], proc_aregs[7]);
    kcprintf("                                                                 usp = %08lx\n",
             proc_usp);
    kcprintf("Processor halted.\n");
    halt();
}

