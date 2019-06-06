
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
