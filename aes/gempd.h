
#ifndef GEMPD_H
#define GEMPD_H

EXTERN PD *pd_index(WORD i);
EXTERN PD *fpdnm(BYTE *pname, UWORD pid);
EXTERN PD *getpd(VOID);
EXTERN VOID p_nameit(PD *p, BYTE *pname);
EXTERN PD *pstart(BYTE *pcode, BYTE *pfilespec, LONG ldaddr);
EXTERN VOID insert_process(PD *pi, PD **root);

#endif
