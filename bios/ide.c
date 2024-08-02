/*
 * ide.c - Falcon IDE functions
 *
 * Copyright (C) 2011-2024 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Note: this driver does not support CHS addressing.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "asm.h"
#include "blkdev.h"
#include "delay.h"
#include "disk.h"
#include "ide.h"
#include "scsicmds.h"
#include "scsidriv.h"
#include "gemerror.h"
#include "string.h"
#include "tosvars.h"
#include "vectors.h"
#include "machine.h"
#include "has.h"
#include "coldfire.h"
#include "processor.h"
#include "biosmem.h"
#include "amiga.h"
#include "intmath.h"

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
    UBYTE command;  /* Read: status */
    UBYTE head;
    UWORD data;
    UBYTE filler0a[2];
    UBYTE features; /* Read: error */
    UBYTE filler0d;
    UBYTE filler0e;
    UBYTE control;  /* Read: Alternate status */
};

#define ide_interface ((volatile struct IDE *)(COMPACTFLASH_BASE + 0x1800))

/* On M548X, the IDE registers must be read and written as a single word. */

#define IDE_WRITE_REGISTER_PAIR(r,a,b) \
    *(volatile UWORD *)&ide_interface->r = MAKE_UWORD(a,b)

/* The interface 'i' is passed to these macros only for compatibility.
   In M548X, the macros always use the only IDE interface on the board. */

#define IDE_WRITE_SECTOR_NUMBER_SECTOR_COUNT(i,a,b) \
    IDE_WRITE_REGISTER_PAIR(sector_number,a,b)

#define IDE_WRITE_CYLINDER_HIGH_CYLINDER_LOW(i,a) \
    *(volatile UWORD *)&ide_interface->cylinder_high = a

#define IDE_WRITE_COMMAND_HEAD(i,a,b) \
    IDE_WRITE_REGISTER_PAIR(command,a,b)

#define IDE_WRITE_CONTROL(i,a) \
    IDE_WRITE_REGISTER_PAIR(filler0e,0,a)

/*
 * this macro uses the NOP command, which is specifically provided
 * for situations where the command register must be written at the
 * same time as the head register.  see the x3t10 ata-2 and ata-3
 * specifications for details.
 */
#define IDE_WRITE_HEAD(i,a) \
    IDE_WRITE_REGISTER_PAIR(command,IDE_CMD_NOP,a)

#define IDE_READ_REGISTER_PAIR(r) \
    *(volatile UWORD *)&ide_interface->r

#define IDE_READ_SECTOR_NUMBER_SECTOR_COUNT(i) \
    IDE_READ_REGISTER_PAIR(sector_number)

#define IDE_READ_CYLINDER_HIGH_CYLINDER_LOW(i) \
    IDE_READ_REGISTER_PAIR(cylinder_high)

#define IDE_READ_STATUS(i)   ide_interface->command

#define IDE_READ_ERROR(i)    ide_interface->features

#define IDE_READ_ALT_STATUS(i) \
    IDE_READ_REGISTER_PAIR(filler0e)

#else

/* On standard hardware, the IDE registers can be accessed as single bytes. */

#define IDE_WRITE_SECTOR_NUMBER_SECTOR_COUNT(i,a,b) \
    { i->sector_number = a; i->sector_count = b; }
#define IDE_WRITE_CYLINDER_HIGH_CYLINDER_LOW(i,a) \
    { i->cylinder_high = HIBYTE(a); i->cylinder_low = LOBYTE(a); }
#define IDE_WRITE_COMMAND_HEAD(i,a,b) \
    { i->head = b; i->command = a; }
#define IDE_WRITE_CONTROL(i,a)    i->control = a
#define IDE_WRITE_HEAD(i,a)       i->head = a

#define IDE_READ_STATUS(i)        i->command
#define IDE_READ_ALT_STATUS(i)    i->control
#define IDE_READ_ERROR(i)         i->features
#define IDE_READ_SECTOR_NUMBER_SECTOR_COUNT(i) \
    MAKE_UWORD(i->sector_number, i->sector_count)
#define IDE_READ_CYLINDER_HIGH_CYLINDER_LOW(i) \
    MAKE_UWORD(i->cylinder_high, i->cylinder_low)

#endif /* MACHINE_M548X */

/* the data register is naturally byteswapped on some hardware */
#if defined(MACHINE_AMIGA)
#define IDE_DATA_REGISTER_IS_BYTESWAPPED TRUE
#else
#define IDE_DATA_REGISTER_IS_BYTESWAPPED FALSE
#endif

/* set the following to 1 to use 32-bit data transfer */
#if CONF_ATARI_HARDWARE
#define IDE_32BIT_XFER TRUE
#else
#define IDE_32BIT_XFER FALSE
#endif

#if IDE_32BIT_XFER
#define XFERWIDTH   ULONG
#define xferswap(a) swpw2(a)
#define ide_get_and_incr(src,dst) asm volatile("move.l (%1),(%0)+" : "=a"(dst): "a"(src), "0"(dst));
#define ide_put_and_incr(src,dst) asm volatile("move.l (%0)+,(%1)" : "=a"(src): "a"(dst), "0"(src));
#else
#define XFERWIDTH   UWORD
#define xferswap(a) swpw(a)
#define ide_get_and_incr(src,dst) asm volatile("move.w (%1),(%0)+" : "=a"(dst): "a"(src), "0"(dst));
#define ide_put_and_incr(src,dst) asm volatile("move.w (%0)+,(%1)" : "=a"(src): "a"(dst), "0"(src));
#endif

#if CONF_ATARI_HARDWARE

#ifdef MACHINE_FIREBEE
#define NUM_IDE_INTERFACES  2
#else
#define NUM_IDE_INTERFACES  4   /* (e.g. stacked ST Doubler) */
#endif

struct IDE
{
    XFERWIDTH data;         /* ATA & ATAPI: data transfer */
#if !IDE_32BIT_XFER
    UBYTE filler02[2];
#endif
    UBYTE filler04;
    UBYTE features;         /* ATA & ATAPI: Read: error / Write: features */
    UBYTE filler06[3];
    UBYTE sector_count;     /* ATAPI: Read: ATAPI Interrupt Reason Register / Write: unused */
    UBYTE filler0a[3];
    UBYTE sector_number;    /* ATAPI: reserved */
    UBYTE filler0e[3];
    UBYTE cylinder_low;     /* ATAPI: Byte Count Register (bits 0-7) */
    UBYTE filler12[3];
    UBYTE cylinder_high;    /* ATAPI: Byte Count Register (bits 8-15) */
    UBYTE filler16[3];
    UBYTE head;             /* ATAPI: Drive select */
    UBYTE filler1a[3];
    UBYTE command;          /* ATA & ATAPI: Read: status / Write: ATA command */
    UBYTE filler1e[27];
    UBYTE control;          /* ATA & ATAPI: Read: alternate status / Write: device control */
    UBYTE filler3a[6];
};

#define ide_interface           ((volatile struct IDE *)0xfff00000)

#else

#define NUM_IDE_INTERFACES  1

#endif /* CONF_ATARI_HARDWARE */


/* IDE defines */

#define IDE_CMD_IDENTIFY_DEVICE 0xec
#define IDE_CMD_NOP             0x00
#define IDE_CMD_READ_SECTOR     0x20
#define IDE_CMD_WRITE_SECTOR    0x30
#define IDE_CMD_READ_SECTOR_EX  0x24
#define IDE_CMD_WRITE_SECTOR_EX 0x34
#define IDE_CMD_READ_MULTIPLE       0xc4
#define IDE_CMD_WRITE_MULTIPLE      0xc5
#define IDE_CMD_READ_MULTIPLE_EX    0x29
#define IDE_CMD_WRITE_MULTIPLE_EX   0x39
#define IDE_CMD_SET_MULTIPLE_MODE   0xc6

#define IDE_CMD_ATAPI_PACKET    0xa0    /* ATAPI-only commands */
#define IDE_CMD_ATAPI_IDENTIFY  0xa1

#define IDE_CAPABILITY_LBA48    0x0400

#define IDE_MODE_CHS    (0 << 6)
#define IDE_MODE_LBA    (1 << 6)
#define IDE_DEVICE(n)   ((n) << 4)

#define IDE_CONTROL_nIEN (1 << 1)
#define IDE_CONTROL_SRST (1 << 2)
#define IDE_CONTROL_HOB  (1 << 7)       /* LBA48 */

#define IDE_STATUS_ERR  (1 << 0)
#define IDE_STATUS_DRQ  (1 << 3)
#define IDE_STATUS_DF   (1 << 5)
#define IDE_STATUS_DRDY (1 << 6)
#define IDE_STATUS_BSY  (1 << 7)

#define IDE_ERROR_ABRT  (1 << 2)

