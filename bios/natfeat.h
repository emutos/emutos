
/*
 * natfeat.h - NatFeat header file
 *
 * Copyright (c) 2001-2003 EmuTOS development team
 *
 * Authors:
 *  joy   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

extern long xhdi_vec(void);
extern long __nfID(void);
extern long __nfCall(void);
extern void natfeat_cookie_struct(void);
extern void detect_native_features(void);
