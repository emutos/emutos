
#ifndef GEMPD_H
#define GEMPD_H

/* returns the PD for the given index [NOT the pid] */
extern PD *pd_index(WORD i);

/* returns the PD for the given name, or if pname is NULL, the given pid) */
extern PD *fpdnm(BYTE *pname, UWORD pid);

/* name a PD from the 8 first chars of the given string, stopping at the 
 * first '.' (remove the file extension)
 */
extern void p_nameit(PD *p, BYTE *pname);

extern PD *pstart(void *pcode, BYTE *pfilespec, LONG ldaddr);

/* insert the process pi at the end of the process list pointed to by root */
extern void insert_process(PD *pi, PD **root);

#endif
