/*
 *  dbgbios.c - bios debug routines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "bios.h"
#include "kprint.h"

extern void printout(char *);

extern void bconout2(WORD, UBYTE);

  
/* doprintf implemented in doprintf.c. 
 * This is an OLD one, and does not support floating point 
 */
#include <stdarg.h>
extern int doprintf(void (*outc)(int), const char *fmt, va_list ap);



#define COMMENT 0
#define MAXDMP 1024
/*
 *  globals
 */

//static  char  buffer[MAXDMP] ;

// GLOBAL
char    *kcrlf = "\n\r" ;

/*==== cprintf - do formatted string output direct to the console ======*/

static void cprintf_outc(int c)
{
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


static void kprintf_outc(int c)
{
  char buf[2];
  buf[0] = c;
  buf[1] = 0;
  printout(buf);
}

static int vkprintf(const char *fmt, va_list ap)
{
  return doprintf(kprintf_outc, fmt, ap);
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

/*==== doassert ======*/

void doassert(const char *file, long line, const char *func, const char *text)
{
  kprintf("assert failed in %s:%ld (function %s): %s\n", file, line, func, text);
}

/*==== dopanic - display information found in 0x380 and halt ======*/

extern LONG proc_lives;
extern LONG proc_dregs[];       
extern LONG proc_aregs[];       
extern LONG proc_enum;  
extern LONG proc_usp;   
extern WORD proc_stk[];

static const char *exc_messages[] = {
  "", "", "bus error", "address error", 
  "illegal exception", "divide by zero", 
  "datatype overflow (CHK)", 
  "trapv overflow bit error",
  "privilege violation", "Trace", "LineA", "LineF" };
  

void dopanic(const char *fmt, ...)
{
  if(proc_lives != 0x12345678) {
    kprintf("No saved info in dopanic; halted.\n");
    halt();
  } 
  /* TODO, make sure the vt52 stuff is ready and wrapping */
  kcprintf("Panic: ");
  if(proc_enum == 0) {
    va_list ap;
    va_start(ap, fmt);
    vkcprintf(fmt, ap);
    va_end(ap);
  } else if(proc_enum == 2 || proc_enum == 3) {
    struct {
      WORD misc;
      LONG address;
      WORD opcode;
      WORD sr;
      LONG pc;
    } *s = (void *)proc_stk;
    kcprintf("%s. misc = 0x%04x, address = 0x%08lx\n",
      exc_messages[proc_enum], s->misc, s->address);
    kcprintf("opcode = 0x%04x, sr = 0x%04x, pc = 0x%08lx\n",
      s->opcode, s->sr, s->pc);
  } else if(proc_enum >= 4 && proc_enum < sizeof(exc_messages)) {
    struct {
      WORD sr;
      LONG pc;
    } *s = (void *)proc_stk;
    kcprintf("%s. sr = 0x%04x, pc = 0x%08lx\n",
      exc_messages[proc_enum], s->sr, s->pc);
  } else {
    struct {
      WORD sr;
      LONG pc;
    } *s = (void *)proc_stk;
    kprintf("Exception number %d. sr = 0x%04x, pc = 0x%08lx\n",
      (int) proc_enum, s->sr, s->pc);
  } 
  halt();
}