/*
 * maximum number of sectors per physical i/o.  this MUST not exceed 256,
 * at least for LBA28-style commands.  the best performance is obtained
 * if this is a multiple of the sectors-per-interrupt value supported by
 * the drive(s) in multiple mode.
 */
#define MAXSECS_PER_IO  32


/* interface/device info */

struct IFINFO {
    struct {
        UBYTE type;
        UBYTE options;
        UBYTE spi;          /* # sectors transferred between interrupts */
#if CONF_WITH_SCSI_DRIVER
        UBYTE sense;        /* ATA: current sense condition on device */
        UBYTE packet_size;  /* ATAPI: packet size */
#endif
    } dev[2];
    volatile struct IDE *base_address;
    BOOL twisted_cable;
};

#define DEVTYPE_NONE    0               /* for 'type' */
#define DEVTYPE_UNKNOWN 1
#define DEVTYPE_ATA     2
#define DEVTYPE_ATAPI   3

#define MULTIPLE_MODE_ACTIVE    0x01    /* for 'options' */
#define LBA48_ACTIVE            0x02

#define INVALID_OPCODE  1               /* for 'sense' */
#define INVALID_SECTOR  2
#define INVALID_CDB     3
#define INVALID_LUN     4

#define ATAPI_MAX_BYTECOUNT     65012   /* largest multiple of 512 in UWORD */


/* timing stuff */

#ifdef MACHINE_AMIGA
/* Amiga already provides proper delay at bus level, no need for more */
#define DELAY_400NS     NULL_FUNCTION()
#else
#define DELAY_400NS     delay_loop(delay400ns)
#endif

#define DELAY_5US       delay_loop(delay5us)

/*
 * timeouts
 *
 * Note: the 'official' timeout for a reset is 31 seconds.  However, this
 * causes excessively long delays during initialisation on (for example)
 * a Falcon if a device is not present.  The value chosen below is
 * conservative for modern IDE devices.  If this is insufficient in
 * particular cases, the default cold boot delay can be patched in the
 * ROM image.  See doc/version.txt for details.
 */
#define SHORT_TIMEOUT   (CLOCKS_PER_SEC/10) /* 100ms */
#define XFER_TIMEOUT    (3*CLOCKS_PER_SEC)  /* 3 seconds for data xfer */
#define LONG_TIMEOUT    (3*CLOCKS_PER_SEC)  /* 3 seconds for reset */

static int has_ide;
static struct IFINFO ifinfo[NUM_IDE_INTERFACES];
static ULONG delay400ns;
static ULONG delay5us;
static struct {
    UWORD general_config;       /* ATAPI */
    UWORD filler01[22];
    char firmware_revision[8];  /* also in ATAPI */
    char model_number[40];      /* also in ATAPI */
    UWORD multiple_io_info;
    UWORD filler2f;
    UWORD capabilities;
    UWORD filler32[10];
    UWORD numsecs_lba28[2]; /* number of sectors for LBA28 cmds */
    UWORD filler3e[20];
    UWORD cmds_supported[3];
    UWORD filler55[15];
    UWORD maxsec_lba48[4];  /* max sector number for LBA48 cmds */
    UWORD filler68[152];
} identify;

#if CONF_WITH_SCSI_DRIVER
static struct {
    UBYTE error_code;
    UBYTE segment;
    UBYTE sense_key;
    UBYTE information[4];
    UBYTE addl_length;
    ULONG cmd_specific;
    UBYTE asc;
    UBYTE ascq;
    UBYTE fru_code;
    UBYTE sense_specific[3];
} reqsense;

static struct {             /* space to build return data for Inquiry */
    UBYTE devtype;
    UBYTE devtype_mod;
    UBYTE versions;
    UBYTE response_fmt;
    UBYTE addl_length;
    UBYTE resvd;
    UBYTE flags1;
    UBYTE flags2;
    UBYTE vendor_id[8];
    UBYTE product_id[16];
    UBYTE product_rev[4];
} inquiry;
#endif


/* prototypes */
static WORD clear_multiple_mode(UWORD ifnum,UWORD dev);
static void ide_detect_devices(UWORD ifnum);
static LONG ata_identify(WORD dev);
static int ide_select_device(volatile struct IDE *interface,UWORD dev);
static void set_multiple_mode(WORD dev,UWORD multi_io);
static void set_lba48_mode(WORD dev, UWORD lba48);
static UWORD get_start_count(volatile struct IDE *interface);
static void set_start_count(volatile struct IDE *interface,UBYTE sector,UBYTE count);
static int wait_for_not_BSY(volatile struct IDE *interface,LONG timeout);

#if CONF_WITH_SCSI_DRIVER
static LONG ata_request_sense(WORD dev,WORD buflen,UBYTE *buffer);
static LONG atapi_identify(WORD dev);
static void set_packet_size(WORD dev,UWORD config);
#endif

/*
 * some add-on IDE interfaces for Atari systems do not completely
 * decode the address of the interface, causing it to appear at
 * multiple locations.  since multiple interfaces are also available
 * for these systems, we must distinguish between the two situations.
 * the following routines do this.
 *
 * we do not check for the FireBee, since there are always exactly
 * two interfaces, or for non-Atari hardware.
 */
#if CONF_ATARI_HARDWARE && !defined(MACHINE_FIREBEE)

/* used by duplicate interface detection logic */
#define SECNUM_MAGIC    0xcc
#define SECCNT_MAGIC    0x33

/*
 * set a special value in the sector number/count registers of
 * device 0 in the specified interface
 *
 */
static void set_interface_magic(volatile struct IDE *interface, WORD ifnum)
{
    UBYTE secnum = SECNUM_MAGIC + ifnum;
    UBYTE seccnt = SECCNT_MAGIC + ifnum;

    IDE_WRITE_HEAD(interface,IDE_DEVICE(0));
    DELAY_400NS;
    IDE_WRITE_SECTOR_NUMBER_SECTOR_COUNT(interface,secnum,seccnt);
}

/*
 * check for special value in the sector number/count
 * registers of device 0 in the specified interface
 *
 * returns 1 if OK, 0 otherwise
 */
static int check_interface_magic(volatile struct IDE *interface, WORD ifnum)
{
    UWORD numcnt = MAKE_UWORD(SECNUM_MAGIC + ifnum, SECCNT_MAGIC + ifnum);

    if (IDE_READ_SECTOR_NUMBER_SECTOR_COUNT(interface) == numcnt)
        return 1;

    return 0;
}

/*
 * check if an interface is a ghost by checking if it has
 * overwritten the magic number of a previous interface
*/
static int ide_interface_is_ghost(WORD ifnum)
{
    int i, bitmask;

    for (i = 0, bitmask = 1; i < ifnum; i++, bitmask <<= 1) {
        if (has_ide&bitmask) {
            /* check a previous interface against the magic of the current interface */
            volatile struct IDE *interface = ifinfo[i].base_address;
            if (check_interface_magic(interface, ifnum))
                return 1;
        }
    }
    return 0;
}

/* Enum to capture interface status during ide_interface_exists(). */
enum ide_if_status
{
    IDE_IF_NOTCHECKED,
    IDE_IF_NOTPRESENT,
    IDE_IF_ISGHOST,
    IDE_IF_PRESENT
};

/*
 * determine if a specific interface really exists, allowing for
 * incomplete hardware address decoding and twisted cables
 *
 * method:
 * as soon as the BSY bit on an interface is low:
 *    write a magic number (dependent on the interface number) to
 *    the sector number/count registers and check...
 *    a. if the magic number is read back correctly and it hasn't
 *       written its magic number on a previous interface
 *       => device found
 *    b. if the magic number is read back correctly but it has
 *       written its own magic number on a previous interface
 *       => ghost interface
 *    c. if the magic number is not read back correctly
 *       => no device present
 * break if...
 *    a. a device is found
 *    b. no device is found on both regular and twisted interfaces
 *    c. a timeout occurs, i.e., both interfaces stayed BSY
 */
