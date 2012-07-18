/*
 * amiga.c - Amiga specific functions
 *
 * Copyright (c) 2012 EmuTOS development team
 * Some parts Copyright (c) 1995-2007, The AROS Development Team.
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_AMIGA 0

#include "config.h"
#include "portab.h"
#include "amiga.h"
#include "vectors.h"
#include "tosvars.h"
#include "kprint.h"
#include "string.h"
#include "processor.h"

#ifdef MACHINE_AMIGA

/*
 * DMA register bits
 */
#define DMAF_SETCLR             0x8000
#define DMAF_AUD0               0x0001
#define DMAF_AUD1               0x0002
#define DMAF_AUD2               0x0004
#define DMAF_AUD3               0x0008
#define DMAF_DISK               0x0010
#define DMAF_SPRITE             0x0020
#define DMAF_BLITTER            0x0040
#define DMAF_COPPER             0x0080
#define DMAF_RASTER             0x0100
#define DMAF_MASTER             0x0200
#define DMAF_BLITHOG            0x0400
#define DMAF_BLTNZERO           0x2000
#define DMAF_BLTDONE            0x4000
#define DMAF_ALL                0x01ff

#define JOY0DAT *(volatile UWORD*)0xdff00a
#define CIAAPRA *(volatile UBYTE*)0xbfe001
#define POTGO *(volatile UWORD*)0xdff034
#define POTGOR *(volatile UWORD*)0xdff016 // = POTINP

/* Screen *********************************************************************/

UWORD copper_list[6];

void amiga_screen_init(void)
{
    sshiftmod = 0x02; // We emulate the ST-High monochrome video mode

    *(volatile UWORD*)0xdff100 = 0x9204; // Hires, one bit-plane, interlaced
    *(volatile UWORD*)0xdff102 = 0; // Horizontal scroll value 0
    *(volatile UWORD*)0xdff108 = 80; // Modulo = 80 for odd bit-planes
    *(volatile UWORD*)0xdff10a = 80; // Ditto for even bit-planes
    *(volatile UWORD*)0xdff092 = 0x003c; // Set data-fetch start for hires
    *(volatile UWORD*)0xdff094 = 0x00d4; // Set data-fetch stop
    *(volatile UWORD*)0xdff08e = 0x2c81; // Set display window start
    *(volatile UWORD*)0xdff090 = 0xf4c1; // Set display window stop

    // Set up color registers
    *(volatile UWORD*)0xdff180 = 0x0fff; // Background color = white
    *(volatile UWORD*)0xdff182 = 0x0000; // Foreground color = black

    // Set up the Copper list (must be in ST-RAM)
    copper_list[0] = 0x0e0; // BPL1PTH
    copper_list[1] = (((ULONG)v_bas_ad) & 0xffff0000) >> 16;
    copper_list[2] = 0x0e2; // BPL1PTL
    copper_list[3] = (((ULONG)v_bas_ad) & 0x0000ffff);
    copper_list[4] = 0xffff; // End of
    copper_list[5] = 0xfffe; // Copper list

    // Initialize the Copper
    *(UWORD* volatile *)0xdff080 = copper_list; // COP1LCH
    *(volatile UWORD*)0xdff088 = 0; // COPJMP1

    // VBL interrupt
    VEC_LEVEL3 = amiga_vbl;
    *(volatile UWORD*)0xdff09a = 0xc020; // INTENA Set Master and VBL bits

    // Start the DMA
    *(volatile UWORD*)0xdff096 = DMAF_SETCLR | DMAF_COPPER | DMAF_RASTER | DMAF_MASTER;
}

/* This scancode table will be used by amiga_int_ciaa_serial */
const UBYTE scancode_atari_from_amiga[128] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x29, 0x00, 0x71,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x00, 0x6d, 0x6e, 0x6f,
    0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x2b, 0x00, 0x6a, 0x6b, 0x6c,
    0x60, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32,
    0x33, 0x34, 0x35, 0x00, 0x00, 0x67, 0x68, 0x69,
    0x39, 0x0e, 0x0f, 0x72, 0x1c, 0x01, 0x62, 0x00,
    0x00, 0x00, 0x4a, 0x00, 0x48, 0x50, 0x4d, 0x4b,
    0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42,
    0x43, 0x44, 0x63, 0x64, 0x65, 0x66, 0x4e, 0x61,
    0x2a, 0x36, 0x3a, 0x1d, 0x38, 0x38, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static BYTE oldMouseX;
static BYTE oldMouseY;
static BOOL oldButton1;
static BOOL oldButton2;
static BOOL oldMouseSet;

