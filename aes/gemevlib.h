
#ifndef GEMEVLIB_H
#define GEMEVLIB_H

extern WORD     gl_dcrates[5];
extern WORD     gl_dcindex;
extern WORD     gl_dclick;
extern WORD     gl_ticktime;


WORD ev_block(WORD code, LONG lvalue);
UWORD ev_button(WORD bflgclks, UWORD bmask, UWORD bstate, WORD rets[]);
UWORD ev_mouse(MOBLK *pmo, WORD rets[]);
void ev_timer(LONG count);
WORD ev_multi(WORD flags, MOBLK *pmo1, MOBLK *pmo2, LONG tmcount,
              LONG buparm, LONG mebuff, WORD prets[]);
WORD ev_dclick(WORD rate, WORD setit);

#endif