static int ide_interface_exists(WORD ifnum, LONG timeout)
{
    volatile struct IDE *regular_iface = ifinfo[ifnum].base_address;
    volatile struct IDE *twisted_iface = (volatile struct IDE *)(((ULONG)ifinfo[ifnum].base_address)-1);
    enum ide_if_status regular_iface_status = IDE_IF_NOTCHECKED;
    enum ide_if_status twisted_iface_status = IDE_IF_NOTPRESENT;
    BOOL allow_twisted = check_read_byte((long)&twisted_iface->control);

    IDE_WRITE_CONTROL(regular_iface,IDE_CONTROL_nIEN);/* no interrupts please */
    if (allow_twisted) {
        /* Registers for potential "twisted" interface are accessible. */
        IDE_WRITE_CONTROL(twisted_iface,IDE_CONTROL_nIEN);/* no interrupts please */
        twisted_iface_status = IDE_IF_NOTCHECKED;
    }

    DELAY_400NS;
    do {
        /* Check BSY on regular interface. */
        if ((IDE_READ_ALT_STATUS(regular_iface) & IDE_STATUS_BSY) == 0) {
            /* Check it exists by setting and reading back magic number. */
            KDEBUG(("checking ide interface %d\n", ifnum));
            set_interface_magic(regular_iface, ifnum);
            if (check_interface_magic(regular_iface, ifnum)) {
                ifinfo[ifnum].twisted_cable = FALSE;
                regular_iface_status = IDE_IF_PRESENT;
                /* Check that it is not a ghost interface. */
                if ((ifnum > 0) && ide_interface_is_ghost(ifnum)) {
                    regular_iface_status = IDE_IF_ISGHOST;
                }
                break;
            } else {
                regular_iface_status = IDE_IF_NOTPRESENT;
            }
        }

        /* Check BSY on twisted interface. */
        if (allow_twisted && ((IDE_READ_ALT_STATUS(twisted_iface) & IDE_STATUS_BSY) == 0)) {
            /* Check it exists by setting and reading back magic number. */
            KDEBUG(("checking ide interface %d with twisted cable\n", ifnum));
            set_interface_magic(twisted_iface, ifnum);
            if (check_interface_magic(twisted_iface, ifnum)) {
                ifinfo[ifnum].base_address = twisted_iface;
                ifinfo[ifnum].twisted_cable = TRUE;
                twisted_iface_status = IDE_IF_PRESENT;
                /* Check that it is not a ghost interface. */
                if ((ifnum > 0) && ide_interface_is_ghost(ifnum)) {
                    twisted_iface_status = IDE_IF_ISGHOST;
                }
                break;
            } else {
                twisted_iface_status = IDE_IF_NOTPRESENT;
            }
        }
    } while ((hz_200 < timeout) &&
             ((regular_iface_status == IDE_IF_NOTCHECKED) || (twisted_iface_status == IDE_IF_NOTCHECKED)));

    int rc = (regular_iface_status == IDE_IF_PRESENT) || (twisted_iface_status == IDE_IF_PRESENT);
    KDEBUG(("ide interface %d %s %s\n",ifnum,rc?"exists":"not present",ifinfo[ifnum].twisted_cable?"(twisted cable)":""));

    return rc;
}
#endif

BOOL detect_ide(void)
{
    int i, bitmask;

    MAYBE_UNUSED(bitmask);

    for (i = 0; i < NUM_IDE_INTERFACES; i++)
    {
        /* initialize base addresses for IDE interface */
        ifinfo[i].base_address = ide_interface + i;
        ifinfo[i].twisted_cable = FALSE;
    }

#ifdef MACHINE_AMIGA
    has_ide = has_gayle ? 0x01 : 0x00;
#elif defined(MACHINE_M548X)
    has_ide = 0x01;
#elif defined(MACHINE_FIREBEE)
    has_ide = 0x03;
#elif CONF_ATARI_HARDWARE

    /*
     * see if the IDE registers for possible interfaces are accessible.
     * note that we may detect multiple interfaces if the IDE adaptor has
     * incomplete address decoding.  this is resolved later in ide_init().
     */
    for (i = 0, bitmask = 1, has_ide = 0; i < NUM_IDE_INTERFACES; i++, bitmask <<= 1)
    {
        if (check_read_byte((long)&ifinfo[i].base_address->command))
            has_ide |= bitmask;

        if (IS_ARANYM)
        {
            /* ARAnyM-JIT crashes when detecting extra IDE interfaces */
            break;
        }
    }
#else
    has_ide = 0x00;
#endif

    KDEBUG(("detect_ide(): has_ide = 0x%02x\n",has_ide));

    return has_ide ? TRUE : FALSE;
}

/*
 * perform any one-time initialisation required
 *
 * for Atari hardware, this includes rejection of 'ghost' interfaces
 * (due to incomplete address decoding), and detection of twisted cables
 *
 * this is called late on in bios initialisation, so delay calibration
 * has already been done, and the system timer is running
 */
void ide_init(void)
{
    int i, bitmask;

    delay400ns = loopcount_1_msec / 2500;
    delay5us = loopcount_1_msec / 200;

    if (!has_ide)
        return;

#if CONF_ATARI_HARDWARE && !defined(MACHINE_FIREBEE)
    /* Reject 'ghost' interfaces & detect twisted cables.
     * We wait a max time for BSY to drop on all IDE interface
     * since this is called during initialisation, which can be
     * invoked by power-on/reset.
     */
    LONG timeout = hz_200 + LONG_TIMEOUT;
    for (i = 0, bitmask = 1; i < NUM_IDE_INTERFACES; i++, bitmask <<= 1)
        if (has_ide&bitmask)
            if (!ide_interface_exists(i, timeout))
                has_ide &= ~bitmask;

    KDEBUG(("ide_init(): has_ide = 0x%02x\n",has_ide));
#endif

    /* detect devices */
    for (i = 0, bitmask = 1; i < NUM_IDE_INTERFACES; i++, bitmask <<= 1)
        if (has_ide&bitmask)
            ide_detect_devices(i);

    /* set multiple mode for all devices that we have info for */
    for (i = 0; i < DEVICES_PER_BUS; i++)
        if (ata_identify(i) == 0) {
            set_multiple_mode(i,identify.multiple_io_info);
            set_lba48_mode(i,identify.cmds_supported[1]);
        }

#if CONF_WITH_SCSI_DRIVER
    /* set packet size for all ATAPI devices */
    for (i = 0; i < DEVICES_PER_BUS; i++)
        if (atapi_identify(i) == 0)
            set_packet_size(i,identify.general_config);
#endif
}

/*
 * return the type of device on the IDE bus
 *
 * returns DEVTYPE_ATA, DEVTYPE_ATAPI or DEVTYPE_NONE; the latter is
 * returned if the bus doesn't exist, or the device doesn't exist, or
 * the device type is unknown
 */
static UWORD ide_device_type(WORD dev)
{
    WORD ifnum;
    UWORD type;

    ifnum = dev / 2;/* i.e. primary IDE, secondary IDE, ... */
    dev &= 1;       /* 0 or 1 */

    if (!(has_ide & (1<<ifnum)))    /* interface does not exist */
        return DEVTYPE_NONE;

    type = ifinfo[ifnum].dev[dev].type;

    if ((type == DEVTYPE_ATA) || (type == DEVTYPE_ATAPI))
        return type;

    return DEVTYPE_NONE;
}

/*
 * the following routines for device type detection are adapted
 * from Hale Landis's public domain ATA driver, MINDRVR.
 */
static int wait_for_not_BSY_and_DRDY(volatile struct IDE *interface,LONG timeout)
{
    LONG next = hz_200 + timeout;

    DELAY_400NS;
    while(hz_200 < next) {
        if ((IDE_READ_ALT_STATUS(interface) & (IDE_STATUS_BSY|IDE_STATUS_DRDY)) == IDE_STATUS_DRDY)
            return 0;
    }

    KDEBUG(("Timeout in wait_for_not_BSY_and_DRDY(%p,%ld)\n",interface,timeout));
    return 1;
}

static void ide_reset(UWORD ifnum)
{
    struct IFINFO *info = ifinfo + ifnum;
    volatile struct IDE *interface = info->base_address;

    /* set, then reset, the soft reset bit */
    IDE_WRITE_CONTROL(interface,(IDE_CONTROL_SRST|IDE_CONTROL_nIEN));
    DELAY_5US;
    IDE_WRITE_CONTROL(interface,IDE_CONTROL_nIEN);
    DELAY_400NS;

    /* if at least one device exists, wait for it to clear BSY and set DRDY */
    if ((info->dev[0].type != DEVTYPE_NONE)
     || (info->dev[1].type != DEVTYPE_NONE))
        wait_for_not_BSY_and_DRDY(interface,LONG_TIMEOUT);
}

static UBYTE ide_decode_type(UBYTE status,UWORD signature)
{
    if (signature == 0xeb14)    /* PATAPI */
        return DEVTYPE_ATAPI;
    if (signature == 0x9669)    /* SATAPI */
        return DEVTYPE_ATAPI;

    if (status == 0)
        return DEVTYPE_UNKNOWN;

    if (signature == 0x0000)    /* PATA */
        return DEVTYPE_ATA;
    if (signature == 0xc33c)    /* SATA */
        return DEVTYPE_ATA;

    return DEVTYPE_UNKNOWN;
}

