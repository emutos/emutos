/*
 * nvram.c - Non-Volatile RAM access
 *
 * Copyright (c) 2001 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "portab.h"
#include "cookie.h"
#include "machine.h"
#include "detect.h"
#include "nvram.h"

int has_nvram;

void detect_nvram(void)
{
    if(check_read_byte(0xffff8961)) {
        has_nvram = 1;
    } else {
        has_nvram = 0;
    }
}

/* XBios function */

int nvmaccess(int type, int start, int count, char *buffer)
{
    volatile BYTE * addr_reg = (volatile BYTE *)0xffff8961;
    volatile BYTE * data_reg = (volatile BYTE *)0xffff8963;
    int i;

    if(! has_nvram) {
        return 0x2E;
    }
    start += 14;
    
    /* TODO: update and check cksum in bytes 62, 63 */
    
    switch(type) {
    case 0:
        while(count--) {
            *addr_reg = start++;
            *buffer++ = *data_reg;
        }
        break;
    case 1:
        while(count--) {
            *addr_reg = start++;
            *data_reg = *buffer++;
        }
        break;
    case 2: /* reset all, including cksum */
        for(i = 14 ; i < 64 ; i++) {
            *addr_reg = i;
            *data_reg = 0;
        }
    }
    return 0;
}
