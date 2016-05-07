/*
 * lineavars.h - name of linea graphics related variables
 *
 * Copyright (c) 2001-2016 by Authors:
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Put in this file only the low-mem vars actually used by C code!
 */

#ifndef LINEAVARS_H
#define LINEAVARS_H

#include "portab.h"

/* Screen related variables */

/*
 * mouse cursor save area
 *
 * NOTE: the lineA version of this only has the first 64 ULONGs,
 * to handle a maximum of 4 video planes.  Writing into area[64]
 * and above when referencing the lineA version will overwrite
 * other lineA variables with unpredictable results.
 */
typedef struct {
        WORD    len;            /* height of saved form */
        UWORD   *addr;          /* screen address of saved form */
        BYTE    stat;           /* save status */
        BYTE    reserved;
        ULONG   area[8*16];     /* handle up to 8 video planes */
} MCS;
/* defines for 'stat' above */
#define MCS_VALID   0x01        /* save area is valid */
#define MCS_LONGS   0x02        /* saved data is in longword format */

extern const BYTE shift_offset[9];  /* pixel to address helper */
extern MCS *mcs_ptr;            /* ptr to mouse cursor save area in use */

extern UWORD v_planes;          /* count of color planes */
extern UWORD v_lin_wr;          /* line wrap : bytes per line */
extern UWORD v_cel_mx;          /* number of columns - 1 */
extern UWORD v_cel_my;          /* number of rows - 1 */
extern UWORD v_cur_cx;          /* current cursor column */
extern UWORD v_cur_cy;          /* current cursor row */
extern UWORD v_hz_rez;          /* screen horizontal resolution */
extern UWORD v_vt_rez;          /* screen vertical resolution */
extern UWORD v_bytes_lin;       /* width of line in bytes */

extern void linea_init(void);   /* initialize variables */

#endif /* LINEAVARS_H */
