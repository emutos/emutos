
#ifndef GEMOBJOP_H
#define GEMOBJOP_H

BYTE ob_sst(LONG tree, WORD obj, LONG *pspec, WORD *pstate, WORD *ptype,
            WORD *pflags, GRECT *pt, WORD *pth);
void everyobj(LONG tree, WORD this, WORD last, WORD (*routine)(),
              WORD startx, WORD starty, WORD maxdep);
WORD get_par(LONG tree, WORD obj, WORD *pnobj);


#endif
