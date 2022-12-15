#ifndef PROGRAM_LOADER_H
#define PROGRAM_LOADER_H

#include "portab.h"
#include "fs.h"
#include "pghdr.h"


typedef struct load_state_t {
    struct program_loader_t *loader;

    /* Owner to set for any memory block allocated to the child process */ 
    PD *owner;

    /* Filled in by get_program_info */
    ULONG flags;            /* flags as per PGMHDR01.h01_flags */
    ULONG relocatable_size; /* size of relocatable block to allocate (may be 0) */
    
    /* Filled by load_program_into_memory */
    void *prg_entry_point;  /* entry point of the program (address of first instruction to execute) */
    
    void *data;             /* free for use by the loader, to store info about a file being loaded */
} LOAD_STATE;


typedef struct program_loader_t {
    /* Tells whether this loader can manage that executable format.
     * Returns < 0 if error, 1 if success, 0 if file cannot be handled by the loader. */
    WORD (*can_load)(FH fh);

    /* The program loader must read or make up a program header */
    WORD (*get_program_info)(FH h, LOAD_STATE *lstate);

    /* Load the program into memory and tweak the PD accordingly. 
     * It's allowed to allocate memory */
    LONG (*load_program_into_memory)(FH h, PD *pdptr, LOAD_STATE *lstate);
} PROGRAM_LOADER;


PROGRAM_LOADER *find_program_loader(FH fh);

#endif