/*
 * lineavars.h - name of linea graphics related variables
 *
 * Copyright (c) 2001-2013 by Authors:
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

extern const BYTE shift_offset[5];  /* pixel to address helper */

extern UWORD v_planes;          /* count of color planes */
extern UWORD v_lin_wr;          /* line wrap : bytes per line */
extern UWORD v_cel_mx;          /* number of columns - 1 */
extern UWORD v_cel_my;          /* number of rows - 1 */
extern UWORD v_hz_rez;          /* screen horizontal resolution */
extern UWORD v_vt_rez;          /* screen vertical resolution */
extern UWORD v_bytes_lin;       /* width of line in bytes */

extern void linea_init(void);   /* initialize variables */

#endif /* LINEAVARS_H */
