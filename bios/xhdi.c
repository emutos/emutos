/*
 * xhdi.c - XHDI handler
 *
 * Copyright (c) 2001,2002 EmuTOS development team
 *
 * Authors:
 *  joy   Petr Stehlik
 *
 *  xhdi_handler() inspired by ppzip driver, written by Frank Naumann
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "stdarg.h"
#include "kprint.h"
#include "blkdev.h"
#include "gemerror.h"
#include "acsi.h"
#include "string.h"
#include "cookie.h"
#include "natfeat.h"

#include "xhdi.h"
 
#define DBG_XHDI	0

/* NatFeats */
long nfid_xhdi;
#define nfCall(n)       (((long (*)(long, ...))__nfCall)n)

/*=========================================================================*/

void create_XHDI_cookie()
{
    cookie_add(COOKIE_XHDI, (long)xhdi_vec);
}

/*=========================================================================*/

long XHInqDev2(UWORD drv, UWORD *major, UWORD *minor, ULONG *start,
               BPB *bpb, ULONG *blocks, char *partid)
{
	long pstart = blkdev[drv].start;
	int mediachange = 0;
	BPB *myBPB;

#if DBG_XHDI
    kprintf("XHInqDev2(%d)\n", drv); 
#endif

	switch(drv) {
		default:
			if (major)  *major = blkdev[drv].unit-2;
			if (minor)  *minor = 0;
			if (bpb) bpb->recsiz = 0;
		
			if (!pstart)
				return EDRVNR;
		
			if (start)
				*start = pstart;

			myBPB = (BPB *)blkdev_getbpb(drv);
			if (bpb) memcpy(bpb, myBPB, sizeof(BPB));
		
			if (blocks)
				*blocks = blkdev[drv].size;
		
			if (partid)
			{
				partid[0] = blkdev[drv].id[0];
				partid[1] = blkdev[drv].id[1];
				partid[2] = blkdev[drv].id[2];
				partid[3] = '\0';
			}
		
			if (mediachange)
				return EDRVNR;
		
			return E_OK;
	}
}

long XHInqDev(UWORD drv, UWORD *major, UWORD *minor, ULONG *start,
              BPB *bpb)
{
    return XHInqDev2(drv, major, minor, start, bpb, NULL, NULL);
}

/*=========================================================================*/

long XHInqTarget2(UWORD major, UWORD minor, ULONG *blocksize,
                  ULONG *deviceflags, char *productname, UWORD stringlen)
{
    int ret;

#if DBG_XHDI
    kprintf("XHInqTarget2(%d, %d)\n", major, minor); 
#endif

    /* direct access to device */
    if (nfid_xhdi) {
        ret = nfCall((nfid_xhdi + XHINQTARGET2, (long)major, (long)minor, (long)blocksize, (long)deviceflags, (long)productname, (long)stringlen));
        if (ret != EINVFN && ret != EUNDEV)
            return ret;
    }

    if (blocksize) {
        /* TODO could add some heuristic here:
         * 1) create two buffers and fill first with zeros and second with $ff
         * 2) read first sector to both buffers
         * 3) find last common byte
         * 4) blocksize = index_of_last_common_byte + 1
         */
        *blocksize = 512;   /* usually physical sector size on HDD is 512 bytes */
    }
    if (deviceflags)
        *deviceflags = 0;  /* not implemented yet */
    if (productname)
        strcpy(productname, "EmuTOS Disk");

    return 0;
}

long XHInqTarget(UWORD major, UWORD minor, ULONG *blocksize,
                 ULONG *deviceflags, char *productname)
{
    return XHInqTarget2(major, minor, blocksize, deviceflags, productname, 33);
}

/*=========================================================================*/

long XHGetCapacity(UWORD major, UWORD minor, ULONG *blocks, ULONG *blocksize)
{
    if (nfid_xhdi) {
        long ret = nfCall((nfid_xhdi + XHGETCAPACITY, (long)major, (long)minor, (long)blocks, (long)blocksize));
        if (ret != EINVFN && ret != EUNDEV)
            return ret;
    }

    /* TODO could read the blocks from Atari root sector */
    return EINVFN;
}

/*=========================================================================*/

