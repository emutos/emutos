/*
 * biosbind..h - Bindings for BIOS access
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef BIOSBIND_H
#define BIOSBIND_H
 
/*
 * BIOS calls (trap13)
 *
 * For the void functions the return value may be not needed.
 * Just don't know how to not use it.  :-)
 */

static inline void Getmpb(long ptr)
{
    register long retval __asm__("d0");
    long  _a = (long) ptr;

    __asm__ __volatile__
        ("
         movl    %1,sp@-;
         movw    #0,sp@-;
         trap    #13;
         lea    sp@(6),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
}



static inline short Bconstat(short dev)
{
    register long retval __asm__("d0");
    short _a = dev;

    __asm__ __volatile__
        ("
         movw    %1,sp@-;
         movw    #1,sp@-;
         trap    #13;
         lea    sp@(4),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline long Bconin(short dev)
{
    register long retval __asm__("d0");
    short _a = dev;

    __asm__ __volatile__
        ("
         movw   %1,sp@-;
         movw   #2,sp@-;
         trap   #13;
         lea    sp@(4),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline long Bconout(short dev, short c)
{
    register long retval __asm__("d0");
    short _a = dev;
    short _b = c;

    __asm__ __volatile__
        ("
         movw   %2,sp@-;
         movw   %1,sp@-;
         movw   #3,sp@-;
         trap   #13;
         lea    sp@(6),sp "
         : "=r"(retval)
         : "r"(_a), "r"(_b)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline long Rwabs(short rwflag, long buf, short count,
                  short recno, short dev)
{
    register long retval __asm__("d0");
    short _a = rwflag;
    long  _b = (long) buf;
    short _c = count;
    short _d = recno;
    short _e = dev;

    __asm__ __volatile__
        ("
         movw   %5,sp@-;
         movw   %4,sp@-;
         movw   %3,sp@-;
         movl   %2,sp@-;
         movw   %1,sp@-;
         movw   #4,sp@-;
         trap   #13;
         lea    sp@(14),sp "
         : "=r"(retval)
         : "r"(_a),"r"(_b),"r"(_c),"r"(_d),"r"(_e)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline long Setexc(short vecnum, long vec )
{
    register long retval __asm__("d0");
    short _a = vecnum;
    long  _b = (long) vec;

    __asm__ __volatile__
        ("
         movl    %2,sp@-;
         movw    %1,sp@-;
         movw    #5,sp@-;
         trap    #13;
         lea    sp@(8),sp "
         : "=r"(retval)
         : "r"(_a), "r"(_b)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline long Tickcal()
{
    register long retval __asm__("d0");

    __asm__ __volatile__
        ("
         movw    #6,sp@-;
         trap    #13;
         lea    sp@(2),sp "
         : "=r"(retval)
         :
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline void * Getbpb(WORD dev)
{
    register long retval __asm__("d0");
    short _a = dev;

    __asm__ __volatile__
        ("
         movw   %1,sp@-;
         movw   #7,sp@-;
         trap   #13;
         lea    sp@(4),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return (void *)retval;
}



static inline short Bcostat(short dev)
{
    register long retval __asm__("d0");
    short _a = dev;

    __asm__ __volatile__
        ("
         movw    %1,sp@-;
         movw    #8,sp@-;
         trap    #13;
         lea    sp@(4),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline short Mediach(short dev)
{
    register long retval __asm__("d0");
    short _a = dev;

    __asm__ __volatile__
        ("
         movw    %1,sp@-;
         movw    #9,sp@-;
         trap    #13;
         lea    sp@(4),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline long Drvmap()
{
    register long retval __asm__("d0");

    __asm__ __volatile__
        ("
         movw    #10,sp@-;
         trap    #13;
         lea    sp@(2),sp "
         : "=r"(retval)
         :
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline long Kbshift(short mode)
{
    register long retval __asm__("d0");
    short _a = mode;

    __asm__ __volatile__
        ("
         movw    %1,sp@-;
         movw    #11,sp@-;
         trap    #13;
         lea    sp@(4),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline long Getshift()
{
    register long retval __asm__("d0");

    __asm__ __volatile__
        ("
         movw    #-1,sp@-;
         movw    #11,sp@-;
         trap    #13;
         lea    sp@(4),sp "
         : "=r"(retval)
         :
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}

#endif
