/*
 * biosbind.h - Bindings for BIOS access
 *
 * Copyright (c) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef BIOSBIND_H
#define BIOSBIND_H

#define Getmpb(a) bios_v_l(0x0,a)
#define Bconstat(a) bios_w_w(0x1,a)
#define Bconin(a) bios_l_w(0x2,a)
#define Bconout(a,b) bios_l_ww(0x3,a,b)
#define Rwabs(a,b,c,d,e,lrec) bios_l_wlwwwl(0x4,a,b,c,d,e,lrec)
#define Setexc(a,b) bios_l_wl(0x5,a,b)
#define Tickcal() bios_l_v(0x6)
#define Getbpb(a) bios_l_w(0x7,a)
#define Bcostat(a) bios_l_w(0x8,a)
#define Mediach(a) bios_l_w(0x9,a)
#define Drvmap() bios_l_v(0xa)
#define Kbshift(a) bios_l_w(0xb,a)



static __inline__ void bios_v_l(int op, long a)
{
    __asm__ volatile (
        "move.l  %1,-(sp)\n\t"
        "move.w  %0,-(sp)\n\t"
        "trap    #13\n\t"
        "addq.l  #6,sp"
         :
         : "nr"(op), "ir"(a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory", "cc"
        );
}

static __inline__ void bios_v_ww(int op, short a, short b)
{
    __asm__ volatile (
        "move.w  %2,-(sp)\n\t"
        "move.w  %1,-(sp)\n\t"
        "move.w  %0,-(sp)\n\t"
        "trap    #13\n\t"
        "addq.l  #6,sp"
         :
         : "nr"(op), "nr"(a), "nr"(b)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory", "cc"
        );
}



static __inline__ short bios_w_w(int op, short a)
{
    register long retval __asm__("d0");

    __asm__ volatile (
        "move.w  %2,-(sp)\n\t"
        "move.w  %1,-(sp)\n\t"
        "trap    #13\n\t"
        "addq.l  #4,sp"
         : "=r"(retval)
         : "nr"(op), "nr"(a)
         : "d1", "d2", "a0", "a1", "a2", "memory", "cc"
        );
    return retval;
}

static __inline__ long bios_l_v(int op)
{
    register long retval __asm__("d0");

    __asm__ volatile (
        "move.w  %1,-(sp)\n\t"
        "trap    #13\n\t"
        "addq.l  #2,sp"
         : "=r"(retval)
         : "nr"(op)
         : "d1", "d2", "a0", "a1", "a2", "memory", "cc"
        );
    return retval;
}

static __inline__ long bios_l_w(int op, short a)
{
    register long retval __asm__("d0");

    __asm__ volatile (
        "move.w  %2,-(sp)\n\t"
        "move.w  %1,-(sp)\n\t"
        "trap    #13\n\t"
        "addq.l  #4,sp"
         : "=r"(retval)
         : "nr"(op), "nr"(a)
         : "d1", "d2", "a0", "a1", "a2", "memory", "cc"
        );
    return retval;
}

static __inline__ long bios_l_ww(int op, short a, short b)
{
    register long retval __asm__("d0");

    __asm__ volatile (
        "move.w  %3,-(sp)\n\t"
        "move.w  %2,-(sp)\n\t"
        "move.w  %1,-(sp)\n\t"
        "trap    #13\n\t"
        "addq.l  #6,sp"
         : "=r"(retval)
         : "nr"(op), "nr"(a), "nr"(b)
         : "d1", "d2", "a0", "a1", "a2", "memory", "cc"
        );
    return retval;
}

static __inline__ long bios_l_wl(int op, short a, long b)
{
    register long retval __asm__("d0");

    __asm__ volatile (
        "move.l  %3,-(sp)\n\t"
        "move.w  %2,-(sp)\n\t"
        "move.w  %1,-(sp)\n\t"
        "trap    #13\n\t"
        "addq.l  #8,sp"
         : "=r"(retval)
         : "nr"(op), "nr"(a), "ir"(b)
         : "d1", "d2", "a0", "a1", "a2", "memory", "cc"
        );
    return retval;
}

static __inline__ long
bios_l_wlwwwl(int op, short a, long b, short c, short d, short e, long f)
{
    register long retval __asm__("d0");

    __asm__ volatile (
        "move.l  %7,-(sp)\n\t"
        "move.w  %6,-(sp)\n\t"
        "move.w  %5,-(sp)\n\t"
        "move.w  %4,-(sp)\n\t"
        "move.l  %3,-(sp)\n\t"
        "move.w  %2,-(sp)\n\t"
        "move.w  %1,-(sp)\n\t"
        "trap    #13\n\t"
        "lea     18(sp),sp"
         : "=r"(retval)
         : "nr"(op), "nr"(a), "ir"(b), "nr"(c), "nr"(d), "nr"(e), "ir"(f)
         : "d1", "d2", "a0", "a1", "a2", "memory", "cc"
        );
    return retval;
}

#endif /* BIOSBIND_H */
