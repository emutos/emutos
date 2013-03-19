/*
 * EmuTOS aes
 *
 * Copyright (c) 2002, 2010 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

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

/* set the application directory of a PD */
extern void p_setappdir(PD *p, BYTE *pfilespec);

extern PD *pstart(PFVOID pcode, BYTE *pfilespec, LONG ldaddr);

/* insert the process pi at the end of the process list pointed to by root */
extern void insert_process(PD *pi, PD **root);

#endif
