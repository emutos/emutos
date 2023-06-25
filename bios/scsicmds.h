/*
 * scsicmds.h - SCSI commands and related defines
 *
 * Copyright (C) 2023 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * This file is intended to include those elements that are common
 * between ACSI and SCSI (and eventually ATAPI)
 */

#if CONF_WITH_ACSI || CONF_WITH_SCSI

/*
 * standard operation codes
 */
#define REQUEST_SENSE   0x03
#define READ6           0x08
#define WRITE6          0x0a
#define INQUIRY         0x12
#define READ_CAPACITY   0x25
#define READ10          0x28
#define WRITE10         0x2a


/*
 * standard data transfer lengths
 */
#define INQUIRY_LENGTH  36
#define REQSENSE_LENGTH 18
#define READCAP_LENGTH  8


/*
 * other standard defines
 */
#define MAX_SCSI_CDBLEN 16      /* maximum legal SCSI CDB length */

#endif
