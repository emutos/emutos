/*
 * aros.c - Amiga functions from AROS
 *
 * Copyright (c) 1995-2013, The AROS Development Team. All rights reserved.
 *
 * The functions in this file were borrowed from the AROS sources.
 * Glued to EmuTOS by:
 *  VRI   Vincent Rivière
 *
 * This file is covered by the AROS PUBLIC LICENSE (APL) Version 1.1
 * See doc/license_aros.txt for details.
 */

#define DBG_AROS 0

#include "config.h"
#include "portab.h"

#if defined(MACHINE_AMIGA) && CONF_WITH_AROS

#include "aros.h"
#include "amiga.h"
#include "string.h"
#include "processor.h"
#include "gemerror.h"
#include "kprint.h"

/* The following functions are defined in aros2.S */
ULONG MemoryTest(void *address, void *endAddress, ULONG blockSize);
ULONG ReadGayle(void);

/******************************************************************************/
/* Compatibility macros and types                                             */
/******************************************************************************/

#if DBG_AROS
#define D(x) do { x; } while (0)
#else
#define D(x) do { } while (0)
#endif

#define bug kprintf
#define __mayalias MAY_ALIAS

#define AROS_UFCA(atype, aval, reg) \
    ((atype)(aval))

#define AROS_UFC3(ftype, fname, a1, a2, a3) \
    fname(a1, a2, a3)

#define AROS_UFC5(ftype, fname, a1, a2, a3, a4, a5) \
    fname(a1, a2, a3, a4, a5)

#define CopyMem(source, dest, size) memcpy(dest, source, size)

/* From compiler/include/aros/cpu.h *******************************************/

#define AROS_32BIT_TYPE     long

/* From compiler/include/exec/types.h *****************************************/

typedef void * APTR; /* memory pointer */
typedef unsigned long IPTR;
typedef unsigned char UBYTE; /* unsigned 8-bit value */

// STRPTR is char* in C++, but should be UBYTE* in C
typedef char * STRPTR; /* Pointer to string (NULL terminated) */

/* From compiler/include/exec/execbase.h **************************************/

// Minimal ExecBase
struct ExecBase
{
    UWORD AttnFlags; /* Attention Flags */
};

static struct ExecBase g_ExecBase;
static struct ExecBase* const SysBase = &g_ExecBase;

#define AFF_68020   (1L<<1)
#define AFF_ADDR32  (1L<<14)

/******************************************************************************/
/* Machine detection                                                          */
/******************************************************************************/

void aros_machine_detect(void)
{
    /* Do we have at least a 68020? */
    if (mcpu >= 20)
        SysBase->AttnFlags |= AFF_68020;

    /* Do we have a 32-bit address bus? */
    if (MemoryTest((APTR)0x08000000, (APTR)0x08000000, 0x00100000) == 0)
        SysBase->AttnFlags |= AFF_ADDR32;

    /* Do we have Gayle? */
    has_gayle = ReadGayle() > 0;
}

#if CONF_WITH_ALT_RAM

/******************************************************************************/
/* Alternate RAM                                                              */
/******************************************************************************/

/* From compiler/include/libraries/configregs.h *******************************/

#define E_SLOTSIZE      0x10000
#define E_SLOTMASK      0xFFFF
#define E_SLOTSHIFT     16

#define E_EXPANSIONBASE     0x00e80000
#define E_EXPANSIONSIZE     0x00080000
#define E_EXPANSIONSLOTS    8

#define E_MEMORYBASE        0x00200000
#define E_MEMORYSIZE        0x00800000
#define E_MEMORYSLOTS       128

#define EZ3_EXPANSIONBASE   0xFF000000
#define EZ3_CONFIGAREA      0x40000000
#define EZ3_CONFIGAREAEND   0x7FFFFFFF
#define EZ3_SIZEGRANULARITY 0x00080000

/* er_Type */
#define ERT_TYPEMASK        0xc0
#define ERT_TYPEBIT         6
#define ERT_TYPESIZE        2
#define ERT_NEWBOARD        0xc0
#define ERT_ZORROII         ERT_NEWBOARD
#define ERT_ZORROIII        0x80

#define ERTB_MEMLIST        5
#define ERTF_MEMLIST        (1L<<5)
#define ERTB_DIAGVALID      4
#define ERTF_DIAGVALID      (1L<<4)
#define ERTB_CHAINEDCONFIG  3
#define ERTF_CHAINEDCONFIG  (1L<<3)

#define ERT_MEMMASK         0x7
#define ERT_MEMBIT          0
#define ERT_MEMSIZE         3

/* er_Flags */
#define ERFB_MEMSPACE       7
#define ERFF_MEMSPACE       (1L<<7)
#define ERFB_NOSHUTUP       6
#define ERFF_NOSHUTUP       (1L<<6)
#define ERFB_EXTENDED       5
#define ERFF_EXTENDED       (1L<<5)
#define ERFB_ZORRO_III      4
#define ERFF_ZORRO_III      (1L<<4)

#define ERT_Z3_SSMASK       0x0F
#define ERT_Z3_SSBIT        0
#define ERT_Z3_SSSIZE       4

/* From compiler/include/exec/nodes.h *****************************************/

struct Node __mayalias;
struct Node
{
    struct Node * ln_Succ,
                * ln_Pred;
    UBYTE       ln_Type;
    BYTE        ln_Pri;
    char        * ln_Name;
};

struct MinNode __mayalias;
struct MinNode
{
    struct MinNode * mln_Succ,
                   * mln_Pred;
};

/* From compiler/include/exec/lists.h *****************************************/

/* Normal list */
struct List __mayalias;
struct List
{
    struct Node * lh_Head,
                * lh_Tail;
    union
    {
        struct Node * lh_TailPred;
        struct List * lh_TailPred_;
    };
    UBYTE     lh_Type;
    UBYTE     l_pad;
};

/* Minimal list */
struct MinList __mayalias;
struct MinList
{
    struct MinNode * mlh_Head,
                   * mlh_Tail;
    union
    {
        struct MinNode * mlh_TailPred;
        struct MinList * mlh_TailPred_;
    };
};

