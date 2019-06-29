/*
 * lineainit.c - linea graphics initialization
 *
 * Copyright (C) 2001-2019 by Authors:
 *
 * Authors:
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "lineavars.h"
#include "kprint.h"
#include "screen.h"
#include "bios.h"

#define EXTENDED_PALETTE (CONF_WITH_VIDEL || CONF_WITH_TT_SHIFTER)

MCS *mcs_ptr;   /* ptr to current mouse cursor save area, based on v_planes */

/*
 * linea_init - init linea variables
 */

void linea_init(void)
{

    screen_get_current_mode_info(&v_planes, &V_REZ_HZ, &V_REZ_VT);

    v_lin_wr = V_REZ_HZ / 8 * v_planes;     /* bytes per line */
    BYTES_LIN = v_lin_wr;       /* I think BYTES_LIN = v_lin_wr (PES) */

#if EXTENDED_PALETTE
    mcs_ptr = (v_planes <= 4) ? &mouse_cursor_save : &ext_mouse_cursor_save;
#else
    mcs_ptr = &mouse_cursor_save;
#endif

    /*
     * this is a convenient place to update the workstation xres/yres which
     * may have been changed by a Setscreen()
     */
    DEV_TAB[0] = V_REZ_HZ - 1;
    DEV_TAB[1] = V_REZ_VT - 1;

    KDEBUG(("planes: %d\n", v_planes));
    KDEBUG(("lin_wr: %d\n", v_lin_wr));
    KDEBUG(("hz_rez: %d\n", V_REZ_HZ));
    KDEBUG(("vt_rez: %d\n", V_REZ_VT));
}
