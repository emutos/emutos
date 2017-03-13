/*
 * EmuTOS desktop - header for deskfun.c
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKFUN_H
#define _DESKFUN_H

/*
 * types of close for fun_close()
 */
#define CLOSE_FOLDER    0       /* display next higher folder in window */
#define CLOSE_WINDOW    1       /* close window entirely */
#define CLOSE_TO_ROOT   2       /* display root folder in window */

WORD fun_alert(WORD defbut, WORD stnum);
WORD fun_alert_merge(WORD defbut, WORD stnum, BYTE merge);
WORD fun_alert_long(WORD defbut, WORD stnum, LONG merge);
void fun_close(WNODE *pw, WORD closetype);
void fun_drag(WORD wh, WORD dest_wh, WORD sobj, WORD dobj, WORD mx, WORD my, WORD keystate);
void fun_msg(WORD type, WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
void fun_rebld(BYTE *path);
WORD fun_mkdir(WNODE *pw_node);
WORD fun_op(WORD op, WORD icontype_src, PNODE *pspath, BYTE *pdest);
void fun_del(WORD sobj);
BOOL wants_to_delete_files(void);

#endif
