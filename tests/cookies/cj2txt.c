/*
 * Copyright (C) 2019 The EmuTOS development team
 *
 * Authors:
 *  THH   Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <osbind.h>
#include "nat_feat.h"

struct cookie
{
    unsigned long id;
    unsigned long value;
};

int main(void)
{
    struct cookie *cookiejar;
    FILE *fh;
    char name[5];

    cookiejar = (struct cookie *)(Setexc(0x05A0/4, (const void (*)(void))-1));
    if (!cookiejar) {
        printf("No cookie jar available!\n");
        return 1;
    }

    fh = fopen("COOKIES.TXT", "wb");
    if (!fh) {
        printf("Can not open COOKIES.TXT\n");
        return 1;
    }

    name[4] = 0;

    while (cookiejar->id) {
        memcpy(name, &cookiejar->id, 4);
        printf("%s : 0x%08lx\n", name, cookiejar->value);
        fprintf(fh, "%s : 0x%08lx\n", name, cookiejar->value);
        ++cookiejar;
    }

    fclose(fh);

    Supexec(nf_shutdown);

    return 0;
}
