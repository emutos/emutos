/*
 * scsi.h - SCSI support
 *
 * Copyright (C) 2018-2019 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _SCSI_H
#define _SCSI_H

#if CONF_WITH_ACSI || CONF_WITH_SCSI

int build_rw_command(UBYTE *cdb, UWORD rw, ULONG sector, UWORD count);

#endif

#if CONF_WITH_SCSI

/*
 * structure passed to send_scsi_command() / scsi_dispatcher()
 */
typedef struct
{
    UBYTE *cdbptr;                  /* command address */
    WORD cdblen;                    /* command length */
    UBYTE *bufptr;                  /* buffer address */
    LONG buflen;                    /* buffer length */
    ULONG xfer_time;                /* calculated, in ticks */
    UBYTE mode;                     /* see below */
#define WRITE_MODE  0x01
#define DMA_MODE    0x02
    UBYTE next_msg_out;             /* next msg to send */
    UBYTE msg_in;                   /* first msg byte received */
    UBYTE status;                   /* (last) status byte received */
} CMDINFO;

BOOL detect_scsi(void);
void scsi_init(void);
LONG scsi_ioctl(WORD dev, UWORD ctrl, void *arg);
LONG scsi_request_sense(WORD dev, UBYTE *buffer);
LONG scsi_rw(UWORD rw, ULONG sector, UWORD count, UBYTE *buf, WORD dev);
LONG send_scsi_command(WORD dev, CMDINFO *info);

#endif /* CONF_WITH_SCSI */

#endif /* _SCSI_H */
