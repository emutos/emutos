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

void detect_scsi(void);
void scsi_init(void);
LONG scsi_ioctl(WORD dev, UWORD ctrl, void *arg);
LONG scsi_rw(UWORD rw, ULONG sector, UWORD count, UBYTE *buf, WORD dev);

#endif /* CONF_WITH_SCSI */

#endif /* _SCSI_H */
