/*
 * ide.c - Falcon IDE functions
 *
 * Copyright (c) 2011 EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Warning: This is very alpha IDE support.
 * Only the Master device is supported.
 * Only standard PC disks (byteswapped) are supported.
 */

#include "config.h"
#include "portab.h"
#include "ide.h"
#include "gemerror.h"
#include "kprint.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

#if CONF_WITH_IDE

#define MAKE_UWORD(a, b) (((UWORD)(a) << 8) | (b))

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

#define ide_interface (*(volatile struct IDE*)0x00f00000)

#endif /* CONF_ATARI_HARDWARE */

/* IDE defines */

#define IDE_SECTOR_SIZE 512

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

#if CONF_ATARI_HARDWARE

/* The following is required to access PC compatible disks */
#define BYTESWAPPED_SECTOR_DATA

static void byteswap(UBYTE* buffer, ULONG size)
{
    UBYTE* p;

    for (p = buffer; p < buffer + IDE_SECTOR_SIZE; p += 2)
    {
        UBYTE temp = p[0];
        p[0] = p[1];
        p[1] = temp;
    }
}

#endif

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
    // first change the device bit without changing anything else 
    ide_interface.head = IDE_DEVICE(device) | (ide_interface.head & ~IDE_DEVICE(1));
    ide_interface.sector_number = (UBYTE)(sector & 0xff);
    ide_interface.cylinder_low = (UBYTE)((sector >> 8) & 0xff);
    ide_interface.cylinder_high = (UBYTE)((sector >> 16) & 0xff);
    // now write the rest of the devhead register.
    ide_interface.head = IDE_MODE_LBA | IDE_DEVICE(device) | (UBYTE)((sector >> 24) & 0x0f);
}

static int ide_wait_drq(void)
{
    for (;;)
    {
        UBYTE status = ide_interface.command;
        //kprintf("IDE status = 0x%02x\n", status);

        if (status == 0)
            return EUNDEV;

        if (status & IDE_STATUS_ERR)
            return ERR;
            
        if (status & IDE_STATUS_DRQ)
            return E_OK;
    }
}

static int ide_read_data(UBYTE buffer[IDE_SECTOR_SIZE])
{
    UWORD* p = (UWORD*)buffer;
    UWORD* end = p + (IDE_SECTOR_SIZE / 2);
    int ret;

    ret = ide_wait_drq();
    if (ret < 0)
        return ret;

    while (p < end)
        *p++ = ide_interface.data;
        
    return E_OK;
}

static int ide_write_data(UBYTE buffer[IDE_SECTOR_SIZE], BOOL need_byteswap)
{
    UWORD* p = (UWORD*)buffer;
    UWORD* end = p + (IDE_SECTOR_SIZE / 2);
    int ret;

    ret = ide_wait_drq();
    if (ret < 0)
        return ret;

    if (need_byteswap)
    {
        UBYTE* pb = (UBYTE*)p;
        UBYTE* endb = (UBYTE*)end;

        while (pb < endb)
        {
            ide_interface.data = MAKE_UWORD(pb[1], pb[0]);
            pb += 2;
        }
    }
    else
    {
        while (p < end)
            ide_interface.data = *p++;
    }
        
    return ide_wait_not_busy_check_error();
}

static int ide_read_sector(UWORD device, ULONG sector, UBYTE buffer[IDE_SECTOR_SIZE])
{
    int ret;

    ide_select_sector_lba(device, sector);
    ide_interface.sector_count = 1;
    ide_interface.command = IDE_CMD_READ_SECTOR;

    ret = ide_read_data(buffer);
    if (ret < 0)
        return ret;

#ifdef BYTESWAPPED_SECTOR_DATA
    byteswap(buffer, IDE_SECTOR_SIZE);
#endif

    ide_wait_not_busy_check_error();

    return E_OK;
}

static int ide_write_sector(UWORD device, ULONG sector, UBYTE buffer[IDE_SECTOR_SIZE])
{
    int ret;

    ide_select_sector_lba(device, sector);
    ide_interface.sector_count = 1;
    ide_interface.command = IDE_CMD_WRITE_SECTOR;

#ifdef BYTESWAPPED_SECTOR_DATA
    ret = ide_write_data(buffer, TRUE);
#else
    ret = ide_write_data(buffer, FALSE);
#endif
    if (ret < 0)
        return ret;

    return E_OK;
}

LONG ide_rw(WORD rw, LONG sector, WORD count, LONG buf, WORD dev)
{
    UBYTE* p = (UBYTE*)buf;
    int ret;

#ifdef MACHINE_AMIGA
    if (!has_gayle)
        return EUNDEV;
#endif

    if (dev >= 2) /* Only Master and Slave device supported */
        return EUNDEV;

    while (count > 0)
    {
        ret = rw ? ide_write_sector(dev, sector, p)
            : ide_read_sector(dev, sector, p);
        if (ret < 0)
            return ret;

        p += IDE_SECTOR_SIZE;
        ++sector;
        --count;
    }
    
    return E_OK;
}

#endif /* CONF_WITH_IDE */
