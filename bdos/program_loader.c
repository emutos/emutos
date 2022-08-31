#include "program_loader.h"
#include "string.h"

extern const PROGRAM_LOADER prg_program_loader;
static const PROGRAM_LOADER *program_loaders[] = {
    &prg_program_loader
};

PROGRAM_LOADER *find_program_loader(FH fh, const UBYTE *first_bytes) {
    PROGRAM_LOADER *loader;
    int i;
    int r;
    
    for (i = 0; i < sizeof(program_loaders)/sizeof(PROGRAM_LOADER); i++) {
        loader = (PROGRAM_LOADER*)&(program_loaders[i]);
        r = loader->can_load(first_bytes);
        if (r >= 0)
            return loader;
        
        /* skip the magic number */
        r = xlseek(r, fh, 0/*SEEK_CUR*/);
        if (r  < 0)
            break; 

        return loader;
    }

    return NULL;
}
