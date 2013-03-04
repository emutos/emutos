/*
 * EmuTOS desktop
 *
 * Copyright (c) 2002, 2010 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

extern BYTE     gl_amstr[4];
extern BYTE     gl_pmstr[4];

extern WORD     gl_apid;

extern GRECT    gl_savewin[];
extern GRECT    gl_normwin;
extern WORD     gl_whsiztop;


void xlate_obj_array(OBJECT *obj_array, int nobj);
void xlate_fix_tedinfo(TEDINFO *tedinfo, int nted);
ANODE *i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp);
WORD hndl_msg(void);
WORD deskmain(void);
