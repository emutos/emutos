/*
 * Copyright (C) 2019 The EmuTOS development team
 *
 * Authors:
 *  THH   Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <osbind.h>
#include "nat_feat.h"

#ifndef CHANNEL
#define CHANNEL 1
#endif

int main(void)
{
    long count = 200;

    while (count-- > 0) {
        Bconout(CHANNEL, Bconin(CHANNEL));
    }

    Supexec(nf_shutdown);

    return 0;
}
