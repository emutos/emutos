/*
 * bdosbind.h - Bindings for BDOS system calls
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BDOSBIND_H
#define _BDOSBIND_H

#include "bdosdefs.h"

/* OS entry points implemented in util/miscasm.S */
extern long trap1(int, ...); /* Not reentrant! Do not call for Pexec() */
extern long trap1_pexec(short mode, const char *path, const char *tail, const char *env);

#define Crawio(w) trap1(0x06, w)
#define Cconws(buf) trap1(0x09, buf)
#define Dsetdrv(drv) trap1(0x0e, drv)
#define Fsetdta(buf) trap1(0x1a, buf)
#define Malloc(number) trap1(0x48, number)
#define Mfree(block) trap1(0x49, block)
#define Pexec(mode,name,cmdline,env) trap1_pexec(mode, name, cmdline, env)
#define Fsfirst(filename,attr) trap1(0x4e, filename, attr)
#define Fsnext() trap1(0x4f)

#endif /* _BDOSBIND_H */