#define NEWLIST(_l)                                     \
do                                                      \
{                                                       \
    struct List *__aros_list_tmp = (struct List *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
    l->lh_TailPred_= l;                                 \
    l->lh_Tail     = 0;                                 \
    l->lh_Head     = (struct Node *)&l->lh_Tail;        \
} while (0)

/* From rom/exec/addtail.c ****************************************************/

static void AddTail(struct List *list, struct Node *node)
{
    /*
    Make the node point to the head of the list. Our predecessor is the
    previous last node of the list.
    */
    node->ln_Succ          = (struct Node *)&list->lh_Tail;
    node->ln_Pred          = list->lh_TailPred;

    /*
    Now we are the last now. Make the old last node point to us
    and the pointer to the last node, too.
    */
    list->lh_TailPred->ln_Succ = node;
    list->lh_TailPred          = node;
}

/* From compiler/include/exec/lists.h *****************************************/

#define ForeachNode(list, node)                        \
for                                                    \
(                                                      \
    node = (void *)(((struct List *)(list))->lh_Head); \
    ((struct Node *)(node))->ln_Succ;                  \
    node = (void *)(((struct Node *)(node))->ln_Succ)  \
)

/* From compiler/include/libraries/configregs.h *******************************/

struct ExpansionRom
{
    UBYTE   er_Type;
    UBYTE   er_Product;
    UBYTE   er_Flags;
    UBYTE   er_Reserved03;
    UWORD   er_Manufacturer;
    ULONG   er_SerialNumber;
    UWORD   er_InitDiagVec;
    union {
        struct {
            UBYTE   c;
            UBYTE   d;
            UBYTE   e;
            UBYTE   f;
        } Reserved0;
        struct DiagArea *DiagArea;
    } er_;
};

/* From compiler/include/libraries/configvars.h *******************************/

struct ConfigDev
{
    struct Node         cd_Node;
    UBYTE               cd_Flags;       /* read/write device flags */
    UBYTE               cd_Pad;
    struct ExpansionRom cd_Rom;         /* copy of boards expansion ROM */
    APTR                cd_BoardAddr;   /* physical address of exp. board */
    ULONG               cd_BoardSize;   /* size in bytes of exp. board */
    UWORD               cd_SlotAddr;    /* private */
    UWORD               cd_SlotSize;    /* private */
    APTR                cd_Driver;      /* pointer to node of driver */
    struct ConfigDev   *cd_NextCD;      /* linked list of devices to configure */
    ULONG               cd_Unused[4];   /* for the drivers use - private */
};

/* Flags definitions for cd_Flags */
#define CDB_SHUTUP      0       /* this board has been shut up */
#define CDF_SHUTUP      0x01
#define CDB_CONFIGME    1       /* board needs a driver to claim it */
#define CDF_CONFIGME    0x02
#define CDB_BADMEMORY   2       /* board contains bad memory */
#define CDF_BADMEMORY   0x04
#define CDB_PROCESSED   3       /* private */
#define CDF_PROCESSED   0x08

/* EmuTOS fake Expansion Library **********************************************/

// Dumb ConfigDev allocation from static array
#define MAX_CONFIG_DEVS 5
static struct ConfigDev g_configDevs[MAX_CONFIG_DEVS];
static int g_nUsedConfigDevs = 0;

static struct ConfigDev *AllocConfigDev(void)
{
    if (g_nUsedConfigDevs >= MAX_CONFIG_DEVS)
        return NULL;

    return &g_configDevs[g_nUsedConfigDevs++];
}

#define FreeConfigDev(x)

// struct IntExpansionBase will actually be struct ExpansionBase
#define IntExpansionBase ExpansionBase
#define eb_BoardList BoardList

/* From rom/expansion/expansion_intern.h **************************************/

#define Z2SLOTS         240
#define SLOTSPERBYTE    8

/* I got this info from the 1.3 include file libraries/expansionbase.h */

struct IntExpansionBase
{
    //struct Library          eb_LibNode;
    //UBYTE                   eb_Flags;
    //UBYTE                   eb_pad;
    //struct ExecBase        *eb_SysBase;
    //IPTR                    eb_SegList;
    //struct CurrentBinding   eb_CurrentBinding;
    struct List             eb_BoardList;
    //struct List             eb_MountList;

    //struct SignalSemaphore  eb_BindSemaphore;

    UBYTE                   eb_z2Slots[Z2SLOTS/SLOTSPERBYTE];
    UWORD                   eb_z3Slot;
};

#define IntExpBase(eb)  ((struct IntExpansionBase*)(eb))

// EmuTOS fake Expansion Library
static struct IntExpansionBase g_eb;
static struct IntExpansionBase* const ExpansionBase = &g_eb;

/* From arch/m68k-amiga/expansion/readexpansionbyte.c *************************/

static UBYTE ReadExpansionByte(APTR board, ULONG offset)
{
    UBYTE v;
    UWORD loffset;
    volatile UBYTE *p = (UBYTE*)board;

    offset *= 4;
    if (((ULONG)board) & 0xff000000)
        loffset = 0x100;
    else
        loffset = 0x002;
    v = (p[offset + loffset] & 0xf0) >> 4;
    v |= p[offset] & 0xf0;
    return v;
}

/* From arch/m68k-amiga/expansion/writeexpansionbyte.c ************************/

static void WriteExpansionByte(APTR board, ULONG offset, ULONG byte)
{
    UWORD loffset;
    volatile UBYTE *p = (UBYTE*)board;

    offset *= 4;
    if (((ULONG)board) & 0xff000000)
        loffset = 0x100;
    else
        loffset = 0x002;
    p[offset + loffset] = byte << 4;
    p[offset] = byte;
}

/* From arch/m68k-amiga/expansion/readexpansionrom.c **************************/

static void readexprom(APTR board, struct ExpansionRom *rom, struct ExpansionBase *ExpansionBase)
{
    WORD cnt;

    for (cnt = 0; cnt < sizeof(struct ExpansionRom); cnt++)
    ((UBYTE*)rom)[cnt] = ~ReadExpansionByte(board, cnt);
    /* AOS expansion.library appears to have off-by-one bug.. */
    ReadExpansionByte(board, cnt);
    rom->er_Type = ~rom->er_Type;
}

static BOOL ReadExpansionRom(APTR board, struct ConfigDev *configDev)
{
    struct ExpansionRom *rom = &configDev->cd_Rom;
    struct ExpansionRom tmprom;
    ULONG size;
    UBYTE cnt;

    readexprom(board, rom, ExpansionBase);

    if (rom->er_Reserved03 != 0)
        return FALSE;

    if (rom->er_Manufacturer == 0 || rom->er_Manufacturer == 0xffff)
        return FALSE;

    if ((rom->er_Type & ERT_TYPEMASK) != ERT_ZORROII && (rom->er_Type & ERT_TYPEMASK) != ERT_ZORROIII)
        return FALSE;

    /* AOS expansion.library wants to be really really sure... */
    for (cnt = 0; cnt < 11; cnt++) {
        readexprom(board, &tmprom, ExpansionBase);
        if (memcmp(&tmprom, rom, sizeof(struct ExpansionRom)))
            return FALSE;
    }

    if ((rom->er_Type & ERT_TYPEMASK) == ERT_ZORROIII && (rom->er_Flags & ERFF_EXTENDED)) {
        size = (16L * 1024 * 1024) << (rom->er_Type & ERT_MEMMASK);
    } else {
        UBYTE mem = rom->er_Type & ERT_MEMMASK;
        if (mem == 0)
            size = 8L * 1024 * 1024; // 8M
        else
            size = (32L * 1024) << mem; // 64k,128k,256k,512k,1m,2m,4m
    }
    if ((rom->er_Type & ERT_TYPEMASK) == ERT_ZORROIII) {
        UBYTE subsize = rom->er_Flags & ERT_Z3_SSMASK;
        ULONG ss = size;
        if (subsize >= 2 && subsize <= 7) { // 64k,128k,256k,512k,1m,2m
            ss = (64L * 1024) << (subsize - 2);
        } else if (subsize >= 8) { // 4m,6m,8m,10m,12m,14m
            ss = (4L + (subsize - 8) * 2) * 1024 * 1024;
        }
        if (size > ss)
            size = ss;
    }

    configDev->cd_BoardSize = size;
    configDev->cd_BoardAddr = board;

    D(bug("Found board %p: mfg=%d prod=%d size=%08lx serial=%08lx diag=%04x\n",
        board, rom->er_Manufacturer, rom->er_Product, size, rom->er_SerialNumber, rom->er_InitDiagVec));

    return TRUE;
}

/* From arch/m68k-amiga/expansion/configboard.c *******************************/

#define Z3SLOT 0x01000000

/* do not touch. Ugly hack. UAE direct JIT versions need this */
/* check UAE expansion.c for ugly details */
static void writeexpansion(APTR board, APTR configdev, UBYTE type, UWORD startaddr, struct ExpansionBase * ExpansionBase)
{
    if (type == ERT_ZORROII) {
        WriteExpansionByte(board, 18, startaddr);
    } else {
        //WriteExpansionWord(board, 17, startaddr);

        /* Here is the ugly hack.
         * a3 must point to configdev during WriteExpansionWord() */
        __asm__ volatile (
            "move.l %0,a3\n\t"
            "move.b %2,72(%1)\n\t"
            "move.w %2,68(%1)"
        : /* outputs */
        : "g"(configdev), "a"(board), "d"(startaddr) /* inputs */
        : "a3" AND_MEMORY /* clobbered */
        );
    }
}

static BOOL ConfigBoard(APTR board, struct ConfigDev * configDev)
{
    UBYTE type = configDev->cd_Rom.er_Type & ERT_TYPEMASK;
    BOOL memorydevice;
    ULONG size = configDev->cd_BoardSize;

    D(bug("Configuring board: cd=%p mfg=%d prod=%d size=%08lx type=%02x\n",
        configDev, configDev->cd_Rom.er_Manufacturer, configDev->cd_Rom.er_Product, size, configDev->cd_Rom.er_Type));

    memorydevice = (configDev->cd_Rom.er_Type & ERTF_MEMLIST) != 0;
    if (type == ERT_ZORROIII) {
        UWORD prevslot, newslot;
        UWORD endslot = 255;
        UWORD slotsize = (size + 0x00ffffff) / Z3SLOT;
        if (IntExpBase(ExpansionBase)->eb_z3Slot == 0)
            IntExpBase(ExpansionBase)->eb_z3Slot = 0x40000000 / Z3SLOT;
        prevslot = IntExpBase(ExpansionBase)->eb_z3Slot;
        // handle alignment
        newslot = (prevslot + slotsize - 1) & ~(slotsize - 1);
        D(bug("size=%d prev=%d new=%d end=%d\n", slotsize, prevslot, newslot, endslot));
        if (newslot + slotsize <= endslot) {
            ULONG startaddr = newslot * Z3SLOT;
            configDev->cd_BoardAddr = (APTR)startaddr;
            configDev->cd_SlotAddr = IntExpBase(ExpansionBase)->eb_z3Slot;
            configDev->cd_SlotSize = slotsize;
            configDev->cd_Flags |= CDF_CONFIGME;
            IntExpBase(ExpansionBase)->eb_z3Slot = newslot + slotsize;
            AROS_UFC5(void, writeexpansion,
                AROS_UFCA(APTR,  board, A0),
                AROS_UFCA(APTR,  configDev, A3),
                AROS_UFCA(UBYTE, type, D0),
                        AROS_UFCA(UWORD, (startaddr >> 16), D1),
                            AROS_UFCA(struct ExpansionBase*, ExpansionBase, A6)
                    );
                    // Warning: UAE may change configDev->cd_BoardAddr in writeexpansion()
                    D(bug("-> configured, %p - %p\n", (void*)configDev->cd_BoardAddr, (void*)(((ULONG)configDev->cd_BoardAddr) + configDev->cd_BoardSize - 1)));
            return TRUE;
        }
    } else {
        ULONG start, end, addr, step;
        UBYTE *space;
        UBYTE area;

        for (area = 0; area < 2; area++) {

            if (area == 0 && (size >= 8 * E_SLOTSIZE || memorydevice))
                continue;

            if (area == 0) {
                start = 0x00E90000;
                end   = 0x00EFFFFF;
            } else {
                start = 0x00200000;
                end   = 0x009FFFFF;
            }
            space = IntExpBase(ExpansionBase)->eb_z2Slots;
            step = 0x00010000;
            for (addr = start; addr < end; addr += step) {
                ULONG startaddr = addr;
                UWORD offset = startaddr / (E_SLOTSIZE * SLOTSPERBYTE);
                BYTE bit = 7 - ((startaddr / E_SLOTSIZE) % SLOTSPERBYTE);
                UBYTE res = space[offset];
                ULONG sizeleft = size;

                if (res & (1 << bit))
                    continue;

                if (size < 4L * 1024 * 1024) {
                    // handle alignment, 128k boards must be 128k aligned and so on..
                    if ((startaddr & (size - 1)) != 0)
                        continue;
                } else {
                    // 4M and 8M boards have different alignment requirements
                    if (startaddr != 0x00200000 && startaddr != 0x00600000)
                        continue;
                }

                // found free start address
                if (size >= E_SLOTSIZE * SLOTSPERBYTE) {
                    // needs at least 1 byte and is always aligned to byte
                    while (space[offset] == 0 && sizeleft >= E_SLOTSIZE && offset <= end / (E_SLOTSIZE * SLOTSPERBYTE)) {
                        offset++;
                        sizeleft -= E_SLOTSIZE * SLOTSPERBYTE;
                    }
                } else {
                    // bit by bit small board check (fits in one byte)
                    while ((res & (1 << bit)) == 0 && sizeleft >= E_SLOTSIZE && bit >= 0) {
                        sizeleft -= E_SLOTSIZE;
                        bit--;
                    }
                }

                if (sizeleft >= E_SLOTSIZE)
                    continue;

                configDev->cd_BoardAddr = (APTR)startaddr;
                configDev->cd_Flags |= CDF_CONFIGME;
                configDev->cd_SlotAddr = (startaddr >> 16);
                configDev->cd_SlotSize = size >> 16;
                AROS_UFC5(void, writeexpansion,
                    AROS_UFCA(APTR,  board, A0),
                    AROS_UFCA(APTR,  configDev, A3),
                    AROS_UFCA(UBYTE, type, D0),
                            AROS_UFCA(UWORD, (startaddr >> 16), D1),
                                AROS_UFCA(struct ExpansionBase*, ExpansionBase, A6)
                        );
                        D(bug("-> configured, %p - %p\n", (void*)startaddr, (void*)(startaddr + configDev->cd_BoardSize - 1)));

                // do not remove this, configDev->cd_BoardAddr
                // might have changed inside writeexpansion
                startaddr = (ULONG)configDev->cd_BoardAddr;
                offset = startaddr / (E_SLOTSIZE * SLOTSPERBYTE);
                bit = 7 - ((startaddr / E_SLOTSIZE) % SLOTSPERBYTE);
                sizeleft = size;
                // now allocate area we reserved
                while (sizeleft >= E_SLOTSIZE) {
                    space[offset] |= 1 << bit;
                    sizeleft -= E_SLOTSIZE;
                    bit--;
                }

                return TRUE;
            }
        }
    }

    D(bug("Configuration failed!\n"));
    if (!(configDev->cd_Flags & ERFF_NOSHUTUP)) {
        configDev->cd_Flags |= CDF_SHUTUP;
        WriteExpansionByte(board, 19, 0); // SHUT-UP!
    } else {
        // uh?
    }
    return FALSE;
}

/* From rom/expansion/addconfigdev.c  *****************************************/

#define ObtainConfigBinding()
#define ReleaseConfigBinding()

static void AddConfigDev(struct ConfigDev *configDev)
{
    if(configDev)
    {
        ObtainConfigBinding();
        AddTail(&IntExpBase(ExpansionBase)->eb_BoardList, (struct Node *)configDev);
        ReleaseConfigBinding();
    }
}

/* From compiler/include/exec/memory.h ****************************************/

/* Memory allocation flags */
#define MEMF_ANY           0L
#define MEMF_PUBLIC        (1L<<0)
#define MEMF_CHIP          (1L<<1)
#define MEMF_FAST          (1L<<2)
#define MEMF_EXECUTABLE    (1L<<4)  /* AmigaOS v4 compatible */
#define MEMF_LOCAL         (1L<<8)
#define MEMF_24BITDMA      (1L<<9)
#define MEMF_KICK          (1L<<10)
#define MEMF_31BIT         (1L<<12) /* Low address space (<2GB). Effective only on 64 bit machines. */
#define MEMF_CLEAR         (1L<<16) /* Explicitly clear memory after allocation */
#define MEMF_LARGEST       (1L<<17)
#define MEMF_REVERSE       (1L<<18)
#define MEMF_TOTAL         (1L<<19)
#define MEMF_HWALIGNED     (1L<<20) /* For AllocMem() - align address and size to physical page boundary */
#define MEMF_SEM_PROTECTED (1L<<20) /* For CreatePool() - add semaphore protection to the pool */
#define MEMF_NO_EXPUNGE    (1L<<31)

/* From rom/exec/addmemlist.c *************************************************/

extern long xmaddalt(UBYTE *start, long size); /* found in bdos/mem.h */

static void AddMemList(ULONG size, ULONG attributes, LONG pri, APTR base, STRPTR name)
{
    // EmuTOS glue
    xmaddalt((UBYTE *)base, (long)size);
}

/* From arch/m68k-amiga/expansion/configchain.c *******************************/

static ULONG autosize(struct ExpansionBase *ExpansionBase, struct ConfigDev *configDev)
{
    UBYTE sizebits = configDev->cd_Rom.er_Flags & ERT_Z3_SSMASK;
    ULONG maxsize = configDev->cd_BoardSize;
    ULONG size = 0;
    UBYTE *addr = (UBYTE*)configDev->cd_BoardAddr;

    D(bug("sizebits=%x\n", sizebits));
    if (sizebits >= 14) /* 14 and 15 = reserved */
        return 0;
    if (sizebits >= 2 && sizebits <= 8)
        return 0x00010000 << (sizebits - 2);
    if (sizebits >= 9)
        return 0x00600000 + (0x200000 * (sizebits - 9));
    size = AROS_UFC3(ULONG, MemoryTest,
        AROS_UFCA(APTR, addr, A0),
        AROS_UFCA(APTR, addr + maxsize, A1),
        AROS_UFCA(ULONG, 0x80000, D0));
    D(bug("addr=%lx size=%lx maxsize=%lx\n", (ULONG)addr, size, maxsize));
    return size;
}

static void findmbram(struct ExpansionBase *ExpansionBase)
{
    LONG ret;
    ULONG step, start, end;

    if (!(SysBase->AttnFlags & AFF_68020))
        return;
    if ((SysBase->AttnFlags & AFF_68020) && !(SysBase->AttnFlags & AFF_ADDR32))
        return;

    /* High MBRAM */
    step =  0x00100000;
    start = 0x08000000;
    end =   0x7f000000;
    ret = AROS_UFC3(LONG, MemoryTest,
        AROS_UFCA(APTR, start, A0),
        AROS_UFCA(APTR, end, A1),
        AROS_UFCA(ULONG, step, D0));
    if (ret < 0)
        return;
    if (ret > 0) {
        AddMemList(ret, MEMF_KICK | MEMF_LOCAL | MEMF_FAST | MEMF_PUBLIC, 40, (APTR)start, "expansion.memory");
        D(bug("MBRAM @%08lx, size %08lx\n", start, ret));
    }

    /* Low MBRAM, reversed detection needed */
    step =  0x00100000;
    start = 0x08000000;
    end =   0x01000000;
    for (;;) {
        ret = AROS_UFC3(LONG, MemoryTest,
            AROS_UFCA(APTR, start - step, A0),
            AROS_UFCA(APTR, start, A1),
            AROS_UFCA(ULONG, step, D0));
        if (ret <= 0)
            break;
        if (end >= start - step)
            break;
        start -= step;
    }
    if (start != 0x08000000) {
        ULONG size = 0x08000000 - start;
        AddMemList(size, MEMF_KICK | MEMF_LOCAL | MEMF_FAST | MEMF_PUBLIC, 30, (APTR)start, "expansion.memory");
        D(bug("MBRAM @%08lx, size %08lx\n", start, size));
    }
}

static void allocram(struct ExpansionBase *ExpansionBase)
{
    struct Node *node;

    findmbram(ExpansionBase);
    // we should merge address spaces, later..
    D(bug("adding ram boards\n"));
    ForeachNode(&ExpansionBase->BoardList, node) {
        struct ConfigDev *configDev = (struct ConfigDev*)node;
        if ((configDev->cd_Rom.er_Type & ERTF_MEMLIST) && !(configDev->cd_Flags & CDF_SHUTUP) && !(configDev->cd_Flags & CDF_PROCESSED)) {
            ULONG attr = MEMF_PUBLIC | MEMF_FAST | MEMF_KICK;
            ULONG size = configDev->cd_BoardSize;
            APTR addr = configDev->cd_BoardAddr;
            LONG pri = 20;
            if (configDev->cd_BoardAddr <= (APTR)0x00FFFFFF) {
                attr |= MEMF_24BITDMA;
                pri = 0;
            } else if ((configDev->cd_Rom.er_Flags & ERT_Z3_SSMASK) != 0) {
                size = autosize(ExpansionBase, configDev);
            }
            if (size && size <= configDev->cd_BoardSize) {
                D(bug("ram board at %08lx, size %08lx attr %08lx\n", (ULONG)addr, size, attr));
                AddMemList(size, attr, pri, addr, "Fast Memory");
            }
            configDev->cd_Flags |= CDF_PROCESSED;
        }
    }
    D(bug("ram boards done\n"));
}

static void ConfigChain(APTR baseAddr)
{
    struct ConfigDev *configDev = NULL;

    /* Try to guess if we have Z3 based machine.
     * Not all Z3 boards appear in Z2 region.
     *
     * Ignores original baseAddr by design.
     */
    BOOL maybeZ3 = (SysBase->AttnFlags & AFF_ADDR32);
    D(bug("configchain\n"));
    for(;;) {
        BOOL gotrom = FALSE;
        if (!configDev)
            configDev = AllocConfigDev();
        if (!configDev)
            break;
        if (maybeZ3) {
            baseAddr = (APTR)EZ3_EXPANSIONBASE;
            gotrom = ReadExpansionRom(baseAddr, configDev);
        }
        if (!gotrom) {
            baseAddr = (APTR)E_EXPANSIONBASE;
            gotrom = ReadExpansionRom(baseAddr, configDev);
        }
        if (!gotrom) {
            FreeConfigDev(configDev);
            break;
        }
        if (ConfigBoard(baseAddr, configDev)) {
            AddConfigDev(configDev);
            configDev = NULL;
        }
    }
    D(bug("configchain done\n"));

    allocram(ExpansionBase);
}

/* From rom/expansion/expansion_init.c ****************************************/

static void ExpansionInit(struct ExpansionBase *ExpansionBase)
{
    D(bug("expansion init\n"));

    NEWLIST(&ExpansionBase->eb_BoardList);

    /* See what expansion hardware we can detect. */
    ConfigChain((APTR)E_EXPANSIONBASE);
}

/* EmuTOS to AROS glue for Alternate RAM detection ****************************/

static void add_slow_ram(void)
{
    APTR address = (APTR)0x00c00000;
    APTR endAddress = has_gayle ? (APTR)0x00d80000 : (APTR)0x00dc0000;
    ULONG size = MemoryTest(address, endAddress, 0x00040000);

    if (size > 0)
    {
        D(bug("Slow RAM detected at %08lx, size %08lx\n", (ULONG)address, size));
        xmaddalt((UBYTE *)address, (long)size);
    }
}

void aros_add_alt_ram(void)
{
    /* Add the slowest RAM first to put it at the end of the Alt RAM pool */
    add_slow_ram();

    /* Configure Zorro II / Zorro III boards and find the Alt RAM */
    ExpansionInit(ExpansionBase);
}

#endif /* CONF_WITH_ALT_RAM */

/******************************************************************************/
/* Floppy                                                                     */
/******************************************************************************/

/* From compiler/include/exec/errors.h ****************************************/

#define IOERR_OPENFAIL   (-1)
#define IOERR_ABORTED    (-2)
#define IOERR_NOCMD      (-3)
#define IOERR_BADLENGTH  (-4)
#define IOERR_BADADDRESS (-5)
#define IOERR_UNITBUSY   (-6)
#define IOERR_SELFTEST   (-7)

static void *AllocMem(ULONG byteSize, ULONG attributes)
{
    D(bug("AllocMem()\n"));

    // Not implemented
    return NULL;
}

static void FreeMem(void *memoryBlock, ULONG byteSize)
{
    D(bug("FreeMem()\n"));

    // Not implemented
}

/* From compiler/include/exec/io.h ********************************************/

struct IOStdReq
{
    //struct Message  io_Message;
    struct Device * io_Device;
    struct Unit   * io_Unit;
    UWORD           io_Command;
    UBYTE           io_Flags;
    BYTE            io_Error;
/* fields that are different from IORequest */
    ULONG           io_Actual;
    ULONG           io_Length;
    APTR            io_Data;
    ULONG           io_Offset;
};

#define CMD_INVALID 0
#define CMD_RESET   1
#define CMD_READ    2
#define CMD_WRITE   3
#define CMD_UPDATE  4
#define CMD_CLEAR   5
#define CMD_STOP    6
#define CMD_START   7
#define CMD_FLUSH   8
#define CMD_NONSTD  9

#define IOB_QUICK     0
#define IOF_QUICK (1<<0)

/* From compiler/include/devices/trackdisk.h **********************************/

struct IOExtTD
{
    struct IOStdReq iotd_Req;
    ULONG           iotd_Count;
    ULONG           iotd_SecLabel;
};

struct TDU_PublicUnit
{
    //struct Unit tdu_Unit;

    UWORD tdu_Comp01Track;
    UWORD tdu_Comp10Track;
    UWORD tdu_Comp11Track;
    ULONG tdu_StepDelay;
    ULONG tdu_SettleDelay;
    UBYTE tdu_RetryCnt;
    UBYTE tdu_PubFlags;       /* see below */
    UWORD tdu_CurrTrk;
    ULONG tdu_CalibrateDelay;
    ULONG tdu_Counter;
};

/* tdu_PubFlags */
#define TDPB_NOCLICK      0
#define TDPF_NOCLICK (1L<<0)

/* From compiler/include/hardware/custom.h ************************************/

struct Custom
{
    UWORD bltddat;
    UWORD dmaconr;
    UWORD vposr;
    UWORD vhposr;
    UWORD dskdatr;
    UWORD joy0dat;
    UWORD joy1dat;
    UWORD clxdat;
    UWORD adkconr;
    UWORD pot0dat;
    UWORD pot1dat;
    UWORD potinp;
    UWORD serdatr;
    UWORD dskbytr;
    UWORD intenar;
    UWORD intreqr;
    APTR  dskpt;
    UWORD dsklen;
    UWORD dskdat;
    UWORD refptr;
    UWORD vposw;
    UWORD vhposw;
    UWORD copcon;
    UWORD serdat;
    UWORD serper;
    UWORD potgo;
    UWORD joytest;
    UWORD strequ;
    UWORD strvbl;
    UWORD strhor;
    UWORD strlong;

    UWORD bltcon0;
    UWORD bltcon1;
    UWORD bltafwm;
    UWORD bltalwm;
    APTR  bltcpt;
    APTR  bltbpt;
    APTR  bltapt;
    APTR  bltdpt;
    UWORD bltsize;
    UBYTE pad2d;
    UBYTE bltcon0l; /* WRITE-ONLY */
    UWORD bltsizv;
    UWORD bltsizh;
    UWORD bltcmod;
    UWORD bltbmod;
    UWORD bltamod;
    UWORD bltdmod;
    UWORD pad34[4];
    UWORD bltcdat;
    UWORD bltbdat;
    UWORD bltadat;

    UWORD pad3b[3];
    UWORD deniseid;
    UWORD dsksync;
    ULONG cop1lc;
    ULONG cop2lc;
    UWORD copjmp1;
    UWORD copjmp2;
    UWORD copins;
    UWORD diwstrt;
    UWORD diwstop;
    UWORD ddfstrt;
    UWORD ddfstop;
    UWORD dmacon;
    UWORD clxcon;
    UWORD intena;
    UWORD intreq;
    UWORD adkcon;

    /* chip audio channel */
    struct AudChannel
    {
        UWORD * ac_ptr;    /* waveform data */
        UWORD   ac_len;    /* waveform length (in words) */
        UWORD   ac_per;    /* sample period */
        UWORD   ac_vol;    /* volume */
        UWORD   ac_dat;
        UWORD   ac_pad[2];
    } aud[4];

    APTR  bplpt[8];
    UWORD bplcon0;
    UWORD bplcon1;
    UWORD bplcon2;
    UWORD bplcon3;
    UWORD bpl1mod;
    UWORD bpl2mod;
    UWORD bplcon4;
    UWORD clxcon2;
    UWORD bpldat[8];
    APTR  sprpt[8];

    struct SpriteDef
    {
        UWORD pos;
        UWORD ctl;
        UWORD dataa;
        UWORD datab;
    } spr[8];

    UWORD color[32];
    UWORD htotal;
    UWORD hsstop;
    UWORD hbstrt;
    UWORD hbstop;
    UWORD vtotal;
    UWORD vsstop;
    UWORD vbstrt;
    UWORD vbstop;
    UWORD sprhstrt;
    UWORD sprhstop;
    UWORD bplhstrt;
    UWORD bplhstop;
    UWORD hhposw;
    UWORD hhposr;
    UWORD beamcon0;
    UWORD hsstrt;
    UWORD vsstrt;
    UWORD hcenter;
    UWORD diwhigh;
    UWORD padf3[11];
    UWORD fmode;
};

/* From compiler/include/hardware/cia.h ***************************************/

struct CIA
{
    UBYTE ciapra;
    UBYTE ciapad0[255];
    UBYTE ciaprb;
    UBYTE ciapad1[255];
    UBYTE ciaddra;
    UBYTE ciapad2[255];
    UBYTE ciaddrb;
    UBYTE ciapad3[255];
    UBYTE ciatalo;
    UBYTE ciapad4[255];
    UBYTE ciatahi;
    UBYTE ciapad5[255];
    UBYTE ciatblo;
    UBYTE ciapad6[255];
    UBYTE ciatbhi;
    UBYTE ciapad7[255];
    UBYTE ciatodlow;
    UBYTE ciapad8[255];
    UBYTE ciatodmid;
    UBYTE ciapad9[255];
    UBYTE ciatodhi;
    UBYTE ciapad10[255];
    UBYTE unusedreg;
    UBYTE ciapad11[255];
    UBYTE ciasdr;
    UBYTE ciapad12[255];
    UBYTE ciaicr;
    UBYTE ciapad13[255];
    UBYTE ciacra;
    UBYTE ciapad14[255];
    UBYTE ciacrb;
};

/* From arch/m68k-amiga/devs/trackdisk/trackdisk_device.h *********************/

/* Maximum number of units */
#define TD_NUMUNITS 4

/* something */
#define TDUF_WRITE      (1<<0)

#define TDU_NODISK      0x00
#define TDU_DISK        0x01

#define TDU_READONLY    0x01
#define TDU_WRITABLE    0x00

#define DT_UNDETECTED 0
#define DT_ADOS 1
#define DT_PCDOS 2

struct TDU
{
    struct  TDU_PublicUnit pub;
    struct List tdu_Listeners;
    UBYTE       tdu_UnitNum;        /* Unit number */
    UBYTE       tdu_DiskIn;         /* Disk in drive? */
    UBYTE       tdu_MotorOn;        /* Motor on? */
    UBYTE       tdu_ProtStatus;
    UBYTE       tdu_flags;
    BOOL        tdu_hddisk;         /* HD disk inserted */
    BOOL        tdu_broken;         /* recalibrate didn't find TRACK0, drive ignored */
    UBYTE       tdu_sectors;        /* number of sectors per track */
    BOOL        tdu_selected;
    UBYTE       tdu_disktype;
};

struct TrackDiskBase
{
    //struct Device           td_device;
    //struct TaskData         *td_TaskData;
    struct TDU              *td_Units[TD_NUMUNITS];
    //struct timerequest      *td_TimerIO;
    struct timerequest      *td_TimerIO2;
    //struct MsgPort        *td_TimerMP;
    struct MsgPort          *td_TimerMP2;
    //struct DiskBase       *td_DiskBase;
    //struct DiscResourceUnit td_dru;
    //struct MsgPort          td_druport;
    //struct MsgPort          td_Port;            // MessagePort
    //struct Task             *td_task;
    APTR                    td_DMABuffer;       /* Buffer for DMA accesses */
    UBYTE                   *td_DataBuffer;
    ULONG                   td_sectorbits;
    volatile struct Custom  *custom;
    volatile struct CIA     *ciaa;
    volatile struct CIA     *ciab;
    ULONG                   td_IntBit;
    BOOL                    td_nomount;
    BOOL                    td_supportHD;
    WORD                    td_buffer_track;
    BYTE                    td_buffer_unit;     /* buffer contains this unit's track */
    UBYTE                   td_lastdir;         /* last step direction */
    BOOL                    td_dirty;
    UWORD                   *crc_table16;       /* PCDOS checksum table */
};

/* From compiler/include/devices/trackdisk.h **********************************/

#define TDERR_NotSpecified   20
#define TDERR_NoSecHdr       21
#define TDERR_BadSecPreamble 22
#define TDERR_BadSecID       23
#define TDERR_BadHdrSum      24
#define TDERR_BadSecSum      25
#define TDERR_TooFewSecs     26
#define TDERR_BadSecHdr      27
#define TDERR_WriteProt      28
#define TDERR_DiskChanged    29
#define TDERR_SeekError      30
#define TDERR_NoMem          31
#define TDERR_BadUnitNum     32
#define TDERR_BadDriveType   33
#define TDERR_DriveInUse     34
#define TDERR_PostReset      35

/* From compiler/include/exec/ports.h *****************************************/

/* Message */
struct Message
{
    struct Node      mn_Node;
    struct MsgPort * mn_ReplyPort;  /* message reply port */
    UWORD            mn_Length;     /* total message length, in bytes */
                                    /* (include the size of the Message
                                       structure in the length) */
};

/* MsgPort */
struct MsgPort
{
    struct Node mp_Node;
    UBYTE       mp_Flags;
    UBYTE       mp_SigBit;  /* Signal bit number */
    void      * mp_SigTask; /* Object to be signalled */
    struct List mp_MsgList; /* Linked list of messages */
};

/* From compiler/include/exec/io.h ********************************************/

struct IORequest
{
    struct Message  io_Message;
    struct Device * io_Device;
    struct Unit   * io_Unit;
    UWORD           io_Command;
    UBYTE           io_Flags;
    BYTE            io_Error;
};

/* From compiler/include/aros/types/timeval_s.h *******************************/

/* The following structure is composed of two anonymous unions so that it
   can be transparently used by both AROS-style programs and POSIX-style
   ones. For binary compatibility reasons the fields in the unions MUST
   have the same size, however they can have different signs (as it is
   the case for microseconds).  */

__extension__ struct timeval
{
    union  /* Seconds passed. */
    {
        unsigned AROS_32BIT_TYPE tv_secs;   /* AROS field */
        unsigned AROS_32BIT_TYPE tv_sec;    /* POSIX field */
    };
    union /* Microseconds passed in the current second. */
    {
        unsigned AROS_32BIT_TYPE tv_micro; /* AROS field */
        signed   AROS_32BIT_TYPE tv_usec;  /* POSIX field */
    };
};

/* From compiler/include/devices/timer.h **************************************/

/* Units */
#define UNIT_MICROHZ    0
#define UNIT_VBLANK     1
#define UNIT_ECLOCK     2
#define UNIT_WAITUNTIL  3
#define UNIT_WAITECLOCK 4

/* IO-Commands */
#define TR_ADDREQUEST (CMD_NONSTD+0)
#define TR_GETSYSTIME (CMD_NONSTD+1)
#define TR_SETSYSTIME (CMD_NONSTD+2)

struct EClockVal
{
    ULONG ev_hi;
    ULONG ev_lo;
};

struct timerequest
{
    struct IORequest tr_node;
    struct timeval   tr_time;
};

/* From rom/exec/setsignal.c **************************************************/

static ULONG SetSignal(ULONG newSignals, ULONG signalSet)
{
    //D(bug("SetSignal(0x%08lx, 0x%08lx)\n", newSignals, signalSet));

    // Not implemented
    return 0;
}

/* From rom/exec/wait.c *******************************************************/

static ULONG Wait(ULONG signalSet)
{
    volatile UWORD *intreq = (volatile UWORD *)0xDFF09C;
    volatile UWORD *intreqr = (volatile UWORD *)0xDFF01E;

    //D(bug("Wait()\n"));
    while (!(*intreqr & 0x0002)); // Wait interrupt
    *intreq = 0x0002; // Clear interrupt
    //D(bug("Wait() done\n"));

    return signalSet;
}

/* Very lame EmuTOS wait routines *********************************************/

static void wait_1_millisecond(void)
{
    volatile struct CIA* ciab = (volatile struct CIA*)0xbfd000;

    // Start the CIAB Timer A in one shot mode
    ciab->ciacra &= 0xc0; // Stop
    ciab->ciacra |= 0x08; // Start one shot

    // The timer will start after writing the high byte
    ciab->ciatalo = 0xc5;   // Low byte
    ciab->ciatahi = 0x02;   // High byte

    // Wait for the interrupt flag
    while (!(ciab->ciaicr & 0x01));
}

static void wait_stop(void)
{
    volatile struct CIA* ciab = (volatile struct CIA*)0xbfd000;

    // Stop the CIAB Timer A
    ciab->ciacra &= 0xc0; // Stop
}

static void wait_microseconds(ULONG us)
{
    ULONG ms;

    for (ms = us / 1000; ms > 0; --ms)
        wait_1_millisecond();
}

static void wait_seconds(ULONG s)
{
    int i;

    for (; s > 0; --s)
    {
        for (i = 0; i < 1000; i++)
        {
            wait_1_millisecond();
        }
    }
}

/* From rom/exec/waitio.c *****************************************************/

static void SendIO(struct IORequest *iORequest)
{
    // Assume that iORequest == tdb->td_TimerIO2
    struct timerequest *timer = (struct timerequest *)iORequest;

    //D(bug("SendIO(): wait %lu.%06lu seconds\n", timer->tr_time.tv_secs, timer->tr_time.tv_micro));

    timer->tr_node.io_Error = 0;

    // FIXME: the timer should start here
    // The actual job will be done in WaitIO()
}

static LONG WaitIO(struct IORequest *iORequest)
{
    // Assume that iORequest == tdb->td_TimerIO2
    struct timerequest *timer = (struct timerequest *)iORequest;

    //D(bug("WaitIO()\n"));

    if (timer->tr_node.io_Error)
        return timer->tr_node.io_Error;

    wait_seconds(timer->tr_time.tv_secs);
    wait_microseconds(timer->tr_time.tv_micro);

    return 0;
}

/* From rom/exec/abortio.c ****************************************************/

static LONG AbortIO(struct IORequest *iORequest)
{
    // Assume that iORequest == tdb->td_TimerIO2
    struct timerequest *timer = (struct timerequest *)iORequest;

    //D(bug("AbortIO()\n"));

    wait_stop();

    timer->tr_node.io_Error = IOERR_ABORTED;
    return 0; // OK
}

/* From arch/m68k-amiga/devs/trackdisk/trackdisk_hw.c *************************/

#define DISK_BUFFERSIZE 13630

#define QUICKRETRYRCNT 8 // re-read retries before reseeking, must be power of 2

#define ioStd(x)  ((struct IOStdReq *)x)

static UBYTE td_flush(struct TDU *tdu, struct TrackDiskBase *tdb);

static void td_wait_start(struct TrackDiskBase *tdb, UWORD millis)
{
    // do not remove, AbortIO()+WaitIO() does not clear signal bit
    // if AbortIO() finishes before WaitIO() waits.

    SetSignal(0, 1L << tdb->td_TimerMP2->mp_SigBit);
    tdb->td_TimerIO2->tr_node.io_Command = TR_ADDREQUEST;
    tdb->td_TimerIO2->tr_time.tv_secs = millis / 1000;
    tdb->td_TimerIO2->tr_time.tv_micro = (millis % 1000) * 1000;
    SendIO((struct IORequest *)tdb->td_TimerIO2);
}

static void td_wait_end(struct TrackDiskBase *tdb)
{
    WaitIO((struct IORequest*)tdb->td_TimerIO2);
}

static void td_wait(struct TrackDiskBase *tdb, UWORD millis)
{
    td_wait_start(tdb, millis);
    td_wait_end(tdb);
}

static UBYTE const drvmask[] = { ~0x08, ~0x10, ~0x20, ~0x40 };
static void td_select(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE tmp;

    if (tdu->tdu_selected)
        return;
    tdu->tdu_selected = TRUE;
    tmp = tdb->ciab->ciaprb;
    tmp |= 0x08 | 0x10 | 0x20 | 0x40;
    tdb->ciab->ciaprb = tmp;
    if (tdu->tdu_MotorOn)
       tmp &= ~0x80;
    else
       tmp |= 0x80;
    tdb->ciab->ciaprb = tmp;
    tmp &= drvmask[tdu->tdu_UnitNum];
    tdb->ciab->ciaprb = tmp;
}

static void td_deselect(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE tmp;
    if (!tdu->tdu_selected)
        return;
    tdu->tdu_selected = FALSE;
    tmp = tdb->ciab->ciaprb;
    tmp |= 0x08 | 0x10 | 0x20 | 0x40;
    tdb->ciab->ciaprb = tmp;
    tmp |= 0x80;
    tdb->ciab->ciaprb = tmp;
}

// 0 = no disk, 1 = disk inserted
static UBYTE td_getDiskChange(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE v;
    v = (tdb->ciaa->ciapra & 0x04) ? 1 : 0;
    return v;
}

static UBYTE countbits(ULONG mask)
{
    UBYTE cnt = 0;
    while (mask) {
        cnt++;
        mask >>= 1;
    }
    return cnt;
}

static BOOL checkbuffer(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    // allocate HD sized buffer if HD disk inserted
    if ((tdu->tdu_hddisk && !tdb->td_supportHD) || !tdb->td_DMABuffer) {
        FreeMem(tdb->td_DMABuffer, DISK_BUFFERSIZE);
        FreeMem(tdb->td_DataBuffer, 11 * 512);
        tdb->td_DMABuffer = AllocMem(DISK_BUFFERSIZE * 2, MEMF_CHIP);
        tdb->td_DataBuffer = AllocMem(22 * 512, MEMF_ANY);
        if (!tdb->td_DMABuffer || !tdb->td_DataBuffer) {
            FreeMem(tdb->td_DMABuffer, DISK_BUFFERSIZE * 2);
            FreeMem(tdb->td_DataBuffer, 22 * 512);
            return 1;
        }
        tdb->td_supportHD = TRUE;
    }
    return 0;
}

static void td_clear(struct TrackDiskBase *tdb)
{
    tdb->td_buffer_unit = -1;
    tdb->td_sectorbits = 0;
    tdb->td_dirty = 0;
}

static void td_setside(UBYTE side, struct TDU *tdu, struct TrackDiskBase *tdb)
{
    if (!side) {
        tdb->ciab->ciaprb |= 0x4;
        tdu->pub.tdu_CurrTrk |= 1;
    } else {
        tdb->ciab->ciaprb &= ~0x04;
        tdu->pub.tdu_CurrTrk &= ~1;
    }
}

static void td_setdirection(UBYTE dir, struct TDU *tdu, struct TrackDiskBase *tdb)
{
    if (dir)
        tdb->ciab->ciaprb |= 0x02;
    else
        tdb->ciab->ciaprb &= ~0x02;
    if (dir != tdb->td_lastdir) {
        td_wait(tdb, 18);
        tdb->td_lastdir = dir;
    }
}

static void td_step(struct TDU *tdu, struct TrackDiskBase *tdb, UBYTE delay)
{
    tdb->ciab->ciaprb &= ~0x01;
    tdb->ciab->ciaprb |= 0x01;
    td_wait(tdb, delay);
}

/* start motor */
static void td_motoron(struct TDU *tdu, struct TrackDiskBase *tdb, BOOL wait)
{
    if (tdu->tdu_MotorOn)
        return;
    tdu->tdu_MotorOn = 1;

    td_deselect(tdu, tdb);
    td_select(tdu, tdb);
    if (wait)
        td_wait(tdb, 500);
}

/* stop motor */
static void td_motoroff(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    if (!tdu->tdu_MotorOn)
        return;
    tdu->tdu_MotorOn = 0;

    td_deselect(tdu, tdb);
    td_select(tdu, tdb);
}

static BOOL td_istrackzero(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    return (tdb->ciaa->ciapra & 0x10) == 0;
}

static UBYTE td_getprotstatus(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE v;
    v = (tdb->ciaa->ciapra & 0x08) ? 0 : 1;
    return v;
}

static BOOL td_recalibrate(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    BYTE steps = 80 + 15;
    td_select(tdu, tdb);
    td_setside(0, tdu, tdb);
    td_wait(tdb, tdu->pub.tdu_CalibrateDelay);
    if (td_istrackzero(tdu, tdb)) {
        // step to cyl 1 if current cyl == 0
        td_setdirection(0, tdu, tdb);
        td_wait(tdb, tdu->pub.tdu_SettleDelay);
        td_step(tdu, tdb, tdu->pub.tdu_CalibrateDelay);
    }
    td_wait(tdb, tdu->pub.tdu_SettleDelay);
    td_setdirection(1, tdu, tdb);
    td_wait(tdb, tdu->pub.tdu_SettleDelay);
    while (!td_istrackzero(tdu, tdb)) {
        if (steps < 0) // drive is broken?
            return FALSE;
        td_step(tdu, tdb, tdu->pub.tdu_CalibrateDelay);
        steps--;
    }
    td_wait(tdb, tdu->pub.tdu_SettleDelay);
    tdu->pub.tdu_CurrTrk = 0;
    return TRUE;
}

static UBYTE td_seek2(struct TDU *tdu, UBYTE cyl, UBYTE side, struct TrackDiskBase *tdb, BOOL nowait)
{
    UBYTE dir;
    D(bug("seek=%d/%d\n", cyl, side));
    td_setside(side, tdu, tdb);
    if (tdu->pub.tdu_CurrTrk / 2 == cyl)
        return 0;
    if (tdu->pub.tdu_CurrTrk / 2 > cyl || cyl == 0xff)
        dir = 1;
    else
        dir = 0;
    td_setdirection(dir, tdu, tdb);
    while (cyl != tdu->pub.tdu_CurrTrk / 2) {
        td_step(tdu, tdb, tdu->pub.tdu_StepDelay);
        if (tdu->pub.tdu_CurrTrk / 2 > cyl && tdu->pub.tdu_CurrTrk >= 2)
            tdu->pub.tdu_CurrTrk -= 2;
        else if (tdu->pub.tdu_CurrTrk / 2 < cyl)
            tdu->pub.tdu_CurrTrk += 2;
        if (cyl == 0xff)
            break;
    }
    td_wait_start(tdb, tdu->pub.tdu_SettleDelay);
    if (!nowait)
        td_wait_end(tdb);
    return 0;
}

static UBYTE td_seek(struct TDU *tdu, int cyl, int side, struct TrackDiskBase *tdb)
{
    return td_seek2(tdu, cyl, side, tdb, FALSE);
}

static UBYTE td_seek_nowait(struct TDU *tdu, int cyl, int side, struct TrackDiskBase *tdb)
{
    return td_seek2(tdu, cyl, side, tdb, TRUE);
}

static void td_encodebuffer(struct TDU *tdu, struct TrackDiskBase *tdb)
{
/*
    if (tdu->tdu_disktype == DT_ADOS)
        td_encodebuffer_ados(tdu, tdb);
*/
}

static UBYTE td_readwritetrack(UBYTE track, UBYTE write, struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE err = 0;
    ULONG sigs;
    UWORD dsklen = 0x8000 | ((DISK_BUFFERSIZE / 2) * (tdu->tdu_hddisk ? 2 : 1)) | (write ? 0x4000 : 0);

    td_motoron(tdu, tdb, TRUE);

    SetSignal(0, 1L << tdb->td_IntBit);

    tdb->custom->adkcon = 0x0600;
    tdb->custom->adkcon = write ? 0x9100 : 0x9500;
    tdb->custom->intreq = 0x0002; // clear disk interrupt request
    //tdb->custom->intena = 0x8002; // enable disk interrupt
    tdb->custom->dmacon = 0x8010; // enable DMA

    tdb->custom->dskpt = tdb->td_DMABuffer;
    tdb->custom->dsklen = dsklen;
    tdb->custom->dsklen = dsklen; // dma started
    D(bug("td diskdma started, track=%d write=%d len=%d\n", track, write, dsklen & 0x3fff));

    td_wait_start(tdb, (tdu->tdu_hddisk ? 2 : 1) * 1000);
    sigs = Wait((1L << tdb->td_TimerMP2->mp_SigBit) | (1L << tdb->td_IntBit));

    tdb->custom->dsklen = 0x4000;
    tdb->custom->intena = 0x0002;

    err = TDERR_BadSecPreamble;
    if (sigs & (1L << tdb->td_IntBit)) {
        // dma finished
        err = 0;
        AbortIO((struct IORequest *)tdb->td_TimerIO2);
    }
    WaitIO((struct IORequest *)tdb->td_TimerIO2);
    D(bug("td diskdma finished, err=%d\n", err));

    if (td_getDiskChange(tdu, tdb) == 0)
        err = TDERR_DiskChanged;

    return err;
}

static void make_crc_table16(struct TrackDiskBase *tdb)
{
    unsigned short w;
    int n, k;

    //tdb->crc_table16 = AllocMem(256 * sizeof(UWORD), MEMF_ANY);
    if (!tdb->crc_table16)
        return;
    for (n = 0; n < 256; n++) {
        w = n << 8;
        for (k = 0; k < 8; k++) {
            w = (w << 1) ^ ((w & 0x8000) ? 0x1021 : 0);
        }
        tdb->crc_table16[n] = w;
    }
}

static UWORD get_crc16_next(struct TrackDiskBase *tdb, void *vbuf, UWORD len, UWORD crc)
{
    UBYTE *buf = (UBYTE*)vbuf;
    while (len-- > 0)
        crc = (crc << 8) ^ tdb->crc_table16[((crc >> 8) ^ (*buf++)) & 0xff];
    return crc;
}
static UWORD get_crc16(struct TrackDiskBase *tdb, void *vbuf, UWORD len)
{
    if (!tdb->crc_table16) {
        make_crc_table16(tdb);
        if (!tdb->crc_table16)
            return 0;
    }
    return get_crc16_next(tdb, vbuf, len, 0xffff);
}

/* Following really needs optimization... */
static UBYTE mfmdecode (UWORD **mfmp)
{
    UWORD mfm;
    UBYTE out;
    UBYTE i;

    mfm = **mfmp;
    out = 0;
    (*mfmp)++;
    for (i = 0; i < 8; i++) {
        out >>= 1;
        if (mfm & 1)
            out |= 0x80;
        mfm >>= 2;
    }
    return out;
}

/* PC floppy format decoding is very expensive compared to ADOS... */
static UBYTE td_decodebuffer_pcdos(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UWORD *raw, *rawend;
    UWORD i;
    UBYTE lasterr;
    UBYTE *data = tdb->td_DataBuffer;
    BYTE sector;
    UBYTE tmp[8];
    UWORD datacrc;
    UBYTE current_head, current_cyl;

    tmp[0] = tmp[1] = tmp[2] = 0xa1;
    tmp[3] = 0xfb;
    datacrc = get_crc16(tdb, tmp, 4);
    current_head = tdb->td_buffer_track & 1;
    current_cyl = tdb->td_buffer_track / 2;

    lasterr = 0;
    raw = tdb->td_DMABuffer;
    rawend = tdb->td_DMABuffer + DISK_BUFFERSIZE * (tdu->tdu_hddisk ? 2 : 1);
    sector = -1;
    while (tdb->td_sectorbits != (1 << tdu->tdu_sectors) - 1) {
        UBYTE *secdata;
        UWORD crc;
        UBYTE mark;

        if (raw != tdb->td_DMABuffer) {
            while (*raw != 0x4489) {
                if (raw >= rawend) {
                    if (lasterr == 0)
                        lasterr = TDERR_TooFewSecs;
                    goto end;
                }
                raw++;
            }
        }
        while (*raw == 0x4489 && raw < rawend)
            raw++;
        if (raw >= rawend - 8) {
            if (lasterr == 0)
                lasterr = TDERR_TooFewSecs;
            goto end;
        }
        mark = mfmdecode(&raw);
        if (mark == 0xfe) {
            UBYTE cyl, head, size;

            cyl = mfmdecode (&raw);
            head = mfmdecode (&raw);
            sector = mfmdecode (&raw);
            size = mfmdecode (&raw);
            crc = (mfmdecode (&raw) << 8) | mfmdecode (&raw);

            tmp[0] = 0xa1; tmp[1] = 0xa1; tmp[2] = 0xa1; tmp[3] = mark;
            tmp[4] = cyl; tmp[5] = head; tmp[6] = sector; tmp[7] = size;
            if (get_crc16(tdb, tmp, 8) != crc || cyl != current_cyl || head != current_head || size != 2 || sector < 1 || sector > tdu->tdu_sectors) {
                D(bug("PCDOS: corrupted sector header. cyl=%d head=%d sector=%d size=%d crc=%04x\n",
                    cyl, head, sector, size, crc));
                sector = -1;
                continue;
            }
            sector--;
            continue;
        }
        if (mark != 0xfb) {
            D(bug("PCDOS: unknown address mark %02X\n", mark));
            continue;
        }
        if (sector < 0)
            continue;
        if (raw >= rawend - 512) {
            if (lasterr == 0)
                lasterr = TDERR_TooFewSecs;
            goto end;
        }
        secdata = data + sector * 512;
        for (i = 0; i < 512; i++)
            secdata[i] = mfmdecode (&raw);
        crc = (mfmdecode (&raw) << 8) | mfmdecode (&raw);
        if (get_crc16_next(tdb, secdata, 512, datacrc) != crc) {
            D(bug("PCDOS: sector %d data checksum error\n", sector + 1));
            continue;
        }
        D(bug("PCDOS: read sector %d\n", sector));
        tdb->td_sectorbits |= 1 << sector;
        sector = -1;
    }
end:
    D(bug("PCDOS: err=%d secmask=%08lx\n", lasterr, tdb->td_sectorbits));
    return lasterr;
}

static UBYTE td_decodebuffer_ados(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    // Not implemented
    return TDERR_NotSpecified;
}

static UBYTE td_decodebuffer(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    if (tdu->tdu_disktype == DT_ADOS)
        return td_decodebuffer_ados(tdu, tdb);
    else if (tdu->tdu_disktype == DT_PCDOS)
        return td_decodebuffer_pcdos(tdu, tdb);
    return TDERR_TooFewSecs;
}

static UBYTE td_readbuffer(UBYTE track, struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE ret;

    if (tdb->td_buffer_unit != tdu->tdu_UnitNum || tdb->td_buffer_track != track)
        tdb->td_sectorbits = 0;
    tdb->td_buffer_unit = tdu->tdu_UnitNum;
    tdb->td_buffer_track = track;
    td_select(tdu, tdb);
    td_seek(tdu, track >> 1, track & 1, tdb);
    ret = td_readwritetrack(track, 0, tdu, tdb);
    if (ret) {
        D(bug("td_readbuffer TRK=%d td_readwritetrack ERR=%d\n", track, ret));
        tdb->td_sectorbits = 0;
        return ret;
    }
    ret = td_decodebuffer(tdu, tdb);
    D(bug("td_readbuffer td_decodebuffer ERR=%d MASK=%08lx\n", ret, tdb->td_sectorbits));
    return ret;
}

static void td_detectformat(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE err;
    UBYTE track = 0;
    UBYTE cnt;

    D(bug("detectformat HD=%d\n", tdu->tdu_hddisk ? 1 : 0));
    if (checkbuffer(tdu, tdb)) {
        tdu->tdu_disktype = DT_ADOS;
        tdu->tdu_sectors = tdu->tdu_hddisk ? 22 : 11;
        return;
    }
    tdu->tdu_disktype = DT_UNDETECTED;
    tdb->td_sectorbits = 0;
    tdb->td_buffer_unit = tdu->tdu_UnitNum;
    tdb->td_buffer_track = track;
    td_select(tdu, tdb);
    td_seek(tdu, track >> 1, track & 1, tdb);
    tdu->tdu_sectors = tdu->tdu_hddisk ? 22 : 11;
    err = td_readwritetrack(track, 0, tdu, tdb);
    if (!err) {
        err = td_decodebuffer_ados(tdu, tdb);
        /* Did all sectors fail to decode? It could be non-ADOS disk */
        if (err && tdb->td_sectorbits == 0) {
            tdu->tdu_sectors = tdu->tdu_hddisk ? 18 : 9;
            err = td_decodebuffer_pcdos(tdu, tdb);
            cnt = countbits(tdb->td_sectorbits);
            if (cnt > tdu->tdu_sectors / 2 + 1) {
                /* enough sectors decoded fine, assume PCDOS */
                tdu->tdu_disktype = DT_PCDOS;
            }
        } else {
            tdu->tdu_disktype = DT_ADOS;
        }
    }
    if (tdu->tdu_disktype == DT_UNDETECTED) {
        tdu->tdu_disktype = DT_ADOS;
        tdu->tdu_sectors = tdu->tdu_hddisk ? 22 : 11;
        tdb->td_sectorbits = 0;
    }
    D(bug("detectformat=%d sectors=%d sectormask=%08lx\n", tdu->tdu_disktype, tdu->tdu_sectors, tdb->td_sectorbits));
 }

static void maybe_detect(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    if (tdu->tdu_disktype == DT_UNDETECTED)
        td_detectformat(tdu, tdb);
}

static UBYTE maybe_flush(struct TDU *tdu, struct TrackDiskBase *tdb, int track)
{
    if (tdb->td_buffer_unit != tdu->tdu_UnitNum || tdb->td_buffer_track != track) {
        UBYTE err = 0;
        err = td_flush(tdu, tdb);
        td_clear(tdb);
        return err;
    }
    return 0;
}

static UBYTE td_read(struct IOExtTD *iotd, struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE err;
    APTR data;
    ULONG len, offset;
    WORD totalretries;
    BYTE seeking;

    if (tdu->tdu_DiskIn == TDU_NODISK)
        return TDERR_DiskChanged;
    if (checkbuffer(tdu, tdb))
        return TDERR_NoMem;
    maybe_detect(tdu, tdb);

    iotd->iotd_Req.io_Actual = 0;
    offset = iotd->iotd_Req.io_Offset;
    len = iotd->iotd_Req.io_Length;
    data = iotd->iotd_Req.io_Data;

    D(bug("td_read: DATA=%lx OFFSET=%lx (TRK=%ld) LEN=%ld\n", (ULONG)data, offset, offset / (512 * tdu->tdu_sectors), len));

    seeking = 0;
    err = 0;
    totalretries = (tdu->pub.tdu_RetryCnt + 1) * QUICKRETRYRCNT - 1;

    while (len > 0 && totalretries >= 0) {

        UBYTE largestsectorneeded, smallestsectorneeded, totalsectorsneeded;
        UBYTE track;
        UBYTE sec, sectorsdone;

        track = offset / (512 * tdu->tdu_sectors);

        if (seeking)
            td_wait_end(tdb);

        if ((totalretries % QUICKRETRYRCNT) == 0) {
            if (!td_recalibrate(tdu, tdb)) {
                err = TDERR_SeekError;
                break;
            }
        }

        err = maybe_flush(tdu, tdb, track);
        if (err)
            break;
        if (tdb->td_buffer_unit != tdu->tdu_UnitNum || tdb->td_buffer_track != track)
            err = td_readbuffer(track, tdu, tdb);

        smallestsectorneeded = (offset / 512) % tdu->tdu_sectors;
        largestsectorneeded = smallestsectorneeded + len / 512;
        if (largestsectorneeded > tdu->tdu_sectors || len / 512 > tdu->tdu_sectors) {
            UBYTE nexttrack = track + 1;
            if (nexttrack < 160) {
                // start stepping to next track in advance (pointless but..)
                td_seek_nowait(tdu, nexttrack >> 1, nexttrack & 1, tdb);
                seeking = 1;
            }
            largestsectorneeded = tdu->tdu_sectors;
        }
        totalsectorsneeded = largestsectorneeded - smallestsectorneeded;

        sectorsdone = 0;
        for (sec = smallestsectorneeded; sec < largestsectorneeded; sec++) {
            if (tdb->td_sectorbits & (1 << sec)) {
                CopyMem(tdb->td_DataBuffer + sec * 512, data + (sec - smallestsectorneeded) * 512, 512);
                sectorsdone++;
            }
        }

        D(bug("td_read2 TRK=%d MIN=%d MAX=%d DONE=%d\n", track, smallestsectorneeded, largestsectorneeded, sectorsdone));

        if (sectorsdone < totalsectorsneeded) {
            // errors, force re-read
            tdb->td_buffer_unit = -1;
            // couldn't decode any sectors = reseek immediately
            if (tdb->td_sectorbits == 0)
                totalretries = (totalretries - 1) & ~(QUICKRETRYRCNT - 1);
            else
                totalretries--;
            continue;
        }

        data += sectorsdone * 512;
        offset += sectorsdone * 512;
        len -= sectorsdone * 512;
        iotd->iotd_Req.io_Actual += sectorsdone * 512;

        err = 0;
    }

    if (seeking)
        td_wait_end(tdb);
    D(bug("td_read2 ERR=%d io_Actual=%lu\n", err, iotd->iotd_Req.io_Actual));
    return err;
}

static UBYTE td_write2(struct IOExtTD *iotd, struct TDU *tdu, struct TrackDiskBase *tdb)
{
    // Not implemented
    return TDERR_WriteProt;
}

static UBYTE td_write(struct IOExtTD *iotd, struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE err;
    if (tdu->tdu_DiskIn == TDU_NODISK)
        return TDERR_DiskChanged;
    if (tdu->tdu_disktype != DT_ADOS)
        return TDERR_WriteProt;
    if (!td_getprotstatus(tdu, tdb)) {
        err = td_write2(iotd, tdu, tdb);
    } else {
        err = TDERR_WriteProt;
    }
    return err;
}

static UBYTE td_flush(struct TDU *tdu, struct TrackDiskBase *tdb)
{
    WORD totalretries;
    UBYTE lasterr, err;

    if (tdb->td_buffer_unit != tdu->tdu_UnitNum)
        return 0;
    if (!tdb->td_dirty)
        return 0;

    err = 0;
    td_select(tdu, tdb);
    td_seek(tdu, tdb->td_buffer_track >> 1, tdb->td_buffer_track & 1, tdb);
    D(bug("td_flush, writing unit %d track %d wmask=%08lx\n",
        tdb->td_buffer_unit, tdb->td_buffer_track, tdb->td_sectorbits));

    totalretries = (tdu->pub.tdu_RetryCnt + 1) * QUICKRETRYRCNT - 1;
    // read all non-modified sectors from disk (if needed)
    lasterr = 0;
    while (tdb->td_sectorbits != (1 << tdu->tdu_sectors) - 1) {
        if ((totalretries % QUICKRETRYRCNT) == 0) {
            if (!td_recalibrate(tdu, tdb)) {
                tdb->td_dirty = 0;
                tdb->td_buffer_unit = -1;
                return TDERR_SeekError;
            }
        }
        err = td_readbuffer(tdb->td_buffer_track, tdu, tdb);
        if (err)
            lasterr = err;
        D(bug("TD_FLUSH READBUF ERR=%d MASK=%08lx\n", err, tdb->td_sectorbits));
        if (totalretries-- <= 0) {
            bug("TD_FLUSH disk error track %d, written data was lost!\n", tdb->td_buffer_track);
            tdb->td_dirty = 0;
            tdb->td_buffer_unit = -1;
            return lasterr ? lasterr : TDERR_NotSpecified;
        }
    }
    // MFM encode buffer
    td_encodebuffer(tdu, tdb);
    // write buffer
    err = td_readwritetrack(tdb->td_buffer_track, 1, tdu, tdb);
    td_wait(tdb, 2);
    tdb->td_dirty = 0;
    return err;
}

/* From arch/m68k-amiga/devs/trackdisk/trackdisk_device.c *********************/

static void TestInsert(struct TrackDiskBase *tdb, struct TDU *tdu, BOOL dostep)
{
    //struct IOExtTD *iotd;

    if (dostep) {
        if (tdu->pub.tdu_PubFlags & TDPF_NOCLICK) {
            td_seek(tdu, -1, 0, tdb);
        } else {
            // step towards cyl 0 if > 0, if not, step to cyl 1
            td_seek(tdu, tdu->pub.tdu_CurrTrk >= 2 ? (tdu->pub.tdu_CurrTrk - 2) / 2 : 1, 0, tdb);
        }
    }
    if (td_getDiskChange (tdu, tdb)) {
        //struct DiskBase *DiskBase = tdb->td_DiskBase;
        D(bug("[Floppy] Insertion detected\n"));
        td_recalibrate(tdu, tdb);
        //tdu->tdu_hddisk = ishd(ReadUnitID(tdu->tdu_UnitNum));
        td_detectformat(tdu, tdb);
        tdu->tdu_DiskIn = TDU_DISK;
        tdu->pub.tdu_Counter++;
        tdu->tdu_ProtStatus = td_getprotstatus(tdu,tdb);
/*
        Forbid();
        ForeachNode(&tdu->tdu_Listeners,iotd) {
            Cause((struct Interrupt *)((struct IOExtTD *)iotd->iotd_Req.io_Data));
        }
        Permit();
*/
    }
}

static void td_chagestate(struct IOExtTD *iotd, struct TDU *tdu, struct TrackDiskBase *tdb)
{
    UBYTE temp;

    td_select(tdu, tdb);
    if (tdu->tdu_DiskIn == TDU_NODISK)
        //TestInsert(tdb, tdu, FALSE);
        TestInsert(tdb, tdu, TRUE);
    if (tdu->tdu_DiskIn == TDU_DISK) {
        /* test if disk is still in there */
        temp = td_getDiskChange(tdu, tdb);
        iotd->iotd_Req.io_Actual = temp ? 0 : 1;
        tdu->tdu_DiskIn = temp ? TDU_DISK : TDU_NODISK;
    } else {
        /* No disk in drive */
        iotd->iotd_Req.io_Actual = 1;
    }
    D(bug("TD%d_CHANGESTATE=%ld\n", tdu->tdu_UnitNum, iotd->iotd_Req.io_Actual));
}

/* EmuTOS to AROS glue for floppy support *************************************/

static UBYTE DMABuffer[DISK_BUFFERSIZE];
static UBYTE DataBuffer[11 * 512];
static UWORD crc_table16[256];

static struct IOExtTD iotd;
static struct TDU tdu0;
static struct TDU tdu1;
static struct timerequest timerIO2;
static struct MsgPort timerMP2;
static struct TrackDiskBase tdb0;

static void init_trackdisk(void)
{
    struct TrackDiskBase* TDBase = &tdb0;

    memset(&iotd, 0, sizeof iotd);
    memset(&tdu0, 0, sizeof tdu0);
    memset(&tdu1, 0, sizeof tdu1);
    memset(&tdb0, 0, sizeof tdb0);
    memset(&timerIO2, 0, sizeof timerIO2);
    memset(&timerMP2, 0, sizeof timerMP2);

    // From From arch/m68k-amiga/devs/trackdisk/trackdisk_device.c, GM_UNIQUENAME(init)()
    TDBase->td_supportHD = 0;
    //TDBase->td_DiskBase = DiskBase;
    TDBase->ciaa = (struct CIA*)0xbfe001;
    TDBase->ciab = (struct CIA*)0xbfd000;
    TDBase->custom = (struct Custom*)0xdff000;
    TDBase->td_TimerIO2 = &timerIO2;
    TDBase->td_TimerMP2 = &timerMP2;

    /* Alloc memory for track buffering, DD buffer only, reallocated
     * later if HD disk detected to save RAM on unexpanded machines */
    TDBase->td_DMABuffer = DMABuffer;
    TDBase->td_DataBuffer = DataBuffer;

    TDBase->crc_table16 = crc_table16;
    make_crc_table16(TDBase);

    // From arch/m68k-amiga/disk/disk_intern_init.c, disk_internal_init()
    TDBase->ciaa->ciaddra &= ~(0x20 | 0x10 | 0x08 | 0x04); // RDY TK0 WPRO CHNG = input
    TDBase->ciab->ciaprb = 0xff; // inactive
    TDBase->ciab->ciaddrb = 0xff; // MTR SELx SIDE DIR STEP = inactive and output

    TDBase->custom->dsklen = 0x4000; // dsklen idle
    TDBase->custom->dmacon = 0x8010; // disk dma on
    TDBase->custom->dsksync = 0x4489; // sync
    TDBase->custom->adkcon = 0x7f00;
    TDBase->custom->adkcon = 0x8000 | 0x1000 | 0x0400 | 0x0100; // mfm, wordsync, fast
}

BOOL aros_flop_detect_drive(WORD dev)
{
    struct TrackDiskBase* tdb = &tdb0;
    struct TDU* tdu = (dev == 0) ? &tdu0 : &tdu1;
    struct TDU* unit = tdu;
    ULONG i = dev;

    MAYBE_UNUSED(i);

    if (!tdb->td_DMABuffer)
        init_trackdisk();

    // From arch/m68k-amiga/devs/trackdisk/trackdisk_device.c, TD_InitUnit()
    unit->tdu_DiskIn = TDU_NODISK;  /* Assume there is no floppy in there */
    unit->pub.tdu_StepDelay = 3;    /* Standard values here */
    unit->pub.tdu_SettleDelay = 15;
    unit->pub.tdu_RetryCnt = 3;
    unit->pub.tdu_CalibrateDelay = 3;
    unit->tdu_UnitNum = dev;
    tdb->td_Units[dev] = unit;

    // From arch/m68k-amiga/devs/trackdisk/trackdisk_device.c, TD_DevTask()
    td_select(tdu, tdb);
    td_motoroff(tdu, tdb);
    tdu->tdu_broken = td_recalibrate(tdu, tdb) == 0;
    if (tdu->tdu_broken)
        D(bug("DF%ld failed to recalibrate!?\n", i));
    else
        D(bug("DF%ld initialized\n", i));
    tdu->tdu_DiskIn = td_getDiskChange(tdu, tdb) ? TDU_DISK : TDU_NODISK;
    tdu->tdu_ProtStatus = td_getprotstatus(tdu,tdb);
    //tdu->tdu_hddisk = ishd(GetUnitID(i));
    tdu->tdu_sectors = tdu->tdu_hddisk ? 22 : 11;
    tdu->tdu_disktype = DT_UNDETECTED;
    td_deselect(tdu, tdb);

    return !tdu->tdu_broken;
}

WORD aros_floprw(UBYTE *buf, WORD rw, WORD dev, WORD sect, WORD track, WORD side, WORD count)
{
    struct TrackDiskBase* tdb = &tdb0;
    struct TDU* tdu = (dev == 0) ? &tdu0 : &tdu1;
    UBYTE err;

    // Very experimental
    td_chagestate(&iotd, tdu, tdb);

    // td_read() parameters
    iotd.iotd_Req.io_Offset = (512UL * 9 * 2 * track) + (512UL * 9 * side) + (512UL * (sect - 1));
    iotd.iotd_Req.io_Data = (APTR)buf;
    iotd.iotd_Req.io_Length = (ULONG)count * 512;

    D(bug("aros_floprw buf=0x%08lx track=%d side=%d sect=%d count=%d offset=%lu\n", (ULONG)buf, track, side, sect, count, iotd.iotd_Req.io_Offset));
    if (rw & 1) {
        err = td_write(&iotd, tdu, tdb);
        D(bug("td_write() returns %d\n", err));
    }
    else {
        err = td_read(&iotd, tdu, tdb);
        D(bug("td_read() returns %d\n", err));
    }
    return err ? ERR : E_OK;
}

#endif /* defined(MACHINE_AMIGA) && CONF_WITH_AROS */
