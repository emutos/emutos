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
#include "vectors.h"
#include "nvram.h"
#include "biosmem.h"

int has_nvram;

/* a 50 byte buffer that will only be allocated if there is an NVRAM */
static UBYTE *nvram_buf;
static int inited;

/*
 * detect_nvram - detect and init the nvram
 */
void detect_nvram(void)
{
    if(check_read_byte(0xffff8961)) {
        has_nvram = 1;
        nvram_buf = balloc(50);
        inited = 0;
    } else {
        has_nvram = 0;
    }
}

/*
 * get_nvram_rtc - read the realtime clock from NVRAM
 */
UBYTE get_nvram_rtc(int index)
{
    volatile UBYTE * addr_reg = (volatile UBYTE *)0xffff8961;
    volatile UBYTE * data_reg = (volatile UBYTE *)0xffff8963;
    int ret_value = 0;

    if (has_nvram) {
        if (index >=0 && index < 14) {
            *addr_reg = index;
                ret_value = *data_reg;
            }
    }

    return ret_value;
}

/*
 * set_nvram_rtc - set the realtime clock in NVRAM
 */
void set_nvram_rtc(int index, int data)
{
    volatile UBYTE * addr_reg = (volatile UBYTE *)0xffff8961;
    volatile UBYTE * data_reg = (volatile UBYTE *)0xffff8963;

    if (has_nvram) {
        if (index >=0 && index < 14) {
            *addr_reg = index;
                *data_reg = data;
            }
    }
}

/*
 * compute_sum - internal checksum handling
 */
static UWORD compute_sum(void)
{
    UWORD sum;
    int i;

    sum = 0;
    for(i = 0 ; i < 48 ; i+=2) {
        sum += (nvram_buf[i] << 8) | nvram_buf[i+1];
    }
    return sum;
}

static UWORD get_sum(void)
{
    return (nvram_buf[48] << 8 ) | nvram_buf[49];
}

static void set_sum(UWORD sum)
{
    volatile UBYTE * addr_reg = (volatile UBYTE *)0xffff8961;
    volatile UBYTE * data_reg = (volatile UBYTE *)0xffff8963;
    
    *addr_reg = 62;
    *data_reg = nvram_buf[48] = sum >> 8;
    *addr_reg = 63;
    *data_reg = nvram_buf[49] = sum;
}

/*
 * nvmaccess - XBIOS read or set NVRAM
 *
 * Arguments:
 *
 *   type   - 0:read, 1:write, 2:reset
 *   start  - start address for operation
 *   count  - count of bytes
 *   buffer - buffer for operations
 */
WORD nvmaccess(WORD type, WORD start, WORD count, PTR buffer)
{
    volatile UBYTE * addr_reg = (volatile UBYTE *)0xffff8961;
    volatile UBYTE * data_reg = (volatile UBYTE *)0xffff8963;
    UBYTE * ubuffer = (UBYTE *) buffer;
    int i;

    if(! has_nvram) {
        return 0x2E;
    }
    if(type == 2) { /* reset all */
        for(i = 0 ; i < 50 ; i++) {
            *addr_reg = i + 14;
            *data_reg = 0;
            nvram_buf[i] = 0;
        }
        inited = 1;
        return 0;
    } 
    /* else, first read the nvram if not done already */
    if(! inited) {
        for(i = 0 ; i < 50 ; i++) {
            *addr_reg = i + 14;
            nvram_buf[i] = *data_reg;
        }
        inited = 1;
        if(compute_sum() != get_sum()) {
            /* TODO, wrong checksum, what do we do ? */
        }
    }
    start += 14;
    switch(type) {
    case 0: /* read, from our buffer since it is already in memory */
        for(i = start ; i < start + count ; i++) {
            *ubuffer++ = nvram_buf[i];
        }
        break;
    case 1: /* write, in our buffer and in the memory */
        for(i = start ; i < start + count ; i++) {
            *addr_reg = i + 14;
            *data_reg = nvram_buf[i] = *ubuffer++; 
        }
        set_sum(compute_sum());
        /* TODO - verify ? */
        break;
    default:
        /* TODO, wrong operation code! */
        return -1;
    }
    return 0;
}
