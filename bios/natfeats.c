/*
 * natfeat.c - NatFeat library
 *
 * Copyright (c) 2001-2003 EmuTOS development team
 *
 * Authors:
 *  joy   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "natfeat.h"
#include "kprint.h"

#if DETECT_NATIVE_FEATURES

static int hasNF;

static long nfid_name;
static long nfid_stderr;
static long nfid_xhdi;

int detect_native_features(void);    /* defined in natfeat.S */

void natfeat_init(void)
{
    hasNF = detect_native_features();

    if (hasNF) {
        nfid_name = NFID("NF_NAME");
        nfid_stderr = NFID("NF_STDERR");
        nfid_xhdi = NFID("XHDI");
    }
    else {
        nfid_name = 0;
        nfid_stderr = 0;
        nfid_xhdi = 0;
    }
}

int has_natfeats(void)
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

int is_nfStdErr(void)
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

/* terminate the execution of the emulato if possible, else no-op */
void nf_shutdown(void)
{
    if(hasNF) {
        long shutdown_id = NFID("NF_SHUTDOWN");
        if(shutdown_id) {
            NFCall(shutdown_id);
        } else {
            kprintf("NF_SHUTDOWN not available\n");
        }
    }
}

/* load a new OS kernel into the memory to 'addr' ('size' bytes available) */
long nf_bootstrap(char *addr, long size)
{
    if(hasNF) {
        long bootstrap_id = NFID("BOOTSTRAP");
        if(bootstrap_id) {
            return NFCall(bootstrap_id, addr, size);
        } else {
            kprintf("BOOTSTRAP natfeat not available\n");
        }
    }
    return 0;
}

/* get the boot drive number */
char nf_getbootdrive(void)
{
    if(hasNF) {
        long bootstrap_id = NFID("BOOTSTRAP");
        if(bootstrap_id) {
            return NFCall(bootstrap_id | 0x0001);
        } else {
            kprintf("BOOTSTRAP natfeat not available\n");
        }
    }
    return 0;
}

/* get the boot drive number */
long nf_getbootstrap_args(char *addr, long size)
{
    if(hasNF) {
        long bootstrap_id = NFID("BOOTSTRAP");
        if(bootstrap_id) {
            return NFCall(bootstrap_id | 0x0002, addr, size);
        } else {
            kprintf("BOOTSTRAP natfeat not available\n");
        }
    }
    return 0;
}

#endif /* DETECT_NATIVE_FEATURES */
