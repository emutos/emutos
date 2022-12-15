#include "mem.h"
#include "program_loader.h"
#include "string.h"
#include "sysconf.h"


extern const PROGRAM_LOADER prg_program_loader;
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
extern const PROGRAM_LOADER pgz_program_loader;
#endif


static const PROGRAM_LOADER *program_loaders[] = {
    &prg_program_loader
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
    , &pgz_program_loader
#endif
};


PROGRAM_LOADER *find_program_loader(FH fh) {
    PROGRAM_LOADER *loader;
    int i;
    int r;

    for (i = 0; i < sizeof(program_loaders)/sizeof(PROGRAM_LOADER); i++) {
        loader = (PROGRAM_LOADER*)&(program_loaders[i]);
        r = loader->can_load(fh);
        if (r == 1)
            return loader;
        else if (r == 0)
            r = xlseek(r, fh, 0/*SEEK_CUR*/);
        else if (r  < 0)
            break;
    }

    return NULL;
}
