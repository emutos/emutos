/*
 *  bios.h - bios defines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BIOS_H
#define _BIOS_H

#include "portab.h"



/*
 * BIOS level character device handles
 */

#define BFHPRN  0
#define BFHAUX  1
#define BFHCON  2


/*
 *  return codes
 */

#define DEVREADY        -1L             /*  device ready                */
#define DEVNOTREADY     0L              /*  device not ready            */
#define MEDIANOCHANGE   0L              /*  media def has not changed   */
#define MEDIAMAYCHANGE  1L              /*  media may have changed      */
#define MEDIACHANGE     2L              /*  media def has changed       */





/*
 *  bios data types
 */

/*
 *  SSN - Sequential Sector Numbers
 *      At the outermost level of support, the disks look like an
 *      array of sequential logical sectors.  The range of SSNs are
 *      from 0 to n-1, where n is the number of logical sectors on
 *      the disk.  (logical sectors do not necessarilay have to be
 *      the same size as a physical sector.
 */

typedef long    SSN ;

/*
 *  Data Structures
 */

/*
 *  PD - Process Descriptor
 */

#ifndef PD
#define PD struct _pd

PD
{
/* 0x00 */
        long    p_lowtpa;
        long    p_hitpa;
        long    p_tbase;
        long    p_tlen;
/* 0x10 */
        long    p_dbase;
        long    p_dlen;
        long    p_bbase;
        long    p_blen;
/* 0x20 */
        long    p_0fill[3] ;
        char    *p_env;
/* 0x30 */
        long    p_1fill[20] ;
/* 0x80 */
        char    p_cmdlin[0x80];
} ;
#endif



#endif /* _BIOS_H */

