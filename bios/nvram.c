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

/* don't know how to declare this only when an NVRAM is detected :-( */
static UBYTE buf[48];
static int inited;

/* detect and init the nvram */
void detect_nvram(void)
{
    if(check_read_byte(0xffff8961)) {
        has_nvram = 1;
	inited = 0;
    } else {
        has_nvram = 0;
    }
}

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

/* XBios function */
/* internal checksum handling */

static UWORD compute_sum(void)
{
    UWORD sum;
    int i;

    sum = 0;
    for(i = 0 ; i < 48 ; i+=2) {
        sum += (buf[i] << 8) | buf[i+1];
    }
    return sum;
}

static UWORD get_sum(void)
{
    return (buf[48] << 8 ) | buf[49];
}

static void set_sum(UWORD sum)
{
    volatile UBYTE * addr_reg = (volatile UBYTE *)0xffff8961;
    volatile UBYTE * data_reg = (volatile UBYTE *)0xffff8963;
    
    *addr_reg = 62;
    *data_reg = buf[48] = sum >> 8;
    *addr_reg = 63;
    *data_reg = buf[49] = sum;
}

/* XBios function */

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
	    buf[i] = 0;
        }
	inited = 1;
	return 0;
    } 
    /* else, first read the nvram if not done already */
    if(! inited) {
        for(i = 0 ; i < 50 ; i++) {
            *addr_reg = i + 14;
            buf[i] = *data_reg;
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
	    *ubuffer++ = buf[i];
	}
        break;
    case 1: /* write, in our buffer and in the memory */
        for(i = start ; i < start + count ; i++) {
	    *addr_reg = i + 14;
            *data_reg = buf[i] = *ubuffer++; 
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
