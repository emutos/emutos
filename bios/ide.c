/*
 * ide.c - Falcon IDE functions
 *
 * Copyright (c) 2011-2013 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Warning: This is very alpha IDE support.
 */

#define DBG_IDE 0

#include "config.h"
#include "portab.h"
#include "asm.h"
#include "blkdev.h"
#include "disk.h"
#include "ide.h"
#include "gemerror.h"
#include "vectors.h"
#include "kprint.h"
#include "coldfire.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

#if CONF_WITH_IDE

#ifdef MACHINE_M548X

#include "coldpriv.h"

struct IDE
{
    UBYTE filler00[2];
    UBYTE sector_number;
    UBYTE sector_count;
    UBYTE cylinder_high;
    UBYTE cylinder_low;
    UBYTE command; /* Read: status */
    UBYTE head;
    UWORD data;
    UBYTE filler0a[5];
    UBYTE control; /* Read: Alternate status */
};

#define ide_interface (*(volatile struct IDE*)(COMPACTFLASH_BASE + 0x1800))

#define IDE_WRITE_REGISTER_PAIR(r, a, b) \
    *(volatile UWORD*)&ide_interface.r = MAKE_UWORD(a, b)

#define IDE_WRITE_PAIR_SECTOR_NUMBER_SECTOR_COUNT(a, b) \
    IDE_WRITE_REGISTER_PAIR(sector_number, a, b)

#define IDE_WRITE_PAIR_CYLINDER_HIGH_CYLINDER_LOW(a) \
    *(volatile UWORD*)&ide_interface.cylinder_high = a

#define IDE_WRITE_PAIR_COMMAND_HEAD(a, b) \
    IDE_WRITE_REGISTER_PAIR(command, a, b)

#endif /* MACHINE_M548X */

#if CONF_ATARI_HARDWARE

struct IDE
{
    UWORD data;
    UBYTE filler02[3];
    UBYTE features; /* Read: error */
    UBYTE filler06[3];
    UBYTE sector_count;
    UBYTE filler0a[3];
    UBYTE sector_number;
    UBYTE filler0e[3];
    UBYTE cylinder_low;
    UBYTE filler12[3];
    UBYTE cylinder_high;
    UBYTE filler16[3];
    UBYTE head;
    UBYTE filler1a[3];
    UBYTE command; /* Read: status */
    UBYTE filler1e[27];
    UBYTE control; /* Read: Alternate status */
};

#define ide_interface (*(volatile struct IDE*)0xfff00000)

#endif /* CONF_ATARI_HARDWARE */

/* IDE defines */

#define IDE_CMD_IDENTIFY_DEVICE 0xec
#define IDE_CMD_READ_SECTOR 0x20
#define IDE_CMD_WRITE_SECTOR 0x30
#define IDE_CMD_DEVICE_RESET 0x08

#define IDE_MODE_CHS (0 << 6)
#define IDE_MODE_LBA (1 << 6)
#define IDE_DEVICE(n) ((n) << 4)

#define IDE_CONTROL_nIEN (1 << 1)
#define IDE_CONTROL_SRST (1 << 2)

#define IDE_STATUS_ERR (1 << 0)
#define IDE_STATUS_DRQ (1 << 3)
#define IDE_STATUS_DRDY (1 << 6)
#define IDE_STATUS_BSY (1 << 7)

int has_ide;

void detect_ide(void)
{
#ifdef MACHINE_AMIGA
    has_ide = has_gayle;
#elif defined(MACHINE_M548X)
    has_ide = TRUE;
#else
    has_ide = check_read_byte((long)&ide_interface.command);
#endif

#if DBG_IDE
    kprintf("has_ide = %d\n", has_ide);
#endif
}

static int ide_wait_not_busy(void)
{
    for (;;)
    {
        UBYTE status = ide_interface.command;
        //kprintf("ide_wait_not_busy() status = 0x%02x\n", status);

        if (status == 0xea)
            return EUNDEV;

        if (!(status & IDE_STATUS_BSY))
            return E_OK;

        //kprintf("ide_wait_not_busy() loop\n");
    }
}

static int ide_wait_not_busy_check_error(void)
{
    int ret;
    UBYTE status;

    ret = ide_wait_not_busy();
    if (ret < 0)
        return ret;

    status = ide_interface.command;
    if (status & IDE_STATUS_ERR)
        return ERR;

    return E_OK;
}