static void ide_detect_devices(UWORD ifnum)
{
    volatile struct IDE *interface = ifinfo[ifnum].base_address;
    struct IFINFO *info = ifinfo + ifnum;
    UBYTE status;
    UWORD signature;
    int i;

    MAYBE_UNUSED(interface);

    IDE_WRITE_CONTROL(interface,IDE_CONTROL_nIEN);    /* no interrupts please */

    /* initial check for devices */
    for (i = 0; i < 2; i++) {
        ide_select_device(interface,i);
        set_start_count(interface,0xaa,0x55);
        set_start_count(interface,0x55,0xaa);
        set_start_count(interface,0xaa,0x55);
        if (get_start_count(interface) == 0xaa55) {
            info->dev[i].type = DEVTYPE_UNKNOWN;
            KDEBUG(("IDE i/f %d device %d detected\n",ifnum,i));
        } else
            info->dev[i].type = DEVTYPE_NONE;
        info->dev[i].options = 0;
        info->dev[i].spi = 0;   /* changed if using READ/WRITE MULTIPLE */
#if CONF_WITH_SCSI_DRIVER
        info->dev[i].sense = 0;
#endif
    }

    /* recheck after soft reset, also detect ata/atapi */
    ide_select_device(interface,0);
    ide_reset(ifnum);

    for (i = 0; i < 2; i++) {
        ide_select_device(interface,i);
        if (get_start_count(interface) == 0x0101) {
            status = IDE_READ_STATUS(interface);
            signature = IDE_READ_CYLINDER_HIGH_CYLINDER_LOW(interface);
            info->dev[i].type = ide_decode_type(status,signature);
        }
    }

    for (i = 0; i < 2; i++)
        KDEBUG(("IDE i/f %d device %d is type %d\n",ifnum,i,info->dev[i].type));
}

/*
 * the following code is intended to follow the PIO data transfer diagrams
 * as shown in the X3T10 specifications for the ATA/ATAPI interface.  note
 * that the transfer diagrams differ slightly between different versions
 * of the interface, so this is my choice of what to believe in :-)  RFB
 */

/*
 * wait for access to IDE registers
 */
static int wait_for_not_BSY(volatile struct IDE *interface,LONG timeout)
{
    LONG next = hz_200 + timeout;

    KDEBUG(("wait_for_not_BSY(%p, %ld)\n", interface, timeout));

    DELAY_400NS;
    while(hz_200 < next) {
        if ((IDE_READ_ALT_STATUS(interface) & IDE_STATUS_BSY) == 0)
            return 0;
    }

    KDEBUG(("Timeout in wait_for_not_BSY(%p,%ld)\n",interface,timeout));
    return 1;
}

static int wait_for_not_BSY_not_DRQ(volatile struct IDE *interface,LONG timeout)
{
    LONG next = hz_200 + timeout;

    DELAY_400NS;
    while(hz_200 < next) {
        if ((IDE_READ_ALT_STATUS(interface) & (IDE_STATUS_BSY|IDE_STATUS_DRQ)) == 0)
            return 0;
    }

    KDEBUG(("Timeout in wait_for_not_BSY_not_DRQ(%p,%ld)\n",interface,timeout));
    return 1;
}

/*
 * select device in IDE registers
 */
static int ide_select_device(volatile struct IDE *interface,UWORD dev)
{
    KDEBUG(("ide_select_device(%p, %u)\n", interface, dev));

    if (wait_for_not_BSY_not_DRQ(interface,SHORT_TIMEOUT))
        return ERR;

    IDE_WRITE_HEAD(interface,IDE_DEVICE(dev));

    if (wait_for_not_BSY_not_DRQ(interface,SHORT_TIMEOUT))
        return ERR;

    return E_OK;
}

/*
 * get sector number / sector count IDE registers
 */
static UWORD get_start_count(volatile struct IDE *interface)
{
    wait_for_not_BSY_not_DRQ(interface,SHORT_TIMEOUT);
    return IDE_READ_SECTOR_NUMBER_SECTOR_COUNT(interface);
}

/*
 * set sector number / sector count IDE registers
 */
static void set_start_count(volatile struct IDE *interface,UBYTE sector,UBYTE count)
{
    wait_for_not_BSY_not_DRQ(interface,SHORT_TIMEOUT);
    IDE_WRITE_SECTOR_NUMBER_SECTOR_COUNT(interface,sector,count);
}

/*
 * set cylinder high / cylinder low IDE registers
 */
static void set_cylinder(volatile struct IDE *interface,UWORD cylinder)
{
    wait_for_not_BSY_not_DRQ(interface,SHORT_TIMEOUT);
    IDE_WRITE_CYLINDER_HIGH_CYLINDER_LOW(interface,cylinder);
}

/*
 * set command / head IDE registers
 */
static void set_command_head(volatile struct IDE *interface,UBYTE cmd,UBYTE head)
{
    wait_for_not_BSY_not_DRQ(interface,SHORT_TIMEOUT);
    IDE_WRITE_COMMAND_HEAD(interface,cmd,head);
}

/*
 * set device / command / sector start / count / LBA mode in IDE registers
 */
static void ide_rw_start(volatile struct IDE *interface,UWORD dev,ULONG sector,UWORD count,UBYTE cmd)
{
    KDEBUG(("ide_rw_start(%p, %u, %lu, %u, 0x%02x)\n", interface, dev, sector, count, cmd));

    if (cmd == IDE_CMD_READ_SECTOR_EX ||
        cmd == IDE_CMD_WRITE_SECTOR_EX ||
        cmd == IDE_CMD_READ_MULTIPLE_EX ||
        cmd == IDE_CMD_WRITE_MULTIPLE_EX) {
        set_start_count(interface,(sector>>24),(count>>8));
        set_start_count(interface,(sector),(count));

        /*
         * We should do this, but for now we only support 2^32
         * sector sizes, i.e. max 2TB disks. It would mean using
         * ULLONG for sector to support 2^48 disk sizes.
         * Additionally, XHDI, etc would need extensions.
         *
         *   set_cylinder(interface,(UWORD)((sector & 0xffff00000000UL) >> 32));
         */
        set_cylinder(interface,0);

        set_cylinder(interface,(UWORD)((sector & 0xffff00) >> 8));
        set_command_head(interface,cmd,IDE_MODE_LBA|IDE_DEVICE(dev));
    } else {
        set_start_count(interface,LOBYTE(sector),LOBYTE(count));
        set_cylinder(interface,(UWORD)((sector & 0xffff00) >> 8));
        set_command_head(interface,cmd,IDE_MODE_LBA|IDE_DEVICE(dev)|(UBYTE)((sector>>24)&0x0f));
    }
}

/*
 * perform a non-data-transfer command
 */
static LONG ide_nodata(UBYTE cmd,UWORD ifnum,UWORD dev,ULONG sector,UWORD count)
{
    volatile struct IDE *interface = ifinfo[ifnum].base_address;
    UBYTE status;

    if (ide_select_device(interface,dev) < 0)
        return EGENRL;

    ide_rw_start(interface,dev,sector,count,cmd);

    if (wait_for_not_BSY(interface,SHORT_TIMEOUT))  /* should vary depending on command? */
        return EGENRL;

    status = IDE_READ_STATUS(interface);     /* status, clear pending interrupt */
    if (status & (IDE_STATUS_BSY|IDE_STATUS_DF|IDE_STATUS_DRQ|IDE_STATUS_ERR))
        return EGENRL;

    return E_OK;
}

#if CONF_WITH_APOLLO_68080
/* Apollo IDE data register can be read (but not written) using 32-bit access */
static void ide_get_data_32(volatile struct IDE *interface,UBYTE *buffer,ULONG bufferlen,int need_byteswap)
{
    ULONG *p = (ULONG *)buffer;
    ULONG *end = (ULONG *)(buffer + (bufferlen & ~(4-1)));
    UWORD *p2;
    UWORD *end2 = (UWORD *)(buffer + bufferlen);
    volatile ULONG_ALIAS *pdatareg = (volatile ULONG_ALIAS *)&interface->data;

    if (need_byteswap) {
        while (p < end) {
            ULONG temp;
            temp = *pdatareg;
            swpw2(temp);
            *p++ = temp;
        }
    } else {
        while (p < end)
            *p++ = *pdatareg;
    }

    /* transfer remaining word (if any) */
    p2 = (UWORD *)p;
    if (p2 < end2) {
        UWORD temp;
        temp = interface->data;
        if (need_byteswap)
            swpw(temp);
        *p2 = temp;
    }
}
#endif /* CONF_WITH_APOLLO_68080 */

/*
 * get data from IDE device
 */
