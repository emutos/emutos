
#ifndef GEMWRECT_H
#define GEMWRECT_H

extern ORECT    *rul;
extern ORECT    gl_mkrect;


void or_start();
ORECT *get_orect();
void mkrect(LONG tree, WORD wh);
void newrect(LONG tree, WORD wh);

#endif
