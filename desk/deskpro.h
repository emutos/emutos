
#ifndef DESKPRO_H
#define DESKPRO_H

WORD pro_chdir(WORD drv, BYTE *ppath);
WORD pro_run(WORD isgraf, WORD isover, WORD wh, WORD curr);
WORD pro_exit(LONG pcmd, LONG ptail);

#endif
