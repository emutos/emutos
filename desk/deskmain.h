/*
 * EmuTOS desktop
 *
 * Copyright (C) 2002-2015 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKMAIN_H
#define _DESKMAIN_H

extern BYTE     gl_amstr[4];
extern BYTE     gl_pmstr[4];

extern WORD     gl_apid;

extern GRECT    gl_savewin[];
extern GRECT    gl_normwin;


void xlate_obj_array(OBJECT *obj_array, int nobj);
void xlate_fix_tedinfo(TEDINFO *tedinfo, int nted);
WORD hndl_msg(void);
WORD deskmain(void);
void centre_title(LONG tree);

#endif  /* _DESKMAIN_H */
