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
#define Crawcin() trap1(0x07)
#define Cconws(buf) trap1(0x09, buf)
#define Cconis() trap1(0x0b)
#define Dsetdrv(drv) trap1(0x0e, drv)
#define Dgetdrv() trap1(0x19)
#define Fsetdta(buf) trap1(0x1a, buf)
#define Fgetdta() trap1(0x2f)
#define Dcreate(path) trap1(0x39, path)
#define Dfree(buf,driveno) trap1(0x36, buf, driveno)
#define Ddelete(path) trap1(0x3a, path)
#define Dsetpath(path) trap1(0x3b, path)
#define Fcreate(fname,attr) trap1(0x3c, fname, attr)
#define Fopen(fname,mode) trap1(0x3d, fname, mode)
#define Fclose(handle) trap1(0x3e, handle)
#define Fread(handle,count,buf) trap1(0x3f, handle, count, buf)
#define Fwrite(handle,count,buf) trap1(0x40, handle, count, buf)
#define Fdelete(fname) trap1(0x41, fname)
#define Fseek(offset,handle,seekmode) trap1(0x42, offset, handle, seekmode)
#define Fattrib(filename,wflag,attrib) trap1(0x43, filename, wflag, attrib)
#define Mxalloc(amount,mode) trap1(0x44, amount, mode)
#define Dgetpath(path,driveno) trap1(0x47, path, driveno)
#define Malloc(number) trap1(0x48, number)
#define Mfree(block) trap1(0x49, block)
#define Mshrink(block,newsiz) trap1(0x4a, 0, block, newsiz)
#define Pexec(mode,name,cmdline,env) trap1_pexec(mode, name, cmdline, env)
#define Fsfirst(filename,attr) trap1(0x4e, filename, attr)
#define Fsnext() trap1(0x4f)
#define Frename(oldname,newname) trap1(0x56, 0, oldname, newname)
#define Fdatime(timeptr,handle,wflag) trap1(0x57, timeptr, handle, wflag)

#endif /* _BDOSBIND_H */
