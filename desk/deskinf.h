/*
 * EmuTOS desktop: header for deskinf.c
 *
 * Copyright (c) 2002-2013 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKINF_H
#define _DESKINF_H

WORD dr_code(PARMBLK *pparms);
WORD inf_show(LONG tree, WORD start);
WORD inf_file_folder(BYTE *ppath, FNODE *pf);
WORD inf_disk(BYTE dr_id);
WORD inf_pref(void);
WORD opn_appl(BYTE *papname, BYTE *papparms, BYTE *pcmd, BYTE *ptail);

#endif
