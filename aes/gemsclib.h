
#ifndef GEMSCLIB_H
#define GEMSCLIB_H


extern LONG  ad_scrap;
extern BYTE  *sc_types[];
extern WORD  sc_bits[];

WORD sc_read(LONG pscrap);
WORD sc_write(LONG pscrap);
WORD sc_clear();


#endif
