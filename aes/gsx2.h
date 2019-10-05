/*
 * gsx2.h - VDI (GSX) bindings
 *
 * Copyright (C) 2014-2019 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GSX2_H
#define GSX2_H

/* VDI parameter block */
typedef struct
{
    WORD *contrl;
    WORD *intin;
    WORD *ptsin;
    WORD *intout;
    WORD *ptsout;
} VDIPB;

extern VDIPB vdipb;

#define i_intin(p)  (vdipb.intin = (p))
#define i_ptsin(p)  (vdipb.ptsin = (p))
#define i_intout(p) (vdipb.intout = (p))
#define i_ptsout(p) (vdipb.ptsout = (p))
#define i_ptr(p)    (ULONG_AT(contrl+7) = (ULONG)(p))
#define i_ptr2(p)   (ULONG_AT(contrl+9) = (ULONG)(p))
#define m_lptr2(p)  (ULONG_AT(p) = ULONG_AT(contrl+9))

void gsx2(void);

#endif /* GSX2_H */