static void ide_get_data(volatile struct IDE *interface,UBYTE *buffer,ULONG bufferlen,int need_byteswap)
{
    XFERWIDTH *p = (XFERWIDTH *)buffer;
    XFERWIDTH *end;
    UWORD *p2;
    UWORD *end2 = (UWORD *)(buffer + bufferlen);

    KDEBUG(("ide_get_data(%p, %p, %lu, %d)\n", interface, buffer, bufferlen, need_byteswap));

#if CONF_WITH_APOLLO_68080
    if (is_apollo_68080)
    {
        ide_get_data_32(interface, buffer, bufferlen, need_byteswap);
        return;
    }
#endif

    if (need_byteswap) {
        end = (XFERWIDTH *)(buffer + (bufferlen & ~(16-1)));    /* mask must match unrolled loop */
        while (p < end) {
            XFERWIDTH temp;

            /* Unroll the loop 4 times, transferring 8/16 bytes in a row. */
            temp = interface->data;
            xferswap(temp);
            *p++ = temp;

            temp = interface->data;
            xferswap(temp);
            *p++ = temp;

            temp = interface->data;
            xferswap(temp);
            *p++ = temp;

            temp = interface->data;
            xferswap(temp);
            *p++ = temp;
        }

        /* transfer remainder 2 bytes at a time */
        p2 = (UWORD *)p;
        while (p2 < end2) {
            UWORD temp;

            temp = *(UWORD_ALIAS *)&interface->data;
            swpw(temp);
            *p2++ = temp;
        }
    } else {
        end = (XFERWIDTH *)(buffer + (bufferlen & ~(64-1)));    /* mask must match unrolled loop */
        while (p < end) {
            /* Unroll the loop 16 times, transferring 32/64 bytes in a row.
             * Note that the pointer p gets incremented implicitly.
             */
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);

            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);

            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);

            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
            ide_get_and_incr(&(interface->data), p);
        }
        /* transfer remainder 2 bytes at a time */
        p2 = (UWORD *)p;
        while (p2 < end2) {
            *p2++ = *(UWORD_ALIAS *)&interface->data;
        }
    }
}

/*
 * read from the IDE device
 */
static LONG ide_read(UBYTE cmd,UWORD ifnum,UWORD dev,ULONG sector,UWORD count,UBYTE *buffer,BOOL need_byteswap)
{
    volatile struct IDE *interface = ifinfo[ifnum].base_address;
    struct IFINFO *info = ifinfo + ifnum;
    UWORD spi;
    UBYTE status1, status2;
    LONG rc = 0L;

    KDEBUG(("ide_read(0x%02x, %u, %u, %lu, %u, %p, %d)\n", cmd, ifnum, dev, sector, count, buffer, need_byteswap));

    if (ide_select_device(interface,dev) < 0)
        return EREADF;

    /*
     * if READ SECTOR and MULTIPLE MODE, set cmd & spi accordingly
     *
     * If the sector is > LBA28 range use LBA48 if supported.
     * This aids performance in for LBA28 addressable sectors.
     */
    spi = 1;
    if ((cmd == IDE_CMD_READ_SECTOR)
     && (info->dev[dev].options & MULTIPLE_MODE_ACTIVE)) {
        if ((info->dev[dev].options & LBA48_ACTIVE) &&
            (sector > 0x0FFFFFFFUL)) {
            cmd = IDE_CMD_READ_MULTIPLE_EX;
        } else {
            cmd = IDE_CMD_READ_MULTIPLE;
        }
        spi = info->dev[dev].spi;
        KDEBUG(("spi=%u\n", spi));
    }

    if ((info->dev[dev].options & LBA48_ACTIVE) &&
        (sector > 0x0FFFFFFFUL) &&
        (cmd == IDE_CMD_READ_SECTOR)) {
       cmd = IDE_CMD_READ_SECTOR_EX;
    }

    ide_rw_start(interface,dev,sector,count,cmd);

    /*
     * each iteration of this loop handles one DRQ block
     */
    while (count > 0)
    {
        UWORD numsecs;
        ULONG xferlen;

        if (wait_for_not_BSY(interface,XFER_TIMEOUT))
            return EREADF;

        status1 = IDE_READ_ALT_STATUS(interface);/* alternate status, ignore */
        status1 = IDE_READ_STATUS(interface);    /* status, clear pending interrupt */

        numsecs = (count>spi) ? spi : count;
        xferlen = numsecs * SECTOR_SIZE;

        rc = E_OK;
        if (status1 & IDE_STATUS_DRQ) {
            if (info->twisted_cable) {
                ide_get_data((volatile struct IDE *)(((ULONG)interface)+1),buffer,xferlen,need_byteswap);
            } else {
                ide_get_data(interface,buffer,xferlen,need_byteswap);
            }
        } else {
            rc = EREADF;
        }
        if (status1 & (IDE_STATUS_DF | IDE_STATUS_ERR))
            rc = EREADF;

        if (rc)
            break;

        buffer += xferlen;
        count -= numsecs;
    }

    /*
     * the following was added to handle an ATAPI DVD recorder that left
     * BSY asserted for more than 5 microseconds after the last word of
     * data from an ATAPI IDENTIFY command was received.  it should be
     * harmless in general.
     */
    if (wait_for_not_BSY(interface,SHORT_TIMEOUT))
        return EREADF;

    status2 = IDE_READ_ALT_STATUS(interface);    /* alternate status, ignore */
    status2 = IDE_READ_STATUS(interface);        /* status, clear pending interrupt */

    if (status2 & (IDE_STATUS_BSY|IDE_STATUS_DF|IDE_STATUS_DRQ|IDE_STATUS_ERR))
        rc = EREADF;

    return rc;
}

/*
 * send data to IDE device
 */
static void ide_put_data(volatile struct IDE *interface,UBYTE *buffer,ULONG bufferlen,int need_byteswap)
{
    XFERWIDTH *p = (XFERWIDTH *)buffer;
    XFERWIDTH *end;
    UWORD *p2;
    UWORD *end2 = (UWORD *)(buffer + bufferlen);

    if (need_byteswap) {
        end = (XFERWIDTH *)(buffer + (bufferlen & ~(16-1)));    /* mask must match unrolled loop */
        while (p < end) {
            XFERWIDTH temp;

            /* Unroll the loop 4 times, transferring 8/16 bytes in a row. */
            temp = *p++;
            xferswap(temp);
            interface->data = temp;

            temp = *p++;
            xferswap(temp);
            interface->data = temp;

            temp = *p++;
            xferswap(temp);
            interface->data = temp;

            temp = *p++;
            xferswap(temp);
            interface->data = temp;
        }

        /* transfer remainder 2 bytes at a time */
        p2 = (UWORD *)p;
        while (p2 < end2) {
            UWORD temp;

            temp = *p2++;
            swpw(temp);
            *(UWORD_ALIAS *)&interface->data = temp;
        }
    } else {
        end = (XFERWIDTH *)(buffer + (bufferlen & ~(64-1)));    /* mask must match unrolled loop */
        while (p < end) {
            /* Unroll the loop 16 times, transferring 32/64 bytes in a row.
             * Note that the pointer p gets incremented implicitly.
             */
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));

            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));

            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));

            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
            ide_put_and_incr(p, &(interface->data));
        }
        /* transfer remainder 2 bytes at a time */
        p2 = (UWORD *)p;
        while (p2 < end2) {
            *(UWORD_ALIAS *)&interface->data = *p2++;
        }
    }
}

/*
 * write to the IDE device
 */
static LONG ide_write(UBYTE cmd,UWORD ifnum,UWORD dev,ULONG sector,UWORD count,UBYTE *buffer,BOOL need_byteswap)
{
    volatile struct IDE *interface = ifinfo[ifnum].base_address;
    struct IFINFO *info = ifinfo + ifnum;
    UWORD spi;
    UBYTE status1, status2;
    LONG rc = 0L;

    KDEBUG(("ide_write(0x%02x, %u, %u, %lu, %u, %p, %d)\n", cmd, ifnum, dev, sector, count, buffer, need_byteswap));

    if (ide_select_device(interface,dev) < 0)
        return EWRITF;

    /*
     * if WRITE SECTOR and MULTIPLE MODE, set cmd & spi accordingly
     *
     * If the sector is > LBA28 range use LBA48 if supported.
     * This aids performance in for LBA28 addressable sectors.
     */
    spi = 1;
    if ((cmd == IDE_CMD_WRITE_SECTOR)
     && (info->dev[dev].options & MULTIPLE_MODE_ACTIVE)) {
        if ((info->dev[dev].options & LBA48_ACTIVE) &&
            (sector > 0x0FFFFFFFUL))
            cmd = IDE_CMD_WRITE_MULTIPLE_EX;
        else
            cmd = IDE_CMD_WRITE_MULTIPLE;
        spi = info->dev[dev].spi;
    }

    if ((info->dev[dev].options & LBA48_ACTIVE) &&
        (sector > 0x0FFFFFFFUL) &&
        (cmd == IDE_CMD_WRITE_SECTOR)) {
       cmd = IDE_CMD_WRITE_SECTOR_EX;
    }

    ide_rw_start(interface,dev,sector,count,cmd);

    if (wait_for_not_BSY(interface,SHORT_TIMEOUT))
        return EWRITF;

    /*
     * each iteration of this loop handles one DRQ block
     */
    while (count > 0)
    {
        UWORD numsecs;
        ULONG xferlen;

        numsecs = (count>spi) ? spi : count;
        xferlen = numsecs * SECTOR_SIZE;

        rc = E_OK;
        status1 = IDE_READ_STATUS(interface);    /* status, clear pending interrupt */
        if (status1 & IDE_STATUS_DRQ) {
            if (info->twisted_cable) {
                ide_put_data((volatile struct IDE *)(((ULONG)interface)+1),buffer,xferlen,need_byteswap);
            } else {
                ide_put_data(interface,buffer,xferlen,need_byteswap);
            }
        } else {
            rc = EWRITF;
        }
        if (status1 & (IDE_STATUS_DF|IDE_STATUS_ERR))
            rc = EWRITF;

        if (wait_for_not_BSY(interface,XFER_TIMEOUT))   /* while device processes data */
            return EWRITF;

        if (rc)
            break;

        buffer += xferlen;
        count -= numsecs;
    }

    status2 = IDE_READ_ALT_STATUS(interface);    /* alternate status (ignore) */
    status2 = IDE_READ_STATUS(interface);        /* status, clear pending interrupt */
    if (status2 & (IDE_STATUS_BSY|IDE_STATUS_DF|IDE_STATUS_DRQ|IDE_STATUS_ERR))
        rc = EWRITF;

    return rc;
}

