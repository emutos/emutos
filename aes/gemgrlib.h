
#ifndef GEMGRLIB_H
#define GEMGRLIB_H


void gr_stepcalc(WORD orgw, WORD orgh, GRECT *pt, WORD *pcx, WORD *pcy,
                 WORD *pcnt, WORD *pxstep, WORD *pystep);

void gr_rubwind(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin,
                GRECT *poff, WORD *pwend, WORD *phend);
void gr_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin,
               WORD *pwend, WORD *phend);
void gr_dragbox(WORD w, WORD h, WORD sx, WORD sy, GRECT *pc, WORD *pdx, WORD *pdy);
void gr_2box(WORD flag1, WORD cnt, GRECT *pt, WORD xstep, WORD ystep, WORD flag2);
void gr_movebox(WORD w, WORD h, WORD srcx, WORD srcy, WORD dstx, WORD dsty);
WORD gr_watchbox(LONG tree, WORD obj, WORD instate, WORD outstate);
WORD gr_slidebox(LONG tree, WORD parent, WORD obj, WORD isvert);
void gr_mkstate(WORD *pmx, WORD *pmy, WORD *pmstat, WORD *pkstat);


#endif
