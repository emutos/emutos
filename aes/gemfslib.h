/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMFSLIB_H
#define GEMFSLIB_H

extern const BYTE gl_fsobj[];
extern LONG     ad_fstree;
extern LONG     ad_fsnames;
extern LONG     ad_fsdta;
extern GRECT    gl_rfs;

extern BYTE     gl_tmp1[];
extern BYTE     gl_tmp2[];

extern WORD     gl_shdrive;
extern WORD     gl_fspos;

WORD fs_input(LONG pipath, LONG pisel, WORD *pbutton);


#endif
