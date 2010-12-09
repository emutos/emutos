/*
 * EmuTOS desktop
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

extern BYTE     gl_amstr[4];
extern BYTE     gl_pmstr[4];

extern WORD     ig_close;

extern WORD     gl_apid;

extern WORD     gl_swtblks[3];

extern LONG     ad_ptext;
extern LONG     ad_picon;

extern GRECT    gl_savewin[];
extern GRECT    gl_normwin;
extern WORD     gl_open1st;
extern BYTE     gl_defdrv;
extern WORD     can_iapp;
extern WORD     can_show;
extern WORD     can_del;
extern WORD     can_output;
extern WORD     gl_whsiztop;
extern WORD     gl_idsiztop;



ANODE *i_find(WORD wh, WORD item, FNODE **ppf, WORD *pisapp);
WORD hndl_msg(void);
WORD deskmain(void);

