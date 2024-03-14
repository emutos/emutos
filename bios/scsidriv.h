/*
 * scsidriv.h - header for SCSI driver routines
 *
 * Copyright (C) 2023-2024 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define SCSIDRIV_VERSION    0x0101

#define NUM_SCSIDRIV_HANDLES 32

/* standard error return codes from SCSI driver */
#define SELECT_ERROR        -1L
#define STATUS_ERROR        -2L
#define PHASE_ERROR         -3L
#define BUS_ERROR           -5L
#define BUSFREE_ERROR       -7L
#define TIMEOUT_ERROR       -8L
#define DATATOOLONG_ERROR   -9L
#define ARBITRATE_ERROR     -11L
#define PENDING_ERROR       -12L

/* used for SCSIId */
typedef struct
{
    ULONG hi;
    ULONG lo;
} DLONG;

typedef UWORD *SCSIHandle;

/*
 * structure passed to input & output routines
 */
typedef struct
{
    SCSIHandle handle;
    UBYTE *cdb;                 /* pointer to CDB */
    UWORD cdblen;               /* CDB length */
    void *buffer;               /* data buffer */
    ULONG xferlen;              /* transfer length */
    UBYTE *sensebuf;            /* buffer for Request Sense (18 Bytes) */
    ULONG timeout;              /* in ticks (5msec units) */
    UWORD flags;                /* ignored by us */
} SCSICmd;

/*
 * private info area for driver
 */
typedef struct
{
    ULONG busavail;
    UBYTE reserved[28];
} SCSIPrivate;

typedef struct
{
    SCSIPrivate private;        /* for driver use only, subject to change */
    char busname[20];           /* e.g. 'SCSI', 'ACSI', 'IDE' */
    UWORD busnum;
    UWORD features;
        #define cArbit      0x01    /* bus supports arbitration */
        #define cAllCmds    0x02    /* can use all SCSI commands */
        #define cTargCtrl   0x04    /* target-controlled (standard for SCSI) */
                                    /* other features are never set by us */
    ULONG maxlen;               /* maximum transfer length on this bus */
} BusInfo;

typedef struct
{
    UBYTE private[32];          /* driver use only */
    DLONG SCSIId;
} DevInfo;

/*
 * root SCSI driver structure, pointed to by 'SCSI' cookie
 *
 * note that we only support initiator routines
 */
typedef struct
{
    UWORD version;          /* in BCD: 0x0100 = 1.00 */

    LONG (*In)(SCSICmd *cmd);
    LONG (*Out)(SCSICmd *cmd);
    LONG (*InquireSCSI)(WORD what, BusInfo *info);
        #define cInqFirst   0   /* values for 'what' (both InquireSCSI() & InquireBus() */
        #define cInqNext    1
    LONG (*InquireBus)(WORD what, WORD busnum, DevInfo *info);
    LONG (*CheckDev)(WORD busnum, const DLONG *SCSIId, char *name, UWORD *features);
    LONG (*RescanBus)(WORD busnum);
    LONG (*Open)(WORD busnum, const DLONG *SCSIId, ULONG *MaxLen);
    LONG (*Close)(SCSIHandle handle);
    LONG (*Error)(SCSIHandle handle, WORD rwflag, WORD errnum);
        #define cErrRead    0   /* values for 'rwflag' */
        #define cErrWrite   1
        #define cErrMediach 0   /* bit numbers for 'errnum' */
        #define cErrReset   1
} SCSIRoot;

extern SCSIRoot scsidriv_root;

void scsidriv_init(void);
