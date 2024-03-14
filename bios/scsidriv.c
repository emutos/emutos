/*
 * scsidriv.c - SCSI driver routines
 *
 * Copyright (C) 2023-2024 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Apart from unknown bugs, the following shortcomings are currently
 * present:
 *  1. we mark all ACSI devices as ICD-compatible. there are likely very
 *     few real ACSI devices left, so this should not be a real-life problem.
 *     fixing this would probably require issuing (e.g.) an IDENTIFY during
 *     ACSI initialisation.
 *  2. we do not detect the host device (ourselves) on an arbitrating bus.
 *     thus, unlike HDdriver, the host does not appear in the response to
 *     InquireBus().  this is unlikely to cause any problems.
 */

/*
 * Note: at this time, EmuTOS does not use this SCSI driver internally;
 * therefore it will not be made aware of I/O conditions such as media
 * change that have been detected by a user program using this driver.
 * Similarly, user programs using this SCSI driver will not be made aware
 * of those conditions that are detected by EmuTOS.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "scsidriv.h"
#include "biosdefs.h"
#include "biosext.h"
#include "has.h"
#include "machine.h"
#include "tosvars.h"
#include "disk.h"
#include "acsi.h"
#include "scsi.h"
#include "ide.h"
#include "scsicmds.h"
#include "gemerror.h"
#include "cookie.h"
#include "string.h"

#if CONF_WITH_SCSI_DRIVER

#define MAXLEN_ACSI     (128 * SECTOR_SIZE)     /* like HD Driver says */
#define MAXLEN_SCSI     (256 * 1024L * 1024L)
#define MAXLEN_IDE      (255 * SECTOR_SIZE)

typedef struct {
    UWORD features;     /* typically the same as the bus features */
    UBYTE status;       /* handle status */
#define IN_USE      0x01
    UBYTE busdev;       /* bits 7-4: bus#, bits 3-0: dev# */
    UWORD errors;       /* only bits 0 & 1 are defined (see scsidriv_Error()) */
} HandleEntry;

static LONG scsidriv_In(SCSICmd *cmd);
static LONG scsidriv_Out(SCSICmd *cmd);
static LONG scsidriv_InquireSCSI(WORD what, BusInfo *info);
static LONG scsidriv_InquireBus(WORD what, WORD busnum, DevInfo *info);
static LONG scsidriv_CheckDev(WORD busnum, const DLONG *SCSIId, char *name, UWORD *features);
static LONG scsidriv_RescanBus(WORD busnum);
static LONG scsidriv_Open(WORD busnum, const DLONG *SCSIId, ULONG *MaxLen);
static LONG scsidriv_Close(SCSIHandle handle);
static LONG scsidriv_Error(SCSIHandle handle, WORD rwflag, WORD errnum);

/*
 * the structure pointed to by the 'SCSI' cookie must be in RAM,
 * since adding subsequent SCSI driver handlers is done by overwriting
 * the existing data, not reallocating the cookie.
 */
SCSIRoot scsidriv_root;         /* in RAM so it can be modified */

const SCSIRoot scsidriv_root_init =
{
    SCSIDRIV_VERSION,
    scsidriv_In,
    scsidriv_Out,
    scsidriv_InquireSCSI,
    scsidriv_InquireBus,
    scsidriv_CheckDev,
    scsidriv_RescanBus,
    scsidriv_Open,
    scsidriv_Close,
    scsidriv_Error
};

static UBYTE *frbptr;           /* set by scsidriv_init() */

/*
 * bitmap of detected devices on each bus
 */
static UWORD detected_devices[MAX_BUS+1];

/*
 * handle area
 */
static HandleEntry handle_array[NUM_SCSIDRIV_HANDLES];

/*
 * scsidriv_init(): initialise SCSI driver processing
 *
 * . initialise the data area pointed to by the 'SCSI' cookie
 * - scan all detected busses for all devices & update detected_devices[] array
 * - initialise handles
 * . set 'frb present' indicator
 */
void scsidriv_init(void)
{
    int bus, i;
    HandleEntry *h;
    ULONG bit;

    scsidriv_root = scsidriv_root_init;

    for (bus = 0, bit = 1; bus <= MAX_BUS; bus++, bit <<= 1)
        if (detected_busses & bit)
            scsidriv_RescanBus(bus);

    for (i = 0, h = handle_array; i < NUM_SCSIDRIV_HANDLES; i++, h++)
        h->status = 0;

#if CONF_WITH_FRB
    frbptr = get_frb_cookie();      /* non-NULL iff FRB present */
#else
    frbptr = NULL;
#endif
}

/*
 * check for valid bus
 */