LONG ide_rw(WORD rw,ULONG sector,UWORD count,UBYTE *buf,WORD dev,BOOL need_byteswap)
{
    UBYTE *p;
    UWORD ifnum;
    WORD maxsecs_per_io = MAXSECS_PER_IO;
    BOOL use_tmpbuf = FALSE;
    LONG ret;

    if (ide_device_type(dev) != DEVTYPE_ATA)
        return EUNDEV;

    ifnum = dev / 2;/* i.e. primary IDE, secondary IDE, ... */
    dev &= 1;       /* 0 or 1 */

    rw &= RW_RW;    /* we just care about read or write for now */

    /*
     * because ide_read()/ide_write() access the buffer with word (or long)
     * moves, we must use an intermediate buffer if the user buffer is not
     * word-aligned, and the processor is a 68000 or 68010
     */
#ifndef __mcoldfire__
    if (IS_ODD_POINTER(buf) && (mcpu < 20))
    {
        if (maxsecs_per_io > DSKBUF_SECS)
            maxsecs_per_io = DSKBUF_SECS;
        use_tmpbuf = TRUE;
    }
#endif

    while (count > 0)
    {
        UWORD numsecs;

        numsecs = (count>maxsecs_per_io) ? maxsecs_per_io : count;

        p = use_tmpbuf ? dskbufp : buf;
        if (rw && use_tmpbuf)
            memcpy(p,buf,numsecs*SECTOR_SIZE);

        ret = rw ? ide_write(IDE_CMD_WRITE_SECTOR,ifnum,dev,sector,numsecs,p,need_byteswap)
                : ide_read(IDE_CMD_READ_SECTOR,ifnum,dev,sector,numsecs,p,need_byteswap);
        if (ret < 0) {
            KDEBUG(("ide_rw(%d,%d,%d,%lu,%u,%p,%d) ret=%ld\n",
                    rw,ifnum,dev,sector,numsecs,p,need_byteswap,ret));
            if (clear_multiple_mode(ifnum,dev)) /* retry after multiple mode reset ? */
                continue;                       /* yes, do so                        */
            return ret;
        }

        if (!rw && use_tmpbuf)
            memcpy(buf,p,numsecs*SECTOR_SIZE);

        buf += numsecs*SECTOR_SIZE;
        sector += numsecs;
        count -= numsecs;
    }

    return E_OK;
}

static void set_lba48_mode(WORD dev, UWORD lba48)
{
    UWORD ifnum;

    if (!(lba48 & IDE_CAPABILITY_LBA48))
        return;

    ifnum = dev / 2;    /* i.e. primary IDE, secondary IDE, ... */
    dev &= 1;           /* 0 or 1 */

    KDEBUG(("Setting lba48 for ifnum %d dev %d\n",ifnum,dev));

    ifinfo[ifnum].dev[dev].options |= LBA48_ACTIVE;
}

/*
 * if multiple mode is set, and ABRT is set in the error register,
 * clear multiple mode and return TRUE to request a retry.
 * otherwise, return FALSE
 */
static WORD clear_multiple_mode(UWORD ifnum,UWORD dev)
{
    volatile struct IDE *interface = ifinfo[ifnum].base_address;
    struct IFINFO *info = ifinfo + ifnum;

    MAYBE_UNUSED(interface);

    if ((info->dev[dev].options&MULTIPLE_MODE_ACTIVE) == 0)
        return 0;

    if ((IDE_READ_ERROR(interface)&IDE_ERROR_ABRT) == 0)
        return 0;

    KDEBUG(("Clearing multiple sector mode for ifnum %d dev %d\n",ifnum,dev));

    info->dev[dev].options &= ~MULTIPLE_MODE_ACTIVE;
    return 1;
}

static void set_multiple_mode(WORD dev,UWORD multi_io)
{
    UWORD ifnum;
    UBYTE spi;

    if (!(multi_io & 0x8000))
        return;

    spi = LOBYTE(multi_io);
    if (spi < 2)        /* 0 => not supported (ATA 2 & earlier) */
        return;         /* 1 => no benefit in using it          */

    ifnum = dev / 2;    /* i.e. primary IDE, secondary IDE, ... */
    dev &= 1;           /* 0 or 1 */

    KDEBUG(("Setting spi=%d for ifnum %d dev %d\n",spi,ifnum,dev));

    if (ide_nodata(IDE_CMD_SET_MULTIPLE_MODE,ifnum,dev,0L,spi))
        return;         /* command failed */

    ifinfo[ifnum].dev[dev].options |= MULTIPLE_MODE_ACTIVE;
    ifinfo[ifnum].dev[dev].spi = spi;
}

static LONG ata_identify(WORD dev)
{
    LONG ret;
    UWORD ifnum, ifdev;

    ifnum = dev / 2;    /* i.e. primary IDE, secondary IDE, ... */
    ifdev = dev & 1;    /* 0 or 1 */

    KDEBUG(("ata_identify(%d [ifnum=%d ifdev=%d])\n", dev, ifnum, ifdev));

    /* with twisted cable the response of IDENTIFY_DEVICE will be byte-swapped */
    if (ide_device_type(dev) == DEVTYPE_ATA) {
        ret = ide_read(IDE_CMD_IDENTIFY_DEVICE,ifnum,ifdev,0L,1,(UBYTE *)&identify,
                       ifinfo[ifnum].twisted_cable != IDE_DATA_REGISTER_IS_BYTESWAPPED);
    } else ret = EUNDEV;

    if (ret < 0)
        KDEBUG(("ata_identify(%d [ifnum=%d ifdev=%d]) ret=%ld\n", dev, ifnum, ifdev, ret));

    return ret;
}

/*
 *  perform miscellaneous non-data-transfer functions
 */
LONG ide_ioctl(WORD dev,UWORD ctrl,void *arg)
{
    LONG ret = ERR;
    ULONG *info = arg;
    int i;

    switch(ctrl) {
    case GET_DISKINFO:
        ret = ata_identify(dev);    /* reads into identify structure */
        if (ret >= 0) {
            if (identify.cmds_supported[1] & IDE_CAPABILITY_LBA48) {
                if (identify.maxsec_lba48[2] ||
                    identify.maxsec_lba48[3]) {
                    /*
                     * Should we return an error here ?
                     */
                    KDEBUG(("Disk size >= 2^32 sectors, unsupported\n"));
                }

                info[0] = MAKE_ULONG(identify.maxsec_lba48[1],
                                     identify.maxsec_lba48[0]);
            } else {
                info[0] = MAKE_ULONG(identify.numsecs_lba28[1],
                                     identify.numsecs_lba28[0]);
            }
            info[1] = SECTOR_SIZE;  /* note: could be different under ATAPI 7 */
            ret = E_OK;
        }
        break;
    case GET_DISKNAME:
        ret = ata_identify(dev);    /* reads into identify structure */
        if (ret >= 0) {
            identify.model_number[39] = 0;  /* null terminate string */

            /* remove right padding with spaces */
            for (i = 38; i >= 0 && identify.model_number[i] == ' '; i--)
                identify.model_number[i] = 0;

            strcpy(arg,identify.model_number);
            ret = E_OK;
        }
        break;
    case GET_MEDIACHANGE:
        ret = MEDIANOCHANGE;
        break;
#if CONF_WITH_SCSI_DRIVER
    case CHECK_DEVICE:
        switch(ide_device_type(dev)) {
        case DEVTYPE_ATA:
            ret = ata_identify(dev);
            break;
        case DEVTYPE_ATAPI:
            ret = atapi_identify(dev);
            break;
        default:
            ret = EUNDEV;
            break;
        }
        break;
#endif
    }

    return ret;
}