void amiga_mouse_vbl(void)
{
    UWORD data = JOY0DAT;
    BYTE mouseX = (data & 0x00ff);
    BYTE mouseY = (data & 0xff00) >> 8;
    BOOL button1 = (CIAAPRA & 0x40) == 0;
    BOOL button2 = (POTGOR & 0x0400) == 0;

    if (!oldMouseSet)
    {
        POTGO = POTGOR | 0x0500;
    }
    else if (mouseX != oldMouseX
       || mouseY != oldMouseY
       || button1 != oldButton1
       || button2 != oldButton2)
    {
        BYTE packet[3];
        packet[0] = 0xf8;

        if (button1)
            packet[0] |= 0x02;

        if (button2)
            packet[0] |= 0x01;

        packet[1] = mouseX - oldMouseX;
        packet[2] = mouseY - oldMouseY;

        call_mousevec(packet);
    }

    oldMouseX = mouseX;
    oldMouseY = mouseY;
    oldButton1 = button1;
    oldButton2 = button2;
    oldMouseSet = TRUE;
}

/* Date/Time to use when the hardware clock is not set.
 * We use the OS creation date at 00:00:00
 */
#define DEFAULT_DATETIME ((ULONG)os_dosdate << 16)

#define BATTCLOCK ((volatile UBYTE*)0x00dc0000)

#define AMIGA_CLOCK_NONE 0
#define AMIGA_CLOCK_MSM6242B 1
#define AMIGA_CLOCK_RF5C01A 2

static int amiga_clock_type;

static UBYTE read_clock_reg(int reg)
{
    return BATTCLOCK[reg * 4 + 3] & 0x0f;
}

static void write_clock_reg(UBYTE reg, UBYTE val)
{
    BATTCLOCK[reg * 4 + 3] = val;
}

static UBYTE read_clock_bcd(int reg)
{
    return read_clock_reg(reg + 1) * 10 + read_clock_reg(reg);
}

void amiga_clock_init(void)
{
#if DBG_AMIGA
    kprintf("d = %d, e = %d, f = %d\n", read_clock_reg(0xd), read_clock_reg(0xe), read_clock_reg(0xf));
#endif

    if (read_clock_reg(0xf) == 4)
    {
        amiga_clock_type = AMIGA_CLOCK_MSM6242B;
    }
    else
    {
        UBYTE before, test, after;

        before = read_clock_reg(0xd);

        /* Write a value */
        test = before | 0x1;
        write_clock_reg(0xd, test);

        /* If the value is still here, there is a register */
        after = read_clock_reg(0xd);
        if (after == test)
            amiga_clock_type = AMIGA_CLOCK_RF5C01A;
        else
            amiga_clock_type = AMIGA_CLOCK_NONE;

        write_clock_reg(0xd, before);
    }

#if DBG_AMIGA
    kprintf("amiga_clock_type = %d\n", amiga_clock_type);
#endif
}

static UWORD amiga_dogettime(void)
{
    UWORD seconds = read_clock_bcd(0);
    UWORD minutes = read_clock_bcd(2);
    UWORD hours = read_clock_bcd(4);
    UWORD time;

#if DBG_AMIGA
    kprintf("amiga_dogettime() %02d:%02d:%02d\n", hours, minutes, seconds);
#endif

    time = (seconds >> 1)
         | (minutes << 5)
         | (hours << 11);

    return time;
}

static UWORD amiga_dogetdate(void)
{
    UWORD offset = (amiga_clock_type == AMIGA_CLOCK_RF5C01A) ? 1 : 0;
    UWORD days = read_clock_bcd(offset + 6);
    UWORD months = read_clock_bcd(offset + 8);
    UWORD years = read_clock_bcd(offset + 10);
    UWORD date;

#if DBG_CLOCK
    kprintf("amiga_dogetdate() %02d/%02d/%02d\n", years, months, days);
#endif

    date = (days & 0x1F)
        | ((months & 0xF) << 5)
        | ((1900 + years - 1980) << 9);

    return date;
}

ULONG amiga_getdt(void)
{
    if (amiga_clock_type == AMIGA_CLOCK_NONE)
        return DEFAULT_DATETIME;

    return (((ULONG) amiga_dogetdate()) << 16) | amiga_dogettime();
}

#if CONF_WITH_UAE

/******************************************************************************/
/* uaelib stuff                                                               */
/******************************************************************************/

/* Location of the UAE Boot ROM, a.k.a. RTAREA */
#define RTAREA_DEFAULT 0x00f00000
#define RTAREA_BACKUP  0x00ef0000

/* uaelib_demux() is the entry point */
#define UAELIB_DEMUX_OFFSET 0xFF60
typedef ULONG uaelib_demux_t(ULONG fnum, ...);
uaelib_demux_t* uaelib_demux = NULL;

#define has_uaelib (uaelib_demux != NULL)

static ULONG uaelib_GetVersion(void);

#define IS_TRAP(x)(((*(ULONG*)(x)) & 0xf000ffff) == 0xa0004e75)

