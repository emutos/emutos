
#ifndef GEMDOS_H
#define GEMDOS_H

extern UWORD    DOS_AX;
extern UWORD    DOS_ERR;


LONG dos_alloc(LONG nbytes);
WORD dos_free(LONG maddr);


#endif