static BOOL valid_bus(WORD busnum)
{
    if (busnum > MAX_BUS)
        return FALSE;

    if (!(detected_busses & (1<<busnum)))
        return FALSE;

    return TRUE;
}

/*
 * check for valid bus & valid device
 */
static BOOL valid_bus_and_device(WORD busnum, WORD devnum)
{
    if (!valid_bus(busnum))
        return FALSE;

    if (!(detected_devices[busnum] & (1<<devnum)))
        return FALSE;

    return TRUE;
}

/*
 * return maxlen for specified bus
 *
 * returns 0 if unknown bus
 */
static ULONG get_bus_maxlen(WORD busnum)
{
    ULONG maxlen = 0UL;

    switch(busnum)
    {
#if CONF_WITH_ACSI
    case ACSI_BUS:
        maxlen = MAXLEN_ACSI;
        break;
#endif
#if CONF_WITH_SCSI
    case SCSI_BUS:
        if (HAS_VIDEL && frbptr)        /* Falcon with TT-RAM */
            maxlen = FRB_SIZE;
        else maxlen = MAXLEN_SCSI;
        break;
#endif
#if CONF_WITH_IDE
    case IDE_BUS:
        maxlen = MAXLEN_IDE;
        break;
#endif
    }

    return maxlen;
}

/*
 * get name, features, and maxlen for specified bus
 *
 * returns EUNDEV for an invalid bus number
 */
static int get_bus_info(WORD busnum, char *name, UWORD *features, ULONG *maxlen)
{
    if (!valid_bus(busnum))
        return EUNDEV;

    switch(busnum)
    {
#if CONF_WITH_ACSI
    case ACSI_BUS:
        strcpy(name, "Atari ACSI");
        *features = cAllCmds;
        break;
#endif
#if CONF_WITH_SCSI
    case SCSI_BUS:
        strcpy(name, "Atari SCSI");
        *features = cArbit | cAllCmds | cTargCtrl;
        break;
#endif
#if CONF_WITH_IDE
    case IDE_BUS:
        strcpy(name, "Atari IDE");
        *features = cAllCmds;
        break;
#endif
    default:        /* shouldn't happen */
        return EUNDEV;
    }

    *maxlen = get_bus_maxlen(busnum);

    return 0;
}

/*
 * get ptr to handle entry for specified handle
 *
 * returns NULL if handle is invalid
 */
static HandleEntry *get_handle_entry(SCSIHandle handle)
{
    int i;
    HandleEntry *h;
    
    for (i = 0, h = handle_array; i < NUM_SCSIDRIV_HANDLES; i++, h++)
        if (handle == &h->features)
            return h;

    return NULL;
}

/*
 * common routine used by scsidriv_In() & scsidriv_Out()
 *
 * 'write' parameter: 0 => read, else write
 */
