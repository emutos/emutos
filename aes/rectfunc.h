
#ifndef RECTFUNC_H
#define RECTFUNC_H

UWORD inside(UWORD x, UWORD y, GRECT *pt);
WORD rc_equal(GRECT *p1, GRECT *p2);
void rc_copy(GRECT *psbox, GRECT *pdbox);
WORD min(WORD a, WORD b);
WORD max(WORD a, WORD b);

#endif
