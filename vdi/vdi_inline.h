/*
 * vdi_inline.h - Definitions of VDI inline functions
 *
 * Copyright 2024-2025 The EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _VDI_INLINE_H
#define _VDI_INLINE_H

#if CONF_WITH_VDI_16BIT
static __inline__ UWORD *get_start_addr16(const WORD x, const WORD y)
{
    return (UWORD *)(v_bas_ad + (x * sizeof(WORD)) + muls(y, v_lin_wr));
}
#endif

#endif                          /* _VDI_INLINE_H */
