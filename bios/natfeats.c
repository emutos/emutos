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

#include "natfeat.h"

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
        return NFCall( nfid_name | 0x0001, buffer, size);
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
