/*
 * lineainit.c - linea graphics initialization
 *
 * Copyright (C) 2001-2020 by Authors:
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
 * Only the indexes 1, 2, 4 and 8 are meaningful.
 */
static const UBYTE shift_offset[9] = {0, 3, 2, 0, 1, 0, 0, 0, 0};

/*
 * Current value from above table, updated when v_planes changes to speed
 * up calculations.  To get the address of a pixel x in a scan line, use
 * the formula: (x&0xfff0)>>v_planes_shift
 */
UBYTE v_planes_shift;

/*
 * set_screen_shift() - sets v_planes_shift from the current value of v_planes
 */
void set_screen_shift(void)
{
    v_planes_shift = shift_offset[v_planes];
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
