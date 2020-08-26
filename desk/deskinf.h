/*
 * deskinf.h - header for EmuDesk's deskinf.c
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKINF_H
#define _DESKINF_H

WORD dr_code(PARMBLK *pparms);
WORD inf_show(OBJECT *tree, WORD start);
WORD inf_file_folder(char *ppath, FNODE *pf);
WORD inf_disk(char dr_id);
void inf_numset(OBJECT *tree, WORD obj, ULONG value);
WORD inf_pref(void);
void set_tedinfo_name(OBJECT *tree, WORD obj, char *str);
WORD opn_appl(char *papname, char *ptail);
void start_dialog(OBJECT *tree);
void end_dialog(OBJECT *tree);

#if CONF_WITH_BACKGROUNDS
BOOL inf_backgrounds(void);
#endif

#if CONF_WITH_DESKTOP_CONFIG
void inf_conf(void);
#endif

#endif  /* _DESKINF_H */
