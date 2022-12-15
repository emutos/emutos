/*
 * kpgmld.c - program load
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2013-2022 The EmuTOS development team
 *
 * Authors:
 *  SCC  Steven C. Cavender
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define ENABLE_KDEBUG

#include "emutos.h"
#include "fs.h"
#include "proc.h"
#include "gemerror.h"
#include "pghdr.h"
#include "string.h"
#include "program_loader.h"


/*
 *  load_program_into_memory - load program except the header (which has already been read)
 *
 *  The program space follows PD
 *
 * Arguments:
 * p - ptr to PD
 * h - file handle opened by read_program_header
 * hd - the program header read by read_program_header
 */
LONG load_program_into_memory(PD *p, FH h, LOAD_STATE *lstate)
{
    LONG r;

    r = lstate->loader->load_program_into_memory(h, p, lstate);

    KDEBUG(("BDOS load_program_into_memory: return code=0x%lx\n",r));

    xclose(h);
    return r;
}
