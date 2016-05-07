/*
 * nvram.c - Non-Volatile RAM access
 *
 * Copyright (c) 2001-2016 The EmuTOS development team
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

/*
 * on the TT, resetting NVRAM causes TOS3 to zero it.
 * on the Falcon and FireBee, it is set to zeroes except for a small
 * portion (see below) starting at NVRAM_INIT_START.
 *
 * we do the same to avoid problems booting Atari TOS and EmuTOS on the
 * same system.
 */
#define NVRAM_INIT_START    8
const UBYTE nvram_init[] = { 0x00, 0x2f, 0x20, 0xff, 0xff, 0xff };

int has_nvram;

/*
 * detect_nvram - detect the nvram
 */
void detect_nvram(void)
{
    if (check_read_byte(NVRAM_ADDR_REG))
        has_nvram = 1;
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
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;
    UBYTE sum;
    int i;

    for (i = 0, sum = 0; i < NVRAM_USER_SIZE; i++)
    {
        *addr_reg = i + NVRAM_START;
        sum += *data_reg;
    }

    return (~sum << 8) | sum;
}

/*
 * get_sum - read checksum from NVRAM
 */
static UWORD get_sum(void)
{
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;
    UWORD sum;

    *addr_reg = NVRAM_START + NVRAM_CKSUM;
    sum = *data_reg << 8;
    *addr_reg = NVRAM_START + NVRAM_CKSUM + 1;
    sum |= *data_reg;

    return sum;
}

/*
 * set_sum - write checksum to NVRAM
 */
static void set_sum(UWORD sum)
{
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;

    *addr_reg = NVRAM_START + NVRAM_CKSUM;
    *data_reg = sum >> 8;
    *addr_reg = NVRAM_START + NVRAM_CKSUM + 1;
    *data_reg = sum;
}

/*
 * nvmaccess - XBIOS read/write/reset NVRAM
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
        for (i = 0; i < NVRAM_USER_SIZE; i++)
        {
            *addr_reg = i + NVRAM_START;
            *data_reg = 0;
        }
        if (cookie_mch == MCH_TT)
            set_sum(compute_sum());
        else
            nvmaccess(1,NVRAM_INIT_START,ARRAY_SIZE(nvram_init),CONST_CAST(UBYTE *,nvram_init));
        return 0;
    }

    if ((buffer == NULL) || (start < 0) || (count < 1) || ((start + count) > NVRAM_USER_SIZE))
        return -5;

    switch(type) {
    case 0:         /* read */
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
            {
                *addr_reg = i + NVRAM_START;
                *buffer++ = *data_reg;
            }
        }
        break;
    case 1:         /* write */
        for (i = start; i < start + count; i++)
        {
            *addr_reg = i + NVRAM_START;
            *data_reg = *buffer++;
        }
        set_sum(compute_sum());
        break;
    default:
        /* wrong operation code! */
        return -5;
    }

    return 0;
}

#endif  /* CONF_WITH_NVRAM */