#if CONF_WITH_SCSI_DRIVER
/*
 * check for the presence of an ATAPI device on the IDE bus
 */
static LONG atapi_identify(WORD dev)
{
    LONG ret;
    UWORD ifnum, ifdev;

    ifnum = dev / 2;    /* i.e. primary IDE, secondary IDE, ... */
    ifdev = dev & 1;    /* 0 or 1 */

    KDEBUG(("atapi_identify(%d [ifnum=%d ifdev=%d])\n", dev, ifnum, ifdev));

    /* with twisted cable the response of IDENTIFY_DEVICE will be byte-swapped */
    if (ide_device_type(dev) == DEVTYPE_ATAPI)
    {
        ret = ide_read(IDE_CMD_ATAPI_IDENTIFY,ifnum,ifdev,0L,1,(UBYTE *)&identify,
                       ifinfo[ifnum].twisted_cable != IDE_DATA_REGISTER_IS_BYTESWAPPED);
    } else ret = EUNDEV;

    if (ret < 0)
    {
        KDEBUG(("atapi_identify(%d) ret=%ld\n", dev, ret));
    }
    else if ((identify.general_config & 0xc000) != 0x8000)
    {
        KDEBUG(("atapi_identify(%d) conflicting protocol(0x%02x)\n", dev, identify.general_config&0xc0));
        ret = EUNDEV;
    }

    return ret;
}

static void set_packet_size(WORD dev, UWORD config)
{
    UWORD ifnum;
    UBYTE size;

    ifnum = dev / 2;    /* i.e. primary IDE, secondary IDE, ... */
    dev &= 1;           /* 0 or 1 */

    size = (config & 0x03) ? 16 : 12;

    KDEBUG(("Setting packet size %d for ifnum %d dev %d\n",size,ifnum,dev));

    ifinfo[ifnum].dev[dev].packet_size = size;
}

/*
 * in the cdb, check the LUN and the bytes that should be zeroes
 *
 * the LUN should always be zero; the bytes that should be zero are
 * indicated by 1 bits in the 'zerobits' bitmask.  for example, if
 * zerobits contains 0x0872, bytes 1, 4-6, and 11 should be zero.
 *
 * note that INVALID_CDB takes precedence over INVALID_LUN, since an
 * invalid CDB should always cause a check condition, whereas an invalid
 * LUN may not.
 */
static LONG check_lun_and_zeroes(IDECmd *cmd, UWORD zerobits)
{
    UBYTE *p;
    UWORD mask;
    LONG ret = E_OK;
    int i;

    for (i = 0, mask = 0x01, p = cmd->cdbptr; i < cmd->cdblen; i++, mask <<= 1, p++)
    {
        if ((zerobits & mask) && *p)    /* should be zero but isn't */
        {
            if ((i == 1)                /* handle byte containing LUN: */
             && ((*p & 0x1f) == 0))     /*  if rest of byte is zero,   */
            {                           /*  LUN must be non-zero       */
                ret = INVALID_LUN;      /* remember! */
                continue;
            }
            return INVALID_CDB;
        }
    }

    /*
     * in the case where the non-LUN bits of CDB byte 1 are not expected
     * to be zero (e,g, the READ6 command), byte 1 won't have been checked
     * above, so check it now
     */
    if (cmd->cdbptr[1] & 0xe0)
        ret = INVALID_LUN;

    return ret;
}

static ULONG get_disk_size(WORD dev)
{
    UNIT *unit = units + GET_UNITNUM(IDE_BUS, dev);

    return unit->valid ? unit->size : 0UL;
}

/*
 * emulate a basic set of SCSI commands on an ATA device
 */
static LONG handle_ata_device(WORD dev, IDECmd *cmd)
{
    UWORD ifnum, ifdev, unitnum, count;
    ULONG start;
    LONG info[2];
    LONG ret, oldret;

    ifnum = dev / 2;
    ifdev = dev & 1;

    switch(cmd->cdbptr[0]) {
    case TEST_UNIT_READY:
        ret = check_lun_and_zeroes(cmd, 0x003e);
        /* at this time we do nothing except validate the CDB */
        break;
    case REQUEST_SENSE:
        ret = check_lun_and_zeroes(cmd, 0x002e);
        if (ret == INVALID_CDB)
            break;
        /*
         * for invalid LUN, update stored sense, so that ata_request_sense()
         * will fill in ASC & ASCQ (this is apparently standard SCSI)
         */
        if (ret == INVALID_LUN)
            ifinfo[ifnum].dev[ifdev].sense = ret;
        oldret = ret;
        ret = ata_request_sense(dev, cmd->buflen, cmd->bufptr);
        if (oldret == INVALID_LUN)  /* for invalid LUN, we clear the error code, */
            cmd->bufptr[0] = 0x00;  /* which is also apparently standard SCSI    */
        break;
    case INQUIRY:
        ret = check_lun_and_zeroes(cmd, 0x002e);
        if (ret == INVALID_CDB)
            break;
        /* direct access device unless LUN is invalid ... */
        inquiry.devtype = (ret==INVALID_LUN) ? 0x7f : 0x00;
        ret = ata_identify(dev);    /* reads into identify structure */
        if (ret)
            break;
        /*
         * initialise Inquiry data, then copy to user's buffer
         */
        inquiry.devtype_mod = 0;
        inquiry.versions = 0x02;        /* iso=0, ecma=0, ansi=2 */
        inquiry.response_fmt = 0x02;    /* scsi 2 */
        inquiry.addl_length = INQUIRY_LENGTH - 1;
        inquiry.resvd = 0;
        inquiry.flags1 = 0;
        inquiry.flags2 = 0;
        memcpy(inquiry.vendor_id, identify.model_number, 24);
        memcpy(inquiry.product_rev, identify.firmware_revision, 4);
        memcpy(cmd->bufptr, &inquiry, min(cmd->buflen, INQUIRY_LENGTH));
        break;
    case READ_CAPACITY:
        ret = check_lun_and_zeroes(cmd, 0x07fe);
        if (ret)
            break;
        ret = ata_identify(dev);
        if (ret)
            break;
        if (identify.cmds_supported[1] & IDE_CAPABILITY_LBA48) {
            if (identify.maxsec_lba48[2] ||
                identify.maxsec_lba48[3]) {
                /*
                 * Should we return an error here ?
                 */
                KDEBUG(("Disk size >= 2^32 sectors, unsupported\n"));
            }

            info[0] = MAKE_ULONG(identify.maxsec_lba48[1],
                                 identify.maxsec_lba48[0]) - 1;
        } else {
            info[0] = MAKE_ULONG(identify.numsecs_lba28[1],
                                 identify.numsecs_lba28[0]) - 1;
        }
        info[1] = SECTOR_SIZE;
        memcpy(cmd->bufptr, (void *)info, 2*sizeof(LONG));
        break;
    case READ6:
        ret = check_lun_and_zeroes(cmd, 0x0020);
        if (ret)
            break;
        start = ((ULONG)cmd->cdbptr[1] << 16) + MAKE_UWORD(cmd->cdbptr[2], cmd->cdbptr[3]);
        count = cmd->cdbptr[4];
        if (!count)
            count = 256;
        if (start + count > get_disk_size(dev))
        {
            ret = INVALID_SECTOR;
            break;
        }
        unitnum = GET_UNITNUM(IDE_BUS, dev);
        ret = ide_rw(RW_READ, start, count, cmd->bufptr, dev, units[unitnum].byteswap);
        break;
    case READ10:
        ret = check_lun_and_zeroes(cmd, 0x0242);
        if (ret)
            break;
        start = MAKE_UWORD(cmd->cdbptr[2], cmd->cdbptr[3]);
        start = (start << 16) + MAKE_UWORD(cmd->cdbptr[4], cmd->cdbptr[5]);
        count = MAKE_UWORD(cmd->cdbptr[7], cmd->cdbptr[8]);
        if (start + count > get_disk_size(dev))
        {
            ret = INVALID_SECTOR;
            break;
        }
        if (count)
        {
            unitnum = GET_UNITNUM(IDE_BUS, dev);
            ret = ide_rw(RW_READ, start, count, cmd->bufptr, dev, units[unitnum].byteswap);
        }
        break;
    case WRITE6:
        ret = check_lun_and_zeroes(cmd, 0x0020);
        if (ret)
            break;
        start = ((ULONG)cmd->cdbptr[1] << 16) + MAKE_UWORD(cmd->cdbptr[2], cmd->cdbptr[3]);
        count = cmd->cdbptr[4];
        if (!count)
            count = 256;
        if (start + count > get_disk_size(dev))
        {
            ret = INVALID_SECTOR;
            break;
        }
        unitnum = GET_UNITNUM(IDE_BUS, dev);
        ret = ide_rw(RW_WRITE, start, count, cmd->bufptr, dev, units[unitnum].byteswap);
        break;
    case WRITE10:
        ret = check_lun_and_zeroes(cmd, 0x0242);
        if (ret)
            break;
        start = MAKE_UWORD(cmd->cdbptr[2], cmd->cdbptr[3]);
        start = (start << 16) + MAKE_UWORD(cmd->cdbptr[4], cmd->cdbptr[5]);
        count = MAKE_UWORD(cmd->cdbptr[7], cmd->cdbptr[8]);
        if (start + count > get_disk_size(dev))
        {
            ret = INVALID_SECTOR;
            break;
        }
        if (count)
        {
            unitnum = GET_UNITNUM(IDE_BUS, dev);
            ret = ide_rw(RW_WRITE, start, count, cmd->bufptr, dev, units[unitnum].byteswap);
        }
        break;
    default:
        /* check condition: invalid opcode */
        ret = INVALID_OPCODE;
        break;
    }

    if (ret <= 0)
        return ret;

    /*
     * a check condition was detected, save the details for later
     */
    ifinfo[ifnum].dev[ifdev].sense = ret;

    return 0x02;    /* check condition */
}