static void ide_select_sector_lba(UWORD device, ULONG sector)
{
#ifdef MACHINE_M548X
    IDE_WRITE_PAIR_SECTOR_NUMBER_SECTOR_COUNT((UBYTE)(sector & 0xff), 1);
    IDE_WRITE_PAIR_CYLINDER_HIGH_CYLINDER_LOW((UWORD)((sector & 0xffff00) >> 8));
#else
    // first change the device bit without changing anything else
    ide_interface.head = IDE_DEVICE(device) | (ide_interface.head & ~IDE_DEVICE(1));
    ide_interface.sector_number = (UBYTE)(sector & 0xff);
    ide_interface.cylinder_low = (UBYTE)((sector >> 8) & 0xff);
    ide_interface.cylinder_high = (UBYTE)((sector >> 16) & 0xff);
    // now write the rest of the devhead register.
    ide_interface.head = IDE_MODE_LBA | IDE_DEVICE(device) | (UBYTE)((sector >> 24) & 0x0f);
#endif
}

static int ide_wait_drq(void)
{
    for (;;)
    {
        UBYTE status = ide_interface.command;
        //kprintf("IDE status = 0x%02x\n", status);

#ifdef MACHINE_M548X
        if (status == 0 || status == 0xea)
#else
        if (status == 0)
#endif
            return EUNDEV;

        if (status & IDE_STATUS_ERR)
            return ERR;

        if (status & IDE_STATUS_DRQ)
            return E_OK;
    }
}

static int ide_read_data(UBYTE buffer[SECTOR_SIZE])
{
    UWORD* p = (UWORD*)buffer;
    UWORD* end = p + (SECTOR_SIZE / 2);
    int ret;

    ret = ide_wait_drq();
    if (ret < 0)
        return ret;

    while (p < end)
        *p++ = ide_interface.data;

    return E_OK;
}

static int ide_write_data(UBYTE buffer[SECTOR_SIZE], BOOL need_byteswap)
{
    UWORD* p = (UWORD*)buffer;
    UWORD* end = p + (SECTOR_SIZE / 2);
    int ret;

    ret = ide_wait_drq();
    if (ret < 0)
        return ret;

    if (need_byteswap) {
        while (p < end) {
            UWORD temp;
            temp = *p++;
            swpw(temp);
            ide_interface.data = temp;
        }
    } else {
        while (p < end)
            ide_interface.data = *p++;
    }

    return ide_wait_not_busy_check_error();
}

static int ide_read_sector(UWORD device, ULONG sector, UBYTE buffer[SECTOR_SIZE], BOOL need_byteswap)
{
    int ret;

    ide_select_sector_lba(device, sector);
#ifdef MACHINE_M548X
    IDE_WRITE_PAIR_COMMAND_HEAD(IDE_CMD_READ_SECTOR, IDE_MODE_LBA | IDE_DEVICE(device) | (UBYTE)((sector >> 24) & 0x0f));
#else
    ide_interface.sector_count = 1;
    ide_interface.command = IDE_CMD_READ_SECTOR;
#endif
    ret = ide_read_data(buffer);
    if (ret < 0)
        return ret;

    if (need_byteswap)
        byteswap(buffer, SECTOR_SIZE);

    return ide_wait_not_busy_check_error();
}

static int ide_write_sector(UWORD device, ULONG sector, UBYTE buffer[SECTOR_SIZE], BOOL need_byteswap)
{
    int ret;

    ide_select_sector_lba(device, sector);
#ifdef MACHINE_M548X
    IDE_WRITE_PAIR_SECTOR_NUMBER_SECTOR_COUNT((UBYTE)(sector & 0xff), 1);
    IDE_WRITE_PAIR_COMMAND_HEAD(IDE_CMD_WRITE_SECTOR, IDE_MODE_LBA | IDE_DEVICE(device) | (UBYTE)((sector >> 24) & 0x0f));
#else
    ide_interface.sector_count = 1;
    ide_interface.command = IDE_CMD_WRITE_SECTOR;
#endif

    ret = ide_write_data(buffer, need_byteswap);
    if (ret < 0)
        return ret;

    return E_OK;
}

LONG ide_rw(WORD rw, LONG sector, WORD count, LONG buf, WORD dev, BOOL need_byteswap)
{
    UBYTE* p = (UBYTE*)buf;
    int ret;

    if (!has_ide)
        return EUNDEV;

    if (dev >= 2) /* Only Master and Slave device supported */
        return EUNDEV;

    while (count > 0)
    {
        ret = rw ? ide_write_sector(dev, sector, p, need_byteswap)
            : ide_read_sector(dev, sector, p, need_byteswap);
        if (ret < 0)
            return ret;

        p += SECTOR_SIZE;
        ++sector;
        --count;
    }

    return E_OK;
}

#endif /* CONF_WITH_IDE */
