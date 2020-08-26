/*
 * natfeat.c - NatFeat library
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "natfeat.h"
#include "lineavars.h"

#if DETECT_NATIVE_FEATURES

static BOOL hasNF;

static long nfid_name;
static long nfid_stderr;
static long nfid_xhdi;
static long nfid_shutdown;
static long bootstrap_id;
static long nfid_config;

#define NF_CONFIG_MMU   0x5f4d4d55L /* '_MMU' */

BOOL detect_native_features(void);  /* defined in natfeat.S */

void natfeat_init(void)
{
    hasNF = detect_native_features();

    if (hasNF) {
        nfid_name = NFID("NF_NAME");
        nfid_stderr = NFID("NF_STDERR");
        nfid_xhdi = NFID("XHDI");
        nfid_shutdown = NFID("NF_SHUTDOWN");
        bootstrap_id = NFID("BOOTSTRAP");
        nfid_config = NFID("NF_CONFIG");
    }
    else {
        nfid_name = 0;
        nfid_stderr = 0;
        nfid_xhdi = 0;
        nfid_shutdown = 0;
        bootstrap_id = 0;
        nfid_config = 0;
    }
}

BOOL has_natfeats(void)
{
    return hasNF;
}

long nfGetFullName(char *buffer, long size)
{
    if (nfid_name) {
        return NFCall( nfid_name /*| 0x0001*/, buffer, size);
    }
    else {
        if (size >= 0) {
            buffer[0] = '\0';
        }
        return 0;
    }
}

BOOL is_nfStdErr(void)
{
    return nfid_stderr > 0;
}

long nfStdErr(const char *text)
{
    if (nfid_stderr) {
        return NFCall(nfid_stderr, text);
    }
    else {
        return 0;
    }
}

long get_xhdi_nfid(void)
{
    return nfid_xhdi;
}

/* terminate the execution of the emulator if possible, else no-op */
void nf_shutdown(void)
{
    if(nfid_shutdown) {
        NFCall(nfid_shutdown);
    } else {
        KINFO(("NF_SHUTDOWN not available\n"));
    }
}

/* check if nf_shutdown() is available */
BOOL has_nf_shutdown(void)
{
    return nfid_shutdown > 0;
}

/* load a new OS kernel into memory at 'addr' ('size' bytes available) */
long nf_bootstrap(UBYTE *addr, long size)
{
    if(hasNF) {
        if(bootstrap_id) {
            return NFCall(bootstrap_id, addr, size);
        } else {
            KINFO(("BOOTSTRAP natfeat not available\n"));
        }
    }
    return 0;
}

/* get the boot drive number */
UWORD nf_getbootdrive(void)
{
    if(hasNF) {
        if(bootstrap_id) {
            return NFCall(bootstrap_id | 0x0001);
        } else {
            KINFO(("BOOTSTRAP natfeat not available\n"));
        }
    }
    return 0;
}

/* get the bootstrap arguments */
long nf_getbootstrap_args(char *addr, long size)
{
    if(hasNF) {
        if(bootstrap_id) {
            return NFCall(bootstrap_id | 0x0002, addr, size);
        } else {
            KINFO(("BOOTSTRAP natfeat not available\n"));
        }
    }
    return 0;
}

/*
 * determine if host is emulating the MMU
 *
 * the NF_CONFIG feature was only defined in the summer of 2019.  for
 * systems not (yet) supporting this feature, we must return TRUE, since
 * the user may be running with MMU emulation.
 */
BOOL mmu_is_emulated(void)
{
    if(hasNF) {
        if(nfid_config) {
            return NFCall(nfid_config | 0x0001, NF_CONFIG_MMU);
        } else {
            KINFO(("NF_CONFIG not available\n"));
        }
    }
    return TRUE;
}

/*
 * propagate address of linea variables to emulators
 */
void nf_setlinea(void)
{
    BOOL err = TRUE;

    if(hasNF) {
        if(nfid_config) {
            if (NFCall(nfid_config | 0x0004, line_a_vars) >= 0)
                err = FALSE;
        }
        if (err) {
            KINFO(("NF_CONFIG not available\n"));
        }
    }
}
#endif /* DETECT_NATIVE_FEATURES */
