
#ifndef GEMPD_H
#define GEMPD_H

extern PD *pd_index(WORD i);
extern PD *fpdnm(BYTE *pname, UWORD pid);
extern PD *getpd(VOID);
extern void p_nameit(PD *p, BYTE *pname);
extern PD *pstart(void *pcode, BYTE *pfilespec, LONG ldaddr);
extern void insert_process(PD *pi, PD **root);

#endif
