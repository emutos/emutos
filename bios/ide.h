/*
 * ide.h - Falcon IDE functions
 *
 * Copyright (C) 2011-2024 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef IDE_H
#define IDE_H

#if CONF_WITH_IDE

BOOL detect_ide(void);
void ide_init(void);
LONG ide_ioctl(WORD dev, UWORD ctrl, void *arg);
LONG ide_rw(WORD rw,ULONG sector,UWORD count,UBYTE *buf,WORD dev,BOOL need_byteswap);

#if CONF_WITH_SCSI_DRIVER
/*
 * structure passed to send_ide_command()
 */
typedef struct
{
    UBYTE *cdbptr;                  /* command address */
    WORD cdblen;                    /* command length */
    UBYTE *bufptr;                  /* buffer address */
    LONG buflen;                    /* buffer length */
    LONG timeout;                   /* in ticks */
    UWORD flags;                    /* RW_READ or RW_WRITE */
} IDECmd;

LONG send_ide_command(WORD dev, IDECmd *cmd);
LONG ide_request_sense(WORD dev, WORD buflen, UBYTE *buffer);
#endif

#endif /* CONF_WITH_IDE */

#endif /* IDE_H */
