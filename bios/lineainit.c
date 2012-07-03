/*
 * lineainit.c - linea graphics initialization
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "config.h"
#include "lineavars.h"
#include "kprint.h"
#include "screen.h"

#define DBG_LINEA 0

BYTE shft_off;                  /* once computed Offset into a Scan Line */

/*
 * linea_init - init linea variables
 */

void linea_init(void)
{
    int n;

    screen_get_current_mode_info(&v_planes, &v_hz_rez, &v_vt_rez);

    v_lin_wr = v_hz_rez / 8 * v_planes;     /* bytes per line */
    v_bytes_lin = v_lin_wr;       /* I think v_bytes_lin = v_lin_wr (PES) */

    /* Calculate the shift offset
     * (used for screen modes that have the planes arranged in an
     *  interleaved fashion with a word for each plane). */
    for (n = v_planes, shft_off = 3; n > 1; n >>= 1)
        shft_off--;

#if DBG_LINEA
    kprintf("planes: %d\n", v_planes);
    kprintf("lin_wr: %d\n", v_lin_wr);
    kprintf("hz_rez: %d\n", v_hz_rez);
    kprintf("vt_rez: %d\n", v_vt_rez);
#endif
}
