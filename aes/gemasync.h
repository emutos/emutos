
#ifndef GEMASYNC_H
#define GEMASYNC_H

void azombie(EVB *e, UWORD ret);
void evinsert(EVB *e, EVB **root);
EVSPEC mwait(EVSPEC mask);
EVSPEC iasync(WORD afunc, LONG aparm);
UWORD apret(EVSPEC mask);
EVSPEC acancel(EVSPEC m);

#endif
