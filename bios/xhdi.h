/*
 * EmuTOS bios
 *
 * Copyright (C) 2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _XHDI_H
#define _XHDI_H

#define XHGETVERSION    0
#define XHINQTARGET     1
#define XHRESERVE       2
#define XHLOCK          3
#define XHSTOP          4
#define XHEJECT         5
#define XHDRVMAP        6
#define XHINQDEV        7
#define XHINQDRIVER     8
#define XHNEWCOOKIE     9
#define XHREADWRITE     10
#define XHINQTARGET2    11
#define XHINQDEV2       12
#define XHDRIVERSPECIAL 13
#define XHGETCAPACITY   14
#define XHMEDIUMCHANGED 15
#define XHMINTINFO      16
#define XHDOSLIMITS     17
#define XHLASTACCESS    18
#define XHREACCESS      19

/* values in device_flags for XHInqTarget(), XHInqTarget2() */
#define XH_TARGET_REMOVABLE 0x02L

/* Information for XHInqDriver() */
#define DRIVER_NAME                 "EmuTOS"
#define DRIVER_NAME_MAXLENGTH       17
#define DRIVER_COMPANY              "EmuTOS Team"
#define DRIVER_COMPANY_MAXLENGTH    17
#define DRIVER_VERSION              "1"
#define DRIVER_VERSION_MAXLENGTH    7
#define MAX_IPL                     5 /* 5 for drivers which use _hz_200 for timing loops */

#if CONF_WITH_XHDI

#include "portab.h"

extern long xhdi_vec(UWORD opcode, ...); /* In bios/natfeat.S */

void create_XHDI_cookie(void);
void init_XHDI_drvmap(void);
long xhdi_handler(UWORD *stack);

#endif /* CONF_WITH_XHDI */

#endif /* _XHDI_H */
