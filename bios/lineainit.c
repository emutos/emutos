/*
 * lineainit.c - linea graphics initialization
 *
 * Copyright (C) 2001-2016 by Authors:
 *
 * Authors:
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "lineavars.h"
#include "kprint.h"
#include "screen.h"

#define DBG_LINEA 0

/* Precomputed value of log2(8/v_planes).
 * To get the address of a pixel x in a scan line, use the formula:
 * (x&0xfff0)>>shift_offset[v_planes]
 * Only the indexes 1, 2, 4 and 8 are meaningful.
 */
const BYTE shift_offset[9] = {0, 3, 2, 0, 1, 0, 0, 0, 0};

/*
 * mouse cursor save areas
 */
extern MCS mouse_cursor_save;       /* in linea variable area */
static MCS ext_mouse_cursor_save;   /* use for v_planes > 4 */

MCS *mcs_ptr;   /* ptr to current mouse cursor save area, based on v_planes */

/*
 * linea_init - init linea variables
 */

void linea_init(void)
{

    screen_get_current_mode_info(&v_planes, &v_hz_rez, &v_vt_rez);

    v_lin_wr = v_hz_rez / 8 * v_planes;     /* bytes per line */
    v_bytes_lin = v_lin_wr;       /* I think v_bytes_lin = v_lin_wr (PES) */

    mcs_ptr = (v_planes <= 4) ? &mouse_cursor_save : &ext_mouse_cursor_save;

#if DBG_LINEA
    kprintf("planes: %d\n", v_planes);
    kprintf("lin_wr: %d\n", v_lin_wr);
    kprintf("hz_rez: %d\n", v_hz_rez);
    kprintf("vt_rez: %d\n", v_vt_rez);
#endif
}
