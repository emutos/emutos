/*
 * xbiosbind.h - Bindings for XBIOS access
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef XBIOSBIND_H
#define XBIOSBIND_H
 
/*
 * XBIOS calls (trap14) - just the used ones for now
 */

static inline
long Initmous(short type, long param, long vptr)
{
    register long retval __asm__("d0");
    long  _a = type;
    long  _b = param;
    long  _c = vptr;

    __asm__ __volatile__
        ("
         movl    %3,sp@-;
         movl    %2,sp@-;
         movw    %1,sp@-;
         movw    #0x00,sp@-;
         trap    #14;
         lea	sp@(6),sp "
         : "=r"(retval)
         : "r"(_a), "r"(_b), "r"(_c)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline
long Iorec(short dev)
{
    register long retval __asm__("d0");
    short _a = dev;

    __asm__ __volatile__
        ("
         movw    %1,sp@-;
         movw    #0x0e,sp@-;
         trap    #14;
         lea	sp@(4),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline
long Random()
{
    register long retval __asm__("d0");

    __asm__ __volatile__
        ("
         movw    #0x11,sp@-;
         trap    #14;
         lea	sp@(2),sp "
         : "=r"(retval)
         :
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline
long Settime(long time)
{
    register long retval __asm__("d0");
    long  _a = (long) time;

    __asm__ __volatile__
        ("
         movl    %1,sp@-;
         movw    #0x16,sp@-;
         trap    #14;
         lea	sp@(6),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline
long Gettime()
{
    register long retval __asm__("d0");

    __asm__ __volatile__
        ("
         movw    #0x17,sp@-;
         trap    #14;
         lea	sp@(2),sp "
         : "=r"(retval)
         :
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline
long Kbdvbase()
{
    register long retval __asm__("d0");

    __asm__ __volatile__
        ("
         movw    #0x22,sp@-;
         trap    #14;
         lea	sp@(2),sp "
         : "=r"(retval)
         :
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline
long Supexec(long ptr)
{
    register long retval __asm__("d0");
    long  _a = (long) ptr;

    __asm__ __volatile__
        ("
         movl    %1,sp@-;
         movw    #0x26,sp@-;
         trap    #14;
         lea	sp@(6),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}



static inline
long Bconmap(short dev)
{
    register long retval __asm__("d0");
    short _a = dev;

    __asm__ __volatile__
        ("
         movw    %1,sp@-;
         movw    #0x2c,sp@-;
         trap    #14;
         lea	sp@(4),sp "
         : "=r"(retval)
         : "r"(_a)
         : "d0", "d1", "d2", "a0", "a1", "a2", "memory"
        );
    return retval;
}


#endif
