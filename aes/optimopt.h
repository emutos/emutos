
#ifndef OPTIMOPT_H
#define OPTIMOPT_H

void r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph);
void r_set(GRECT *pxywh, WORD x, WORD y, WORD w, WORD h);
BYTE *scasb(BYTE *p, BYTE b);
void bfill(WORD num, BYTE bval, void *addr);
WORD strchk(BYTE *s,  BYTE *t);

#endif
