#ifndef PROGRAM_LOADER_H
#define PROGRAM_LOADER_H

#include "portab.h"
#include "fs.h"
#include "pghdr.h"


typedef struct program_loader_t {
    WORD (*can_load)(const UBYTE *first_bytes);
    WORD (*get_program_header)(FH h, PGMHDR01 *hd);
    LONG (*load_program_into_memory)(FH h, PD *pdptr, PGMHDR01 *hd);
} PROGRAM_LOADER;

PROGRAM_LOADER *find_program_loader(FH fh, const UBYTE *first_bytes);

#endif