static LONG scsidriv_InOut(WORD write, SCSICmd *cmd)
{
    HandleEntry *h;
    LONG rc, rc2;
    int bus, dev;

    /* check for valid handle */
    h = get_handle_entry(cmd->handle);
    if (!h)
        return EIHNDL;

    /* check for pending error */
    if (h->errors)
        return PENDING_ERROR;

    bus = h->busdev >> 4;
    dev = h->busdev & 0x0f;

    if (cmd->xferlen > get_bus_maxlen(bus))
        return DATATOOLONG_ERROR;

    rc = EIHNDL;

    switch(bus)
    {
#if CONF_WITH_ACSI
    case ACSI_BUS:
        {
            ACSICMD acsi;
            UBYTE *buf;

            buf = cmd->buffer;
            if (IS_ODD_POINTER(buf) || !IS_STRAM_POINTER(buf))
            {
                if (frbptr)
                {
                    if (cmd->xferlen > FRB_SIZE)
                        return STATUS_ERROR;
                    buf = frbptr;
                }
                else
                {
                    if (cmd->xferlen > DSKBUF_SIZE)
                        return STATUS_ERROR;
                    buf = dskbufp;
                }
                /* if writing, copy user area to temp buffer */
                if (write)
                    memcpy(buf, cmd->buffer, cmd->xferlen);
            }

            acsi.cdbptr = cmd->cdb;
            acsi.cdblen = cmd->cdblen;
            acsi.bufptr = buf;
            acsi.buflen = cmd->xferlen;
            acsi.timeout = cmd->timeout;
            acsi.rw = write ? RW_WRITE : RW_READ;
            rc = send_command(dev, &acsi);
            if (rc < 0)
                return TIMEOUT_ERROR;

            rc &= 0x00ff;       /* isolate status byte */

            /* if reading, copy temp buffer to user area if necessary */
            if (!write && (rc == 0) && (cmd->buffer != buf))
                memcpy(cmd->buffer, buf, cmd->xferlen);

            /*
             * if check condition AND request sense buffer is supplied, fill it in
             */
            if ((rc & 0x02) && cmd->sensebuf)
            {
                rc2 = acsi_request_sense(dev, cmd->sensebuf);
                if (rc2 < 0)    /* an error doing request sense is bad ... */
                    rc = rc2;
            }
        }
        break;
#endif
#if CONF_WITH_SCSI
    case SCSI_BUS:
        {
            CMDINFO scsi;

            scsi.cdbptr = cmd->cdb;
            scsi.cdblen = cmd->cdblen;
            scsi.bufptr = cmd->buffer;
            scsi.buflen = cmd->xferlen;
            scsi.xfer_time = cmd->timeout;
            scsi.mode = write ? WRITE_MODE : 0;
            rc = send_scsi_command(dev, &scsi);
            if (rc < 0L)        /* -ve: already have the correct return code */
                break;

            /*
             * if check condition AND request sense buffer is supplied, fill it in
             */
            rc &= 0x00ff;       /* isolate status byte */
            if ((rc & 0x02) && cmd->sensebuf)
            {
                rc2 = scsi_request_sense(dev, cmd->sensebuf);
                if (rc2 < 0)    /* an error doing request sense is bad ... */
                    rc = rc2;
            }
        }
        break;
#endif
#if CONF_WITH_IDE
    case IDE_BUS:
        {
            IDECmd ide;

            ide.cdbptr = cmd->cdb;
            ide.cdblen = cmd->cdblen;
            ide.bufptr = cmd->buffer;
            ide.buflen = cmd->xferlen;
            ide.timeout = cmd->timeout;
            ide.flags = write ? RW_WRITE : RW_READ;
            rc = send_ide_command(dev, &ide);
            if (rc < 0L)        /* -ve: already have the correct return code */
                break;

            /*
             * if check condition AND request sense buffer is supplied, fill it in
             */
            rc &= 0x00ff;       /* isolate status byte */
            if ((rc & 0x02) && cmd->sensebuf)
            {
                rc2 = ide_request_sense(dev, REQSENSE_LENGTH, cmd->sensebuf);
                if (rc2 < 0)    /* an error doing request sense is bad ... */
                    rc = rc2;
            }
        }
        break;
#endif
    default:
        break;
    }

    return rc;
}

/*
 * scsidriv_In(): issue input command
 */
static LONG scsidriv_In(SCSICmd *cmd)
{
    return scsidriv_InOut(0, cmd);
}

/*
 * scsidriv_Out(): issue output command
 */
static LONG scsidriv_Out(SCSICmd *cmd)
{
    return scsidriv_InOut(1, cmd);
}

/*
 * scsidriv_InquireSCSI(): determine available busses
 */
static LONG scsidriv_InquireSCSI(WORD what, BusInfo *info)
{
    int bus;
    ULONG bit;

    if (what == cInqFirst)
        info->private.busavail = detected_busses;

    if (info->private.busavail == 0)    /* no (more) busses */
        return EUNDEV;                  /* this is what HDDRIVER returns */

    for (bus = 0, bit = 1; bus <= MAX_BUS; bus++, bit <<= 1)
    {
        if (info->private.busavail & bit)
        {
            info->private.busavail &= ~bit;     /* don't report again */
            break;
        }
    }

    info->busnum = bus;
    if (get_bus_info(bus, info->busname, &info->features, &info->maxlen) < 0)
        return EUNDEV;

    KDEBUG(("InquireSCSI(bus %u): name=%s, features=0x%04x, maxlen=%lu\n",
            info->busnum, info->busname, info->features, info->maxlen));

    return 0L;
}

/*
 * scsidriv_InquireBus(): determine available devices on bus
 */
static LONG scsidriv_InquireBus(WORD what, WORD busnum, DevInfo *info)
{
    int dev;
    UWORD bit;

    if (!valid_bus(busnum))
        return -1L;

    if (what == cInqFirst)
        info->private[0] = 0;               /* next device to check */

    for (dev = info->private[0], bit = (1<<dev); dev < DEVICES_PER_BUS; dev++, bit <<= 1)
    {
        if (detected_devices[busnum] & bit)
        {
            info->SCSIId.hi = 0UL;
            info->SCSIId.lo = dev;
            info->private[0] = dev + 1;     /* for next time */
            KDEBUG(("InquireBus(bus %d): dev %d exists\n",busnum,dev));
            return 0L;
        }
    }

    info->private[0] = DEVICES_PER_BUS;     /* so next cInqNext will exit immediately */

    return -1L;     /* this is what HDDRIVER returns */
}

/*
 * scsidriv_CheckDev(): like scsidriv_InquireSCSI, except a specific bus/device
 *
 * note that the name returned is the name of the bus, not the device
 */