long XHReadWrite(UWORD major, UWORD minor, UWORD rw, ULONG sector,
                 UWORD count, void *buf)
{
	WORD dev = major;
#if DBG_XHDI
    kprintf("XH%s(%d, %d, %ld, %d, %p)\n", 
            rw ? "Write" : "Read", major, minor, sector, count, buf);
#endif

    /* direct access to device */
    if (nfid_xhdi) {
        long ret = nfCall((nfid_xhdi + XHREADWRITE, (long)dev, (long)0, (long)rw, (long)sector, (long)count, buf));
        if (ret != EINVFN && ret != EUNDEV)
            return ret;
    }

    if (minor != 0)
        return EUNDEV;

    /* hardware access to device */
    if (dev >= 0 && dev < 8) {
        return acsi_rw(rw, sector, count, (LONG)buf, dev);
    }
    else if (dev < 16) {
        return EUNDEV;  /* call scsi_rw() here when implemented */
    }
    else if (dev < 24) {
        return EUNDEV;  /* call ide_rw() here when implemented*/
    }
    return EUNDEV;      /* unknown device */
}

/*=========================================================================*/

long xhdi_handler(UWORD opcode, long a1, long a2, long a3, long a4,
                  long a5, long a6, long a7)
{
	typedef long (*wrap1)(long);
	typedef long (*wrap2)(long, long);
	typedef long (*wrap3)(long, long, long);
	typedef long (*wrap4)(long, long, long, long);
	typedef long (*wrap5)(long, long, long, long, long);
	typedef long (*wrap6)(long, long, long, long, long, long);
	typedef long (*wrap7)(long, long, long, long, long, long, long);
	
	switch (opcode)
	{
		case  XHGETVERSION:
		{
			return 0x130;
		}
		case  XHINQTARGET:
		{
			wrap4 f = (wrap4) XHInqTarget;
			return (*f)(a1, a2, a3, a4);
		}
		/*
		case  2:
		{
			wrap2 f = (wrap2) xhdi_handler_XHReserve;
			return (*f)(a1, a2);
		}
		case  3:
		{
			wrap2 f = (wrap2) xhdi_handler_XHLock;
			return (*f)(a1, a2);
		}
		case  4:
		{
			wrap2 f = (wrap2) xhdi_handler_XHStop;
			return (*f)(a1, a2);
		}
		case  5:
		{
			wrap2 f = (wrap2) xhdi_handler_XHEject;
			return (*f)(a1, a2);
		}
		*/
		case  6:
		{
			return blkdev_drvmap() & ~0x03;    /* FIXME */
		}
		case  XHINQDEV:
		{
			wrap5 f = (wrap5) XHInqDev;
			return (*f)(a1, a2, a3, a4, a5);
		}
		/*
		case  8:
		{
			wrap6 f = (wrap6) xhdi_handler_XHInqDriver;
			return (*f)(a1, a2, a3, a4, a5, a6);
		}
		case  9:
		{
			wrap1 f = (wrap1) xhdi_handler_XHNewCookie;
			return (*f)(a1);
		}
		*/
		case XHREADWRITE:
		{
			wrap4 f = (wrap4) XHReadWrite;
			return (*f)(a1, a2, a3, a4);
		}
		case XHINQTARGET2:
		{
			wrap5 f = (wrap5) XHInqTarget2;
			return (*f)(a1, a2, a3, a4, a5);
		}
		case XHINQDEV2:
		{
			wrap7 f = (wrap7) XHInqDev2;
			return (*f)(a1, a2, a3, a4, a5, a6, a7);
		}
		/*
		case 13:
		{
			wrap4 f = (wrap4) xhdi_handler_XHDriverSpecial;
			return (*f)(a1, a2, a3, a4);
		}
		*/
		case XHGETCAPACITY:
		{
			wrap3 f = (wrap3) XHGetCapacity;
			return (*f)(a1, a2, a3);
		}
		/*
		case 15:
		{
			wrap1 f = (wrap1) xhdi_handler_XHMediumChanged;
			return (*f)(a1);
		}
		case 16:
		{
			wrap2 f = (wrap2) xhdi_handler_XHMiNTInfo;
			return (*f)(a1, a2);
		}
		case 17:
		{
			wrap2 f = (wrap2) xhdi_handler_XHDOSLimits;
			return (*f)(a1, a2);
		}
		case 18:
		{
			wrap2 f = (wrap2) xhdi_handler_XHLastAccess;
			return (*f)(a1, a2);
		}
		case 19:
		{
			wrap1 f = (wrap1) xhdi_handler_XHReaccess;
			return (*f)(a1);
		}
		*/
	}
	
	return -1; /*ENOSYS;*/
}
