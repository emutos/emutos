/*
 * EmuTOS desktop
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


WORD fun_alert(WORD defbut, WORD stnum, WORD *pwtemp);
void fun_msg(WORD type, WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
void fun_rebld(WNODE *pwin);
WORD fun_mkdir(WNODE *pw_node);
WORD fun_op(WORD op, PNODE *pspath, BYTE *pdest);
void fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly, WORD keystate);
void fun_del(WNODE *pdw);
