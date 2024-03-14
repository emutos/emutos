/*
 * lineainit.c - linea graphics initialization
 *
 * Copyright (C) 2001-2024 by Authors:
 *
 * Authors:
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "lineavars.h"
#include "screen.h"
#include "bios.h"
#include "vdiext.h"

/*
 * Precomputed value of log2(8/v_planes), used to derive v_planes_shift.
 * Only the indexes 1, 2 and 4 are meaningful (see set_screen_shift()).
 */
static const UBYTE shift_offset[5] = {0, 3, 2, 0, 1};

/*
 * Current shift value, used to speed up calculations; changed when v_planes
 * changes.  To get the address of a pixel x in a scan line in a bit-plane
 * resolution, use the formula: (x&0xfff0)>>v_planes_shift
 */
UBYTE v_planes_shift;

/*
 * set_screen_shift() - sets v_planes_shift from the current value of v_planes
 *
 * . v_planes==8 (used by both Falcon & TT) has a shift value of 0
 *
 * . v_planes==16 indicates Falcon 16-bit mode which does not use bit planes,
 *   so we also set v_planes_shift to 0 (it should not be accessed)
 */
void set_screen_shift(void)
{
    v_planes_shift = (v_planes > 4) ? 0 : shift_offset[v_planes];
}

/*
 * linea_init - init linea variables
 */
void linea_init(void)
{
    screen_get_current_mode_info(&v_planes, &V_REZ_HZ, &V_REZ_VT);

    /* precalculate shift value to optimize pixel address calculations */
    set_screen_shift();

    /* update resolution-dependent values */
    update_rez_dependent();

    KDEBUG(("linea_init(): %dx%d %d-plane (v_lin_wr=%d)\n",
            V_REZ_HZ, V_REZ_VT, v_planes, v_lin_wr));
}