void amiga_uaelib_init(void)
{
    if (IS_TRAP(RTAREA_DEFAULT + UAELIB_DEMUX_OFFSET))
        uaelib_demux = (uaelib_demux_t*)(RTAREA_DEFAULT + UAELIB_DEMUX_OFFSET);
    else if (IS_TRAP(RTAREA_BACKUP + UAELIB_DEMUX_OFFSET))
        uaelib_demux = (uaelib_demux_t*)(RTAREA_BACKUP + UAELIB_DEMUX_OFFSET);

#if DBG_AMIGA
    if (has_uaelib)
    {
        ULONG version = uaelib_GetVersion();
        kprintf("EmuTOS running on UAE version %d.%d.%d\n",
            (int)((version & 0xff000000) >> 24),
            (int)((version & 0x00ff0000) >> 16),
            (int)(version & 0x0000ffff));
    }
#endif
    UNUSED(uaelib_GetVersion);
}

/* Get UAE version */
static ULONG uaelib_GetVersion(void)
{
    return uaelib_demux(0);
}

/* Write a string prefixed by DBG: to the UAE debug log
 * The final \n will automatically be appended */
static ULONG uaelib_DbgPuts(const char* str)
{
    return uaelib_demux(86, str);
}

/* Exit UAE */
static ULONG uaelib_ExitEmu(void)
{
    return uaelib_demux(13);
}

/******************************************************************************/
/* kprintf() stuff                                                            */
/******************************************************************************/

#define UAE_MAX_DEBUG_LENGTH 255

static char uae_debug_string[UAE_MAX_DEBUG_LENGTH + 1];

/* The only available output function is uaelib_DbgPuts(),
 * so we have to buffer the string until until \n */
void kprintf_outc_uae(int c)
{
    if (!has_uaelib)
        return;

    if (c == '\n')
    {
        /* Output the current string then clear the buffer */
        uaelib_DbgPuts(uae_debug_string);
        uae_debug_string[0] = '\0';
    }
    else
    {
        char* p;

        /* Append the character to the buffer */
        for (p = uae_debug_string; *p; ++p);
        if ((p - uae_debug_string) < UAE_MAX_DEBUG_LENGTH)
        {
            *p++ = (char)c;
            *p = '\0';
        }
    }
}

#endif /* CONF_WITH_UAE */

/******************************************************************************/
/* Shutdown stuff                                                             */
/******************************************************************************/

void amiga_shutdown(void)
{
#if CONF_WITH_UAE
    if (!has_uaelib)
        return;

    uaelib_ExitEmu();
#endif
}

/******************************************************************************/
/* The following code comes from AROS sources.                                */
/* It is covered by the AROS PUBLIC LICENSE (APL) Version 1.1                 */
/******************************************************************************/

/* Compatibility macros *******************************************************/

#if DBG_AMIGA
#define D(x) (x)
#else
#define D(x)
#endif

#define bug kprintf
#define __mayalias MAY_ALIAS

#define AROS_UFCA(atype, aval, reg) \
    ((atype)(aval))

#define AROS_UFC3(ftype, fname, a1, a2, a3) \
    fname(a1, a2, a3)

#define AROS_UFC5(ftype, fname, a1, a2, a3, a4, a5) \
    fname(a1, a2, a3, a4, a5)

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

/* Machine detection **********************************************************/

int has_gayle;

void amiga_machine_detect(void)
{
    /* Do we have at least a 68020? */
    if (mcpu >= 20)
        SysBase->AttnFlags |= AFF_68020;

    /* Do we have a 32-bit address bus? */
    if (MemoryTest((APTR)0x08000000, (APTR)0x08000000, 0x00100000) == 0)
        SysBase->AttnFlags |= AFF_ADDR32;

    /* Do we have Gayle? */
    has_gayle = ReadGayle() > 0;
    D(bug("has_gayle = %d\n", has_gayle));
}

#if CONF_WITH_ALT_RAM

/******************************************************************************/
/* Alt RAM stuff                                                              */
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
struct IntExpansionBase g_eb;
struct IntExpansionBase* const ExpansionBase = &g_eb;

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

/* From arch/m68k-amiga/expansion/readexpansionbyte.c *************************/

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

extern long xmaddalt(long start, long size); /* found in bdos/mem.h */

static void AddMemList(ULONG size, ULONG attributes, LONG pri, APTR base, STRPTR name)
{
    // EmuTOS glue
    xmaddalt((long)base, (long)size);
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

/* Alternate RAM detection ****************************************************/

static void add_slow_ram(void)
{
    APTR address = (APTR)0x00c00000;
    APTR endAddress = has_gayle ? (APTR)0x00d80000 : (APTR)0x00dc0000;
    ULONG size = MemoryTest(address, endAddress, 0x00040000);

    if (size > 0)
    {
        D(bug("Slow RAM detected at %08lx, size %08lx\n", (ULONG)address, size));
        xmaddalt((long)address, (long)size);
    }
}

void amiga_add_alt_ram(void)
{
    /* Add the slowest RAM first to put it at the end of the Alt RAM pool */
    add_slow_ram();

    /* Configure Zorro II / Zorro III boards and find the Alt RAM */
    ExpansionInit(ExpansionBase);
}

#endif /* CONF_WITH_ALT_RAM */

#endif /* MACHINE_AMIGA */
