/*
 * acsi.h - Atari Computer System Interface (ACSI) support
 *
 * Copyright (C) 2002-2024 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef ACSI_H
#define ACSI_H

#if CONF_WITH_ACSI

/*
 * structure passed to send_command()
 */
typedef struct
{
    UBYTE *cdbptr;                  /* command address */
    WORD cdblen;                    /* command length */
    UBYTE *bufptr;                  /* buffer address */
    LONG buflen;                    /* buffer length */
    LONG timeout;                   /* in ticks */
    UBYTE rw;                       /* RW_READ or RW_WRITE */
} ACSICMD;


BOOL detect_acsi(void);
void acsi_init(void);
LONG acsi_ioctl(UWORD drv, UWORD ctrl, void *arg);
#if CONF_WITH_SCSI_DRIVER
LONG acsi_request_sense(WORD dev, UBYTE *buffer);
#endif
LONG acsi_rw(WORD rw, LONG sector, WORD count, UBYTE *buf, WORD dev);
int send_command(WORD dev,ACSICMD *cmd);

#endif /* CONF_WITH_ACSI */

#endif /* ACSI_H */
