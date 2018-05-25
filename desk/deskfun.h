/*
 * EmuTOS desktop - header for deskfun.c
 *
 * Copyright (C) 2002-2017 The EmuTOS development team
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

#if CONF_WITH_FORMAT
WORD fun_alert_long(WORD defbut, WORD stnum, LONG merge);
#endif

#if CONF_WITH_DESKTOP_SHORTCUTS
WORD fun_alert_string(WORD defbut, WORD stnum, BYTE *merge);
#endif

#if CONF_WITH_FILEMASK
void fun_mask(WNODE *pw);
#endif

void fun_close(WNODE *pw, WORD closetype);
BOOL fun_drag(WORD wh, WORD dest_wh, WORD sobj, WORD dobj, WORD mx, WORD my, WORD keystate);
void fun_msg(WORD type, WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
void fun_mark_for_rebld(BYTE *path);
void fun_rebld_marked(void);
void fun_rebld(BYTE *path);
WORD fun_mkdir(WNODE *pw_node);
WORD fun_op(WORD op, WORD icontype_src, PNODE *pspath, BYTE *pdest);
void fun_del(WORD sobj);
BOOL wants_to_delete_files(void);

/*
 * test if file in FNODE is selected
 */
static __inline__ BOOL fnode_is_selected(FNODE *fn)
{
    if ((fn->f_obid != NIL) && (G.g_screen[fn->f_obid].ob_state & SELECTED))
        return TRUE;

    return FALSE;
}

#endif
