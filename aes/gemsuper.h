
#ifndef GEMSUPER_H
#define GEMSUPER_H

EXTERN WORD     gl_bvdisk;
EXTERN WORD     gl_bvhard;
EXTERN WORD     gl_dspcnt;
EXTERN WORD     gl_mnpds[NUM_PDS];
EXTERN WORD     gl_mnclick;


UWORD crysbind(WORD opcode, REG LONG pglobal, REG UWORD int_in[], REG UWORD int_out[], REG LONG addr_in[])

#endif
