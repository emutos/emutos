/*
 * processor.h - declarations for processor type check
 *
 * Copyright (c) 2001 EmuTOS development team.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _PROCESSOR_H
#define _PROCESSOR_H
 
#include "portab.h"

extern void processor_init(void);
extern LONG mcpu;
extern LONG fputype;
extern WORD longframe;

#endif /* PROCESSOR_H */
  
