
#ifndef GEMFMLIB_H
#define GEMFMLIB_H

extern LONG   ad_g2loc;

void fm_own(WORD beg_ownit);

WORD fm_keybd(LONG tree, WORD obj, WORD *pchar, WORD *pnew_obj);
WORD fm_button(LONG tree, WORD new_obj, WORD clks, WORD *pnew_obj);
WORD fm_do(LONG tree, WORD start_fld);
WORD fm_dial(WORD fmd_type, GRECT *pt);
WORD fm_show(WORD string, UWORD *pwd, WORD level);
WORD fm_error(WORD n);

#endif