/*
 * set device / command / bytecount for ATAPI device in IDE registers
 */
static void atapi_rw_start(volatile struct IDE *interface,UWORD ifdev,UWORD bytecount,UBYTE cmd)
{
    KDEBUG(("atapi_rw_start(%p, %u, %u, 0x%02x)\n", interface, ifdev, bytecount, cmd));

    set_cylinder(interface,bytecount);
    set_command_head(interface,cmd,0xa0|IDE_DEVICE(ifdev));
}

/*
 * send CDB to ATAPI device
 *
 * note the following (currently true) assumptions about packet size:
 *  . it is an exact multiple of XFERWIDTH
 *  . it is less than or equal to MAX_SCSI_CDBLEN
 */
static void atapi_send_cdb(WORD dev,UBYTE *cdbptr,WORD cdblen)
{
    struct IFINFO *info;
    volatile struct IDE *interface;
    UWORD cdb[MAX_SCSI_CDBLEN/sizeof(UWORD)];
    WORD ifnum, ifdev, packetlen;

    ifnum = dev / 2;
    ifdev = dev & 1;

    info = &ifinfo[ifnum];
    interface = info->base_address;
    packetlen = info->dev[ifdev].packet_size;

    /* copy cdb to word-aligned area with zero fill */
    memset(cdb,0,MAX_SCSI_CDBLEN);
    memcpy(cdb,(void *)cdbptr,min(cdblen,packetlen));

    KDEBUG(("atapi_send_cdb(): dev=%d, cdblen=%d, cdb=%02x...\n",dev, cdblen,cdb[0]));

    /*
     * we assume that data on ATAPI devices is always byte-swapped,
     * so iff we have a twisted cabel, we don't byte-swap
     */
    if (info->twisted_cable) {
        ide_put_data((volatile struct IDE *)(((ULONG)interface)+1),(UBYTE *)cdb,packetlen,0);
    } else {
        ide_put_data(interface,(UBYTE *)cdb,packetlen,1);
    }
}

/*
 * send SCSI commands to an ATAPI device
 */
static LONG handle_atapi_device(WORD dev, IDECmd *cmd)
{
    struct IFINFO *info;
    volatile struct IDE *interface;
    UBYTE *buf;
    UWORD bytecount, ifdev, ifnum;
    UBYTE status;

    KDEBUG(("handle_atapi_device(%d): cmd=0x%02x (%d,%p,%ld,%ld,%u)\n",
            dev,cmd->cdbptr[0],cmd->cdblen,cmd->bufptr,cmd->buflen,cmd->timeout,cmd->flags));

    ifnum = dev / 2;
    ifdev = dev & 1;
    info = ifinfo + ifnum;
    interface = info->base_address;

    if (ide_select_device(interface,ifdev) < 0)
        return SELECT_ERROR;

    /* determine byte count for cdb packet */
    bytecount = min(cmd->buflen,ATAPI_MAX_BYTECOUNT);

    atapi_rw_start(interface,ifdev,bytecount,IDE_CMD_ATAPI_PACKET);
    if (wait_for_not_BSY(interface,SHORT_TIMEOUT))
        return TIMEOUT_ERROR;

    /* send the packet containing the cdb */
    atapi_send_cdb(dev,cmd->cdbptr,cmd->cdblen);

    buf = cmd->bufptr;
    while(1) {
        if (wait_for_not_BSY(interface,cmd->timeout))
            return TIMEOUT_ERROR;

        status = IDE_READ_STATUS(interface);    /* status, clear pending interrupt */
        if (!(status & IDE_STATUS_DRQ))
            break;

        /*
         * find out how many bytes the device wants to send (or receive),
         * and get (or put) them.
         */
        bytecount = IDE_READ_CYLINDER_HIGH_CYLINDER_LOW(interface);
        if (cmd->flags == RW_WRITE) {
            if (info->twisted_cable) {
                ide_put_data((volatile struct IDE *)(((ULONG)interface)+1),buf,bytecount,0);
            } else {
                ide_put_data(interface,buf,bytecount,1);
            }
        } else {
            if (info->twisted_cable) {
                ide_get_data((volatile struct IDE *)(((ULONG)interface)+1),buf,bytecount,0);
            } else {
                ide_get_data(interface,buf,bytecount,1);
            }
        }
        buf += bytecount;
    }

    if (status & IDE_STATUS_DF)
        return STATUS_ERROR;

    if (status & IDE_STATUS_ERR) {
        status = IDE_READ_ERROR(interface);
        if (status&0xf0) {      /* non-zero sense key */
            return 0x02;        /* check condition */
        }
    }

    return E_OK;
}

LONG send_ide_command(WORD dev, IDECmd *cmd)
{
    LONG ret;

    switch(ide_device_type(dev)) {
    case DEVTYPE_ATA:
        ret = handle_ata_device(dev, cmd);
        break;
    case DEVTYPE_ATAPI:
        ret = handle_atapi_device(dev, cmd);
        break;
    default:
        ret = STATUS_ERROR;     /* "can't happen" */
    }

    return ret;
}

/*
 * creates a request sense response based on the last 'check condition'
 */
static LONG ata_request_sense(WORD dev, WORD buflen, UBYTE *buffer)
{
    UWORD ifnum, ifdev;
    UBYTE sense, *senseptr;

    bzero(&reqsense, REQSENSE_LENGTH);
    reqsense.addl_length = (REQSENSE_LENGTH-1) - 7;

    ifnum = dev / 2;
    ifdev = dev & 1;

    senseptr = &ifinfo[ifnum].dev[ifdev].sense;
    sense = *senseptr;      /* get last saved check condition */
    *senseptr = 0;          /*  & reset it                    */

    if (sense)
    {
        reqsense.error_code = 0xf0; /* valid / error code 0x70 */
        reqsense.sense_key = 0x05;  /* illegal request */
    }

    switch(sense) {
    case INVALID_OPCODE:
        reqsense.asc = 0x20;        /* invalid command operation code */
        break;
    case INVALID_SECTOR:
        reqsense.asc = 0x21;        /* logical block address out of range */
        break;
    case INVALID_CDB:
        reqsense.asc = 0x24;        /* invalid field in cdb */
        break;
    case INVALID_LUN:
        reqsense.asc = 0x25;        /* logical unit not supported */
        break;
    }

    memcpy(buffer, &reqsense, min(buflen, REQSENSE_LENGTH));

    return E_OK;
}

/*
 * issue REQUEST SENSE on ATAPI
 */
static LONG atapi_request_sense(WORD dev, WORD buflen, UBYTE *buffer)
{
    UBYTE cdb[6];
    IDECmd cmd;

    cdb[0] = REQUEST_SENSE;
    cdb[1] = 0;
    cdb[2] = 0;
    cdb[3] = 0;
    cdb[4] = REQSENSE_LENGTH;
    cdb[5] = 0;

    cmd.cdbptr = cdb;
    cmd.cdblen = 6;
    cmd.bufptr = buffer;
    cmd.buflen = buflen;
    cmd.timeout = XFER_TIMEOUT;
    cmd.flags = RW_READ;

    return handle_atapi_device(dev, &cmd);
}

LONG ide_request_sense(WORD dev, WORD buflen, UBYTE *buffer)
{
    LONG ret;

    switch(ide_device_type(dev)) {
    case DEVTYPE_ATA:
        ret = ata_request_sense(dev, buflen, buffer);
        break;
    case DEVTYPE_ATAPI:
        ret = atapi_request_sense(dev, buflen, buffer);
        break;
    default:
        ret = STATUS_ERROR;     /* "can't happen" */
    }

    return ret;
}

#endif

#endif /* CONF_WITH_IDE */
