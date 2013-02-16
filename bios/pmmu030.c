/*
 * pmmu030.c - initialisation for 68030 PMMU
 *
 * Copyright (c) 2013 EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "config.h"
#include "portab.h"
#include "string.h"
#include "tosvars.h"
#include <stddef.h>                 /* for 'offsetof' */

/*
 * pmmu tree
 */
struct pmmutable
{
    LONG tia[16];
    LONG tib1[16];
    LONG tib2[16];
    LONG tic[16];
};

/*
 * pmmu descriptor flags
 */
#define PMMU_FLAGS_PD   0x01        /* short-format page descriptor */
#define PMMU_FLAGS_TD   0x02        /* short-format table descriptor */
#define PMMU_FLAGS_CI   0x40        /* cache inhibit (page descriptors only) */


#define mmutable_ram (*(struct pmmutable *)PMMUTREE_ADDRESS_68030)

/*
 * some macros useful for table creation
 */
#define PMMU_SF_TABLE(table)    ( ((LONG)&mmutable_ram.table[0]) | PMMU_FLAGS_TD )
#define PMMU_SF_PAGE(addr)      ( ((LONG)(addr)) | PMMU_FLAGS_PD )
#define PMMU_SF_PAGE_CI(addr)   ( ((LONG)(addr)) | PMMU_FLAGS_CI | PMMU_FLAGS_PD )


#if CONF_WITH_68030_PMMU
/*
 * SPECIAL NOTE
 * The PMMU tree used here replicates that used by the Falcon.  The tree
 * used by the TT differs in only one area: the 'tib2' table on the TT
 * maps the logical address range 0xf0000000-0xfeffffff to physical
 * addresses 0x00000000-0x0effffff rather than 0xf0000000-0xfeffffff.
 * Since that logical address range is not used on either machine, and
 * since Transparent Translation register 1 covers it anyway, there seems
 * to be no reason not to use the same PMMU tree for both machines.
 *
 * IMPORTANT: for safety, the tree must be in protected memory (below 0x800).
 * it currently starts at 0x700; therefore it must be no more than 0x100
 * bytes in length.
 */

/*
 * 68030 PMMU tree
 */
static const struct pmmutable mmutable_rom =
{
    /* tia: top level table (for 0x00000000-0xffffffff) */
    {
      PMMU_SF_TABLE(tib1),          /* for 0x00000000-0x0fffffff, use table tib1 */
      PMMU_SF_PAGE(0x10000000),     /* map 0x10000000-0x7fffffff to the same */
      PMMU_SF_PAGE(0x20000000),     /*   physical addresses, allow caching   */
      PMMU_SF_PAGE(0x30000000),
      PMMU_SF_PAGE(0x40000000),
      PMMU_SF_PAGE(0x50000000),
      PMMU_SF_PAGE(0x60000000),
      PMMU_SF_PAGE(0x70000000),
      PMMU_SF_PAGE_CI(0x80000000),  /* map 0x80000000-0xefffffff to the same */
      PMMU_SF_PAGE_CI(0x90000000),  /*   physical addresses, DON'T allow caching */
      PMMU_SF_PAGE_CI(0xa0000000),
      PMMU_SF_PAGE_CI(0xb0000000),
      PMMU_SF_PAGE_CI(0xc0000000),
      PMMU_SF_PAGE_CI(0xd0000000),
      PMMU_SF_PAGE_CI(0xe0000000),
      PMMU_SF_TABLE(tib2)           /* for 0xf0000000-0xffffffff, use table tib2 */
    },
    /* tib1: second level table (for 0x00000000-0x0fffffff) */
    { PMMU_SF_TABLE(tic),           /* for 0x00000000-0x00ffffff, use table tic */
      PMMU_SF_PAGE(0x01000000),     /* map 0x01000000-0x0fffffff to the same */
      PMMU_SF_PAGE(0x02000000),     /*   physical addresses, allow caching   */
      PMMU_SF_PAGE(0x03000000),
      PMMU_SF_PAGE(0x04000000),
      PMMU_SF_PAGE(0x05000000),
      PMMU_SF_PAGE(0x06000000),
      PMMU_SF_PAGE(0x07000000),
      PMMU_SF_PAGE(0x08000000),
      PMMU_SF_PAGE(0x09000000),
      PMMU_SF_PAGE(0x0a000000),
      PMMU_SF_PAGE(0x0b000000),
      PMMU_SF_PAGE(0x0c000000),
      PMMU_SF_PAGE(0x0d000000),
      PMMU_SF_PAGE(0x0e000000),
      PMMU_SF_PAGE(0x0f000000)
    },
    /* tib2: second-level table (for 0xf0000000-0xffffffff) */
    { PMMU_SF_PAGE_CI(0xf0000000),  /* map 0xf0000000-0xfeffffff to the same */
      PMMU_SF_PAGE_CI(0xf1000000),  /*   physical addresses, DON'T allow caching */
      PMMU_SF_PAGE_CI(0xf2000000),
      PMMU_SF_PAGE_CI(0xf3000000),
      PMMU_SF_PAGE_CI(0xf4000000),
      PMMU_SF_PAGE_CI(0xf5000000),
      PMMU_SF_PAGE_CI(0xf6000000),
      PMMU_SF_PAGE_CI(0xf7000000),
      PMMU_SF_PAGE_CI(0xf8000000),
      PMMU_SF_PAGE_CI(0xf9000000),
      PMMU_SF_PAGE_CI(0xfa000000),
      PMMU_SF_PAGE_CI(0xfb000000),
      PMMU_SF_PAGE_CI(0xfc000000),
      PMMU_SF_PAGE_CI(0xfd000000),
      PMMU_SF_PAGE_CI(0xfe000000),
      PMMU_SF_TABLE(tic)            /* for 0xf0000000-0xffffffff, use table tic */
    },
    /* tic: third-level table for standard Falcon addresses
     * 
     * note that this table maps both 0x00000000-0x00ffffff and 0xff000000-0xffffffff
     * to the same physical address range (0x00000000-0x00ffffff)
     */
    { PMMU_SF_PAGE(0x00000000),     /* map 0x??000000-0x??efffff (// = 00 or ff) to */
      PMMU_SF_PAGE(0x00100000),     /*   0x00000000-0x00efffff, allow caching       */
      PMMU_SF_PAGE(0x00200000),
      PMMU_SF_PAGE(0x00300000),
      PMMU_SF_PAGE(0x00400000),
      PMMU_SF_PAGE(0x00500000),
      PMMU_SF_PAGE(0x00600000),
      PMMU_SF_PAGE(0x00700000),
      PMMU_SF_PAGE(0x00800000),
      PMMU_SF_PAGE(0x00900000),
      PMMU_SF_PAGE(0x00a00000),
      PMMU_SF_PAGE(0x00b00000),
      PMMU_SF_PAGE(0x00c00000),
      PMMU_SF_PAGE(0x00d00000),
      PMMU_SF_PAGE(0x00e00000),
      PMMU_SF_PAGE_CI(0x00f00000)   /* map 0x??f00000-0x??ffffff (?? = 00 or ff) to */
                                    /*   0x00f00000-0x00ffffff, DON'T allow caching */
    }
};

void setup_68030_pmmu(void)
{
    memcpy(&mmutable_ram, &mmutable_rom, sizeof mmutable_rom);
}
#endif /* CONF_WITH_68030_PMMU */
