/*
 * Quick & dirty Malloc / Mfree stress test
 *
 * Compile with:
 *      m68k-atari-mint-gcc -o MEMSTRES.TOS -Wall memstres.c
 *
 * Copyright 2016 Christian Zietz <czietz@gmx.net>
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_STDLIB
void* _Malloc(unsigned long s) {
    return malloc(s);
}
int _Mfree(void *x) {
    free(x);
    return 0;
}
#else
#include <osbind.h>
#define _Malloc Malloc
#define _Mfree Mfree
#endif

#define MAX_SLOTS 1000
void* g_slots[MAX_SLOTS] = {NULL};
int g_slotsinuse = 0;
int g_nslots = 0;

/* Quick & dirty (mostly) portable random generator, but still better than some C stdlib implementations */
/* Idea is from Numerical Recipes in C, 2nd ed. */
/* Note that least significant bits have a very small period! */
unsigned long qdrand() {
    static unsigned long idum = 0;
    idum = 1664525L*idum + 1013904223L;
    return idum;
}

/* get block size to allocate */
unsigned long getblocksize(void) {
    unsigned long s;

    if (qdrand() & 8) {
        /* large block */
        s = (qdrand() & 0xFFFL) + 16L;
    } else {
        /* small block */
        s = (qdrand() & 0xFFL) + 16L;
    }

    return s;
}

int allocateslot(void) {
    int k;
    unsigned long size;

    if (g_slotsinuse == g_nslots) {
        /* all slots in use */
        return -1;
    }

    /* find first unused slot */
    for (k=0; g_slots[k] != NULL; k++)
    { /* nop */ }

    /* allocate memory */
    size = getblocksize();
    g_slots[k] = (void *)_Malloc(size);
    printf("Alloc %5ld bytes: %08lx\r\n", size, (unsigned long)g_slots[k]);

    /* check result */
    if (g_slots[k] != NULL) {
        g_slotsinuse++;
        return 1;
    } else {
        return 0;
    }
}

int freeslot(void) {
    int k, r;

    if (g_slotsinuse == 0) {
        /* no slot in use */
        return -1;
    }

    /* find a random slot to free */
    while (1) {

        k = qdrand() % g_nslots;
        if (g_slots[k] != NULL) {
            break;
        }
    }

    r = _Mfree(g_slots[k]);
    printf("Free %08lx: %d\r\n", (unsigned long)g_slots[k], r);

    if (r==0) {
        g_slots[k] = NULL;
        g_slotsinuse--;
        return 1;
    } else {
        return 0;
    }
}

int randomwalk(unsigned char p) {
    return ((unsigned char)(qdrand() & 0xFF) < p);
}

int main(int argc, char* argv[]) {

    /* allow the user to give the number of blocks to allocate */
    if (argc < 2) {
        g_nslots = 250;
    } else {
        g_nslots = atoi(argv[1]);
    }

    if ((g_nslots > 0) && (g_nslots <= MAX_SLOTS)) {
        printf("Running memory stress test with %d blocks\r\n", g_nslots);
    } else {
        printf("Must run with at least 1 and at most %d block!\r\n", MAX_SLOTS);
        return 1;
    }

    while (1) {
        printf("ALLOC PHASE\r\n");
        while (g_slotsinuse < g_nslots) {
            /* on average 75% allocation, 25% free */
            if (randomwalk(192)) {
                allocateslot();
            } else {
                freeslot();
            }
        }

        printf("\r\nFREE PHASE\r\n");
        while (g_slotsinuse > 0) {
            /* on average 75% free, 25% alloc */
            if (randomwalk(64)) {
                allocateslot();
            } else {
                freeslot();
            }
        }

        printf("\r\n");
    }
}
