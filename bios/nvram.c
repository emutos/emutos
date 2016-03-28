/*
 * nvram.c - Non-Volatile RAM access
 *
 * Copyright (c) 2001-2014 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "cookie.h"
#include "machine.h"
#include "vectors.h"
#include "nvram.h"
#include "biosmem.h"
#include "kprint.h"

#if CONF_WITH_NVRAM

#define NVRAM_ADDR_REG  0xffff8961L
#define NVRAM_DATA_REG  0xffff8963L

#define NVRAM_RTC_SIZE  14          /* first 14 registers are RTC */
#define NVRAM_START     NVRAM_RTC_SIZE
#define NVRAM_SIZE      50          /* remaining 50 are RAM */
#define NVRAM_USER_SIZE 48          /* of which the user may access 48 */
#define NVRAM_CKSUM     NVRAM_USER_SIZE /* and the last 2 are checksum */

int has_nvram;

static UBYTE nvram_buf[NVRAM_SIZE];
static int inited;


/*
 * detect_nvram - detect the nvram
 */
void detect_nvram(void)
{
    if (check_read_byte(NVRAM_ADDR_REG))
    {
        has_nvram = 1;
        inited = 0;
    }
    else has_nvram = 0;
}

/*
 * get_nvram_rtc - read the realtime clock from NVRAM
 */
UBYTE get_nvram_rtc(int index)
{
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;
    int ret_value = 0;

    if (has_nvram)
    {
        if ((index >= 0) && (index < NVRAM_RTC_SIZE))
        {
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
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;

    if (has_nvram)
    {
        if ((index >= 0) && (index < NVRAM_RTC_SIZE))
        {
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
    UBYTE sum;
    int i;

    sum = 0;
    for (i = 0; i < NVRAM_USER_SIZE; i++)
    {
        sum += nvram_buf[i];
    }

    return (~sum << 8) | sum;
}

static UWORD get_sum(void)
{
    return (nvram_buf[NVRAM_CKSUM] << 8) | nvram_buf[NVRAM_CKSUM+1];
}

static void set_sum(UWORD sum)
{
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;

    *addr_reg = NVRAM_START + NVRAM_CKSUM;
    *data_reg = nvram_buf[NVRAM_CKSUM] = sum >> 8;
    *addr_reg = NVRAM_START + NVRAM_CKSUM + 1;
    *data_reg = nvram_buf[NVRAM_CKSUM+1] = sum;
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
WORD nvmaccess(WORD type, WORD start, WORD count, UBYTE *buffer)
{
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;
    int i;

    if (!has_nvram)
        return 0x2E;

    if (type == 2)      /* reset all */
    {
        for (i = 0; i < NVRAM_SIZE; i++)
        {
            *addr_reg = i + NVRAM_START;
            *data_reg = 0;
            nvram_buf[i] = 0;
        }
        inited = 1;
        return 0;
    }

    /* else, first read the nvram if not done already */
    if (!inited)
    {
        for (i = 0; i < NVRAM_SIZE; i++)
        {
            *addr_reg = i + NVRAM_START;
            nvram_buf[i] = *data_reg;
        }
        inited = 1;
    }

    if (buffer == NULL || start < 0 || count < 1 || (start + count) > 49)
        return -5;

    switch(type) {
    case 0:         /* read, from our buffer since it is already in memory */
        {
            UWORD expected = compute_sum();
            UWORD actual = get_sum();

            if (expected != actual)
            {
                KDEBUG(("wrong nvram: expected=0x%04x actual=0x%04x\n", expected, actual));
                /* wrong checksum, return error code */
                return -12;
            }
            for (i = start; i < start + count; i++)
                *buffer++ = nvram_buf[i];
        }
        break;
    case 1:         /* write, in our buffer and in the memory */
        for (i = start; i < start + count; i++)
        {
            *addr_reg = i + NVRAM_START;
            *data_reg = nvram_buf[i] = *buffer++;
        }
        set_sum(compute_sum());
        /* TODO - verify ? */
        break;
    default:
        /* wrong operation code! */
        return -5;
    }

    return 0;
}

#endif  /* CONF_WITH_NVRAM */