static LONG scsidriv_CheckDev(WORD busnum, const DLONG *SCSIId, char *name, UWORD *features)
{
    ULONG dummy;

    if (!valid_bus_and_device(busnum,SCSIId->lo))
        return EUNDEV;

    if (get_bus_info(busnum, name, features, &dummy) < 0)
        return EUNDEV;

    return 0L;
}

/*
 * scsidriv_RescanBus(): rescan specified bus for devices
 */
static LONG scsidriv_RescanBus(WORD busnum)
{
    int dev;
    UWORD bit;
    LONG ret;

    if (!valid_bus(busnum))
        return -1L;

    detected_devices[busnum] = 0;

    for (dev = 0, bit = 1; dev < DEVICES_PER_BUS; dev++, bit <<= 1)
    {
        switch(busnum)
        {
#if CONF_WITH_ACSI
        case ACSI_BUS:
            ret = acsi_ioctl(dev, CHECK_DEVICE, NULL);
            break;
#endif
#if CONF_WITH_SCSI
        case SCSI_BUS:
            ret = scsi_ioctl(dev, CHECK_DEVICE, NULL);
            break;
#endif
#if CONF_WITH_IDE
        case IDE_BUS:
            ret = ide_ioctl(dev, CHECK_DEVICE, NULL);
            break;
#endif
        default:
            ret = -1;
            break;
        }
        if (ret == 0)
            detected_devices[busnum] |= bit;
    }

    KDEBUG(("RescanBus(bus %d): detected devices = 0x%04x\n",busnum,detected_devices[busnum]));
    return 0L;
}

/*
 * scsidriv_Open(): get a handle for use with scsidriv_In()/scsidriv_Out()
 *
 * the handle returned is actually a pointer to a UWORD containing the features supported
 */
static LONG scsidriv_Open(WORD busnum, const DLONG *SCSIId, ULONG *MaxLen)
{
    int i;
    HandleEntry *h;
    char dummy[20];

    if (!valid_bus_and_device(busnum,SCSIId->lo))
        return EUNDEV;

    /*
     * look for unused handle
     */
    for (i = 0, h = handle_array; i < NUM_SCSIDRIV_HANDLES; i++, h++)
        if (!(h->status & IN_USE))
            break;

    if (i >= NUM_SCSIDRIV_HANDLES)
        return ENHNDL;

    if (get_bus_info(busnum, dummy, &h->features, MaxLen) < 0)
        return EUNDEV;

    h->status = IN_USE;
    h->busdev = ((busnum & 0x0f) << 4) | (SCSIId->lo & 0x0f);

    return (LONG)&h->features;
}

/*
 * scsidriv_Close(): free up a handle
 */
static LONG scsidriv_Close(SCSIHandle handle)
{
    HandleEntry *h;

    h = get_handle_entry(handle);
    if (!h)
        return EIHNDL;

    h->status &= ~IN_USE;

    return 0L;
}

/*
 * scsidriv_Error(): read/write error status
 *
 * . read: get the error bitmap for the specified handle, then clear it
 * . write: set the specified error bit in all *other* open handles for
 *          the device associated with the specified handle
 *
 * How this is used:
 * 1. If a mediachange or reset condition is detected on a device, the program
 *    detecting it must issue a scsidriv_Error(write) for the condition; this will
 *    set a bit in the 'errors' variable for each open handle for that device.
 * 2. When scsidriv_In()/scsidriv_Out() is called, if the handle in question has
 *    a nonzero 'errors' variable, EmuTOS immediately returns PENDING_ERROR.
 * 3. When a program sees PENDING_ERROR from scsidriv_In()/scsidriv_Out(), it must
 *    immediately call scsidriv_Error(read) to determine the condition that was
 *    detected.
 */
static LONG scsidriv_Error(SCSIHandle handle, WORD rwflag, WORD errnum)
{
    HandleEntry *h, *p;
    int i;
    LONG rc;

    h = get_handle_entry(handle);
    if (!h)
        return EIHNDL;
    if (!(h->status & IN_USE))
        return EIHNDL;

    switch(rwflag)
    {
    case cErrRead:      /* return error bitmap for this handle */
        rc = h->errors;     /* get bitmap */
        h->errors = 0;      /* & reset it */
        break;
    case cErrWrite:     /* set error bit in all other handles for this device */
        if ((errnum < cErrMediach) || (errnum > cErrReset))
            return -1L;
        for (i = 0, p = handle_array; i < NUM_SCSIDRIV_HANDLES; i++, p++)
            if ((p != h) && (p->status & IN_USE) && (p->busdev == h->busdev))
                p->errors |= (1 << errnum);
        rc = 0L;
        break;
    default:
        return -1L;
    }

    return rc;
}
#endif
