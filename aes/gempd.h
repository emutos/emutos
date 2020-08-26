/*
 * gempd.h - header for EmuTOS AES process management functions
 *
 * Copyright (C) 2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMPD_H
#define GEMPD_H

/* returns the AESPD for the given index [NOT the pid] */
extern AESPD *pd_index(WORD i);

/* returns the AESPD for the given name, or if pname is NULL, the given pid) */
extern AESPD *fpdnm(char *pname, UWORD pid);

/* name an AESPD from the 8 first chars of the given string, stopping at the
 * first '.' (remove the file extension)
 */
extern void p_nameit(AESPD *p, char *pname);

/* set the application directory of an AESPD */
extern void p_setappdir(AESPD *p, char *pfilespec);

extern AESPD *pstart(PFVOID pcode, char *pfilespec, LONG ldaddr);

/* insert the process pi at the end of the process list pointed to by root */
extern void insert_process(AESPD *pi, AESPD **root);

#endif
