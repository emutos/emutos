/*
 * Copyright (C) 2019 The EmuTOS development team
 *
 * Authors:
 *  THH   Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef NAT_FEAT_H
#define NAT_FEAT_H

static inline void nf_shutdown(void)
{
    const char *str = "NF_SHUTDOWN";

    asm volatile (" move.l  %0,-(sp)\n"
                  " pea     0f(pc)\n"
                  " .dc.w   0x7300\n"
                  " move.l  d0,4(sp)\n"
                  " .dc.w   0x7301\n"
                  " addq.l  #8,sp\n"
                  "0:\n"
                  :: "r"(str)
                  : "d0", "d1", "d2", "a0", "a1", "a2", "memory", "cc");
}

#endif
