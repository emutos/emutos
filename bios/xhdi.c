/*
 * xhdi.c - XHDI handler
 *
 * Copyright (C) 2001-2017 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *
 *  xhdi_handler() inspired by ppzip driver (by Frank Naumann)
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*#define ENABLE_KDEBUG*/

#include "config.h"
#include "xhdi.h"
#include "kprint.h"
#include "blkdev.h"
#include "gemerror.h"
#include "string.h"
#include "cookie.h"
#include "disk.h"
#include "ahdi.h"


#if CONF_WITH_XHDI

#define XHDI_MAXSECS    0x7fffffffL     /* returned by XHDOSLimits() */

/*--- Global variables ---*/

/* XHDI_HANDLER is the type where an XHDI cookie points to */
typedef long (*XHDI_HANDLER)(UWORD opcode, ...);
static XHDI_HANDLER next_handler; /* Next handler installed by XHNewCookie() */

static ULONG XHDI_drvmap;

/*---Functions ---*/

void create_XHDI_cookie(void)
{
    cookie_add(COOKIE_XHDI, (long)xhdi_vec);
}

void init_XHDI_drvmap(void)
{
    /* Currently, floppy drives can't be accessed through XHDI. */
    XHDI_drvmap = blkdev_drvmap() & ~0x03UL;
}

static UWORD XHGetVersion(void)
{
    UWORD version = 0x130;

    if (next_handler) {
        UWORD next_version = (UWORD)next_handler(XHGETVERSION);
        if (next_version < version)
            version = next_version;
    }

    return version;
}

static ULONG XHDrvMap(void)
{
    ULONG drvmap = XHDI_drvmap;

    if (next_handler)
        drvmap |= next_handler(XHDRVMAP);

    return drvmap;
}

static long XHNewCookie(ULONG newcookie)
{
    if (next_handler)
        return next_handler(XHNEWCOOKIE, newcookie);

    next_handler = (XHDI_HANDLER)newcookie;

    return E_OK;
}

