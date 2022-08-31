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

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "fs.h"
#include "proc.h"
#include "gemerror.h"
#include "pghdr.h"
#include "string.h"
#include "program_loader.h"


/*
 * read_program_header - load program header
 *
 * contrary to what was before, we load the prg header first,
 * then allocate the basepage, choosing the memory pool according to
 * the flags and the amount of memory needed. Then, we actually load
 * the program.
 */
LONG read_program_header(FH h, PGMHDR01 *hd, PROGRAM_LOADER **found_loader)
{
    LONG r;
    UBYTE first_bytes[4];
    PROGRAM_LOADER *loader;

    /* Read the first few bytes. Hopefully it will contain a magic number for all supported formats */
    r = xread(h, sizeof(first_bytes), first_bytes);   /* read magic number */
    if (r < 0L)
        return r;
    if (r != sizeof(first_bytes))
        return EPLFMT;

    loader = find_program_loader(h, first_bytes);
    if (loader == 0L)
    {
        KDEBUG(("BDOS read_program_header: Unknown executable format!\n"));
        return EPLFMT;
    }

    /* read in the program header */
    r = loader->get_program_header(h, hd);
    if (r  < 0)
        return r; 

    *found_loader = loader; 
    return 0;
}


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
LONG load_program_into_memory(PD *p, FH h, PGMHDR01 *hd, const PROGRAM_LOADER *loader)
{
    LONG r;

    r = loader->load_program_into_memory(h, p, hd);

    KDEBUG(("BDOS load_program_into_memory: return code=0x%lx\n",r));

    xclose(h);
    return r;
}