static long XHInqDev2(UWORD drv, UWORD *major, UWORD *minor, ULONG *start,
                      BPB *bpb, ULONG *blocks, char *partid)
{
    long pstart = blkdev[drv].start;
    BPB *myBPB;

    KDEBUG(("XHInqDev2(%c:) start=%ld, size=%ld, ID=%c%c%c\n",
            'A' + drv, pstart, blkdev[drv].size,
            blkdev[drv].id[0], blkdev[drv].id[1], blkdev[drv].id[2]));

    if (next_handler) {
        long ret = next_handler(XHINQDEV2, drv, major, minor, start, bpb, blocks, partid);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    if (drv >= BLKDEVNUM || !(blkdev[drv].flags&DEVICE_VALID))
        return EDRIVE;

    if (major)
        *major = blkdev[drv].unit - NUMFLOPPIES;
    if (minor)
        *minor = 0;
    if (bpb)
        bpb->recsiz = 0;

    if (!pstart)
        return EDRVNR;

    if (start)
        *start = pstart;

    myBPB = (BPB *)blkdev_getbpb(drv);
    if (bpb && myBPB)
        memcpy(bpb, myBPB, sizeof(BPB));

    if (blocks)
        *blocks = blkdev[drv].size;

    if (partid) {
        partid[0] = blkdev[drv].id[0];
        partid[1] = blkdev[drv].id[1];
        partid[2] = blkdev[drv].id[2];
    }

   return E_OK;
}

static long XHInqDev(UWORD drv, UWORD *major, UWORD *minor, ULONG *start,
                     BPB *bpb)
{
    if (next_handler) {
        long ret = next_handler(XHINQDEV, drv, major, minor, start, bpb);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return XHInqDev2(drv, major, minor, start, bpb, NULL, NULL);
}

static long XHReserve(UWORD major, UWORD minor, UWORD do_reserve, UWORD key)
{
    if (next_handler) {
        long ret = next_handler(XHRESERVE, major, minor, do_reserve, key);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

static long XHLock(UWORD major, UWORD minor, UWORD do_lock, UWORD key)
{
    if (next_handler) {
        long ret = next_handler(XHLOCK, major, minor, do_lock, key);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

static long XHStop(UWORD major, UWORD minor, UWORD do_stop, UWORD key)
{
    if (next_handler) {
        long ret = next_handler(XHSTOP, major, minor, do_stop, key);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

static long XHEject(UWORD major, UWORD minor, UWORD do_eject, UWORD key)
{
    if (next_handler) {
        long ret = next_handler(XHEJECT, major, minor, do_eject, key);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

static long XHInqDriver(UWORD bios_device, char *name, char *version, char *company, UWORD *ahdi_version, UWORD *max_IPL)
{
    if (next_handler) {
        long ret = next_handler(XHINQDRIVER, bios_device, name, version, company, ahdi_version, max_IPL);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    if (bios_device >= BLKDEVNUM || !(blkdev[bios_device].flags&DEVICE_VALID))
        return EDRIVE;

    if (disk_inquire(blkdev[bios_device].unit, NULL, NULL, NULL, 0) == EUNDEV)
        return EDRIVE;

    if(name)
        strlcpy(name, DRIVER_NAME, DRIVER_NAME_MAXLENGTH);
    if(version)
        strlcpy(version, DRIVER_VERSION, DRIVER_VERSION_MAXLENGTH);
    if(company)
        strlcpy(company, DRIVER_COMPANY, DRIVER_COMPANY_MAXLENGTH);
    if(ahdi_version)
        *ahdi_version = pun_ptr->version_num;
    if(max_IPL)
        *max_IPL = MAX_IPL;

    return E_OK;
}

static long XHDriverSpecial(ULONG key1, ULONG key2, UWORD subopcode, void *data)
{
    if (next_handler) {
        long ret = next_handler(XHDRIVERSPECIAL, key1, key2, subopcode, data);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

static long XHMediumChanged(UWORD major, UWORD minor)
{
    if (next_handler) {
        long ret = next_handler(XHMEDIUMCHANGED, major, minor);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

static long XHMiNTInfo(UWORD opcode, void *data)
{
    if (next_handler) {
        long ret = next_handler(XHMINTINFO, opcode, data);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

static long XHDOSLimits(UWORD which, ULONG limit)
{
    long ret;

    if (next_handler) {
        ret = next_handler(XHDOSLIMITS, which, limit);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    /* Currently setting new limits is not supported */
    if (limit == 0) {

        switch (which) {
            case XH_DL_SECSIZ:
                /* Maximum sector size (BIOS level) */
                ret = MAX_LOGSEC_SIZE;
                break;

            case XH_DL_MINFAT:
                /* Minimum number of FATs */
                ret = MIN_FATS;
                break;

            case XH_DL_MAXFAT:
                /* Maximal number of FATs */
                ret = MAX_FATS;
                break;

            case XH_DL_MINSPC:
                /* Minimum sectors per cluster */
                ret = MIN_SECS_PER_CLUS;
                break;

            case XH_DL_MAXSPC:
                /* Maximum sectors per cluster */
                ret = MAX_SECS_PER_CLUS;
                break;

            case XH_DL_CLUSTS:
                /* Maximum number of clusters of a 16-bit FAT */
                ret = MAX_FAT16_CLUSTERS;
                break;

            case XH_DL_MAXSEC:
                /* Maximum number of sectors */
                ret = XHDI_MAXSECS;
                break;

            case XH_DL_DRIVES:
                /* Maximum number of BIOS drives supported by the DOS */
                ret = BLKDEVNUM;
                break;

            case XH_DL_CLSIZB:
                /* Maximum cluster size */
                ret = MAX_CLUSTER_SIZE;
                break;

            case XH_DL_RDLEN:
                /* Max. (bpb->rdlen * bpb->recsiz/32) */
                ret = EINVFN; /* meaning of XH_DL_RDLEN is unclear */
                break;

            case XH_DL_CLUSTS12:
                /* Max. number of clusters of a 12-bit FAT */
                ret = MAX_FAT12_CLUSTERS;
                break;

            case XH_DL_CLUSTS32:
                /* Max. number of clusters of a 32 bit FAT */
                ret = EINVFN; /* No FAT32 support. */
                break;

            case XH_DL_BFLAGS:
                /* Supported bits in bpb->bflags */
                ret = 1; /* Bit 0 (16 bit fat) */
                break;

            default:
                ret = EINVFN;
                break;
        }

    } else { /* limit != 0 */
        ret = EINVFN;
    }

    return ret;
}

static long XHLastAccess(UWORD major, UWORD minor, ULONG *ms)
{
    if (next_handler) {
        long ret = next_handler(XHLASTACCESS, major, minor, ms);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

static long XHReaccess(UWORD major, UWORD minor)
{
    if (next_handler) {
        long ret = next_handler(XHREACCESS, major, minor);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    return EINVFN;
}

/*=========================================================================*/

static long XHInqTarget2(UWORD major, UWORD minor, ULONG *blocksize,
                         ULONG *deviceflags, char *productname, UWORD stringlen)
{
    UWORD unit;

    KDEBUG(("XHInqTarget2(%d.%d)\n", major, minor));

    if (next_handler) {
        long ret = next_handler(XHINQTARGET2, major, minor, blocksize, deviceflags, productname, stringlen);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    if (minor != 0)
        return EUNDEV;

    unit = NUMFLOPPIES + major;

    return disk_inquire(unit, blocksize, deviceflags, productname, stringlen);
}

static long XHInqTarget(UWORD major, UWORD minor, ULONG *blocksize,
                 ULONG *deviceflags, char *productname)
{
    UWORD unit;

    KDEBUG(("XHInqTarget(%d.%d)\n", major, minor));

    if (next_handler) {
        long ret = next_handler(XHINQTARGET, major, minor, blocksize, deviceflags, productname);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    if (minor != 0)
        return EUNDEV;

    unit = NUMFLOPPIES + major;

    return disk_inquire(unit, blocksize, deviceflags, productname, 33);
}

static long XHGetCapacity(UWORD major, UWORD minor, ULONG *blocks, ULONG *blocksize)
{
    UWORD unit;

    KDEBUG(("XHGetCapacity(%d.%d)\n", major, minor));

    if (next_handler) {
        long ret = next_handler(XHGETCAPACITY, major, minor, blocks, blocksize);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    if (minor != 0)
        return EUNDEV;

    unit = NUMFLOPPIES + major;

    return disk_get_capacity(unit, blocks, blocksize);
}

static long XHReadWrite(UWORD major, UWORD minor, UWORD rw, ULONG sector,
                 UWORD count, UBYTE *buf)
{
    UWORD unit;

    KDEBUG(("XHReadWrite(device=%u.%u, rw=%u, sector=%lu, count=%u, buf=%p)\n",
            major, minor, rw, sector, count, buf));

    if (next_handler) {
        long ret = next_handler(XHREADWRITE, major, minor, rw, sector, count, buf);
        if (ret != EINVFN && ret != EUNDEV && ret != EDRIVE)
            return ret;
    }

    if (minor != 0)
        return EUNDEV;

    unit = NUMFLOPPIES + major;

    return disk_rw(unit, rw, sector, count, buf);
}

/*=========================================================================*/

/* EmuTOS' XHDI cookie points to _xhdi_vec implemented in bios/natfeat.S.
 * It backs up all the registers according to the XHDI specification,
 * then calls the xhdi_handler() C implementation below.
 */
long xhdi_handler(UWORD *stack)
{
    UWORD opcode = *stack;

    switch (opcode)
    {
        case XHGETVERSION:
        {
            return XHGetVersion();
        }

        case XHINQTARGET:
        {
            struct XHINQTARGET_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                ULONG *blocksize;
                ULONG *deviceflags;
                char *productname;
            } *args = (struct XHINQTARGET_args *)stack;

            return XHInqTarget(args->major, args->minor, args->blocksize, args->deviceflags, args->productname);
        }

        case XHRESERVE:
        {
            struct XHRESERVE_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                UWORD do_reserve;
                UWORD key;
            } *args = (struct XHRESERVE_args *)stack;

            return XHReserve(args->major, args->minor, args->do_reserve, args->key);
        }

        case XHLOCK:
        {
            struct XHLOCK_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                UWORD do_lock;
                UWORD key;
            } *args = (struct XHLOCK_args *)stack;

            return XHLock(args->major, args->minor, args->do_lock, args->key);
        }

        case XHSTOP:
        {
            struct XHSTOP_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                UWORD do_stop;
                UWORD key;
            } *args = (struct XHSTOP_args *)stack;

            return XHStop(args->major, args->minor, args->do_stop, args->key);
        }

        case XHEJECT:
        {
            struct XHEJECT_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                UWORD do_eject;
                UWORD key;
            } *args = (struct XHEJECT_args *)stack;

            return XHEject(args->major, args->minor, args->do_eject, args->key);
        }

        case XHDRVMAP:
        {
            return XHDrvMap();
        }

        case XHINQDEV:
        {
            struct XHINQDEV_args
            {
                UWORD opcode;
                UWORD drv;
                UWORD *major;
                UWORD *minor;
                ULONG *start;
                BPB *bpb;
            } *args = (struct XHINQDEV_args *)stack;

            return XHInqDev(args->drv, args->major, args->minor, args->start, args->bpb);
        }

        case XHINQDRIVER:
        {
            struct XHINQDRIVER_args
            {
                UWORD opcode;
                UWORD bios_device;
                char *name;
                char *version;
                char *company;
                UWORD *ahdi_version;
                UWORD *maxIPL;
            } *args = (struct XHINQDRIVER_args *)stack;

            return XHInqDriver(args->bios_device, args->name, args->version, args->company, args->ahdi_version, args->maxIPL);
        }

        case XHNEWCOOKIE:
        {
            struct XHNEWCOOKIE_args
            {
                UWORD opcode;
                ULONG newcookie;
            } *args = (struct XHNEWCOOKIE_args *)stack;

            return XHNewCookie(args->newcookie);
        }

        case XHREADWRITE:
        {
            struct XHREADWRITE_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                UWORD rw;
                ULONG sector;
                UWORD count;
                UBYTE *buf;
            } *args = (struct XHREADWRITE_args *)stack;

            return XHReadWrite(args->major, args->minor, args->rw, args->sector, args->count, args->buf);
        }

        case XHINQTARGET2:
        {
            struct XHINQTARGET2_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                ULONG *blocksize;
                ULONG *deviceflags;
                char *productname;
                UWORD stringlen;
            } *args = (struct XHINQTARGET2_args *)stack;

            return XHInqTarget2(args->major, args->minor, args->blocksize, args->deviceflags, args->productname, args->stringlen);
        }

        case XHINQDEV2:
        {
            struct XHINQDEV2_args
            {
                UWORD opcode;
                UWORD drv;
                UWORD *major;
                UWORD *minor;
                ULONG *start;
                BPB *bpb;
                ULONG *blocks;
                char *partid;
            } *args = (struct XHINQDEV2_args *)stack;

            return XHInqDev2(args->drv, args->major, args->minor, args->start, args->bpb, args->blocks, args->partid);
        }

        case XHDRIVERSPECIAL:
        {
            struct XHDRIVERSPECIAL_args
            {
                UWORD opcode;
                ULONG key1;
                ULONG key2;
                UWORD subopcode;
                void *data;
            } *args = (struct XHDRIVERSPECIAL_args *)stack;

            return XHDriverSpecial(args->key1, args->key2, args->subopcode, args->data);
        }

        case XHGETCAPACITY:
        {
            struct XHGETCAPACITY_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                ULONG *blocks;
                ULONG *blocksize;
            } *args = (struct XHGETCAPACITY_args *)stack;

            return XHGetCapacity(args->major, args->minor, args->blocks, args->blocksize);
        }

        case XHMEDIUMCHANGED:
        {
            struct XHMEDIUMCHANGED_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
            } *args = (struct XHMEDIUMCHANGED_args *)stack;

            return XHMediumChanged(args->major, args->minor);
        }

        case XHMINTINFO:
        {
            struct XHMINTINFO_args
            {
                UWORD opcode;
                UWORD subopcode;
                void *data;
            } *args = (struct XHMINTINFO_args *)stack;

            return XHMiNTInfo(args->subopcode, args->data);
        }

        case XHDOSLIMITS:
        {
            struct XHDOSLIMITS_args
            {
                UWORD opcode;
                UWORD which;
                ULONG limit;
            } *args = (struct XHDOSLIMITS_args *)stack;

            return XHDOSLimits(args->which, args->limit);
        }

        case XHLASTACCESS:
        {
            struct XHLASTACCESS_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
                ULONG *ms;
            } *args = (struct XHLASTACCESS_args *)stack;

            return XHLastAccess(args->major, args->minor, args->ms);
        }

        case XHREACCESS:
        {
            struct XHREACCESS_args
            {
                UWORD opcode;
                UWORD major;
                UWORD minor;
            } *args = (struct XHREACCESS_args *)stack;

            return XHReaccess(args->major, args->minor);
        }

        default:
        {
            return EINVFN;
        }
    }
}

#endif /* CONF_WITH_XHDI */
