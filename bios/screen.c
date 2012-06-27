/*
 * screen.c - low-level screen routines
 *
 * Copyright (c) 2001-2011 The EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  THH   Thomas Huth
 *  LVL   Laurent Vogel
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"  
#include "machine.h"
#include "screen.h"
#include "videl.h"
#include "asm.h"
#include "tosvars.h"
#include "lineavars.h"
#include "nvram.h"
#include "kprint.h"
#include "font.h"
#include "vt52.h"
#include "xbiosbind.h"
#include "vectors.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

#define DBG_SCREEN 0

static unsigned long initial_vram_size();
static unsigned long vram_size();
static void setphys(LONG addr,int checkaddr);

#if CONF_WITH_SHIFTER

/* Define palette */

static const WORD dflt_palette[] = {
    RGB_WHITE, RGB_RED, RGB_GREEN, RGB_YELLOW,
    RGB_BLUE, RGB_MAGENTA, RGB_CYAN, RGB_LTGRAY,
    RGB_GRAY, RGB_LTRED, RGB_LTGREEN, RGB_LTYELLOW,
    RGB_LTBLUE, RGB_LTMAGENTA, RGB_LTCYAN, RGB_BLACK
};

#if CONF_WITH_TT_SHIFTER
static const WORD tt_dflt_palette[] = {
 TTRGB_WHITE, TTRGB_RED, TTRGB_GREEN, TTRGB_YELLOW,
 TTRGB_BLUE, TTRGB_MAGENTA, TTRGB_CYAN, TTRGB_LTGRAY,
 TTRGB_GRAY, TTRGB_LTRED, TTRGB_LTGREEN, TTRGB_LTYELLOW,
 TTRGB_LTBLUE, TTRGB_LTMAGENTA, TTRGB_LTCYAN, TTRGB_BLACK,
 0x0fff, 0x0eee, 0x0ddd, 0x0ccc, 0x0bbb, 0x0aaa, 0x0999, 0x0888,
 0x0777, 0x0666, 0x0555, 0x0444, 0x0333, 0x0222, 0x0111, 0x0000,
 0x0f00, 0x0f01, 0x0f02, 0x0f03, 0x0f04, 0x0f05, 0x0f06, 0x0f07,
 0x0f08, 0x0f09, 0x0f0a, 0x0f0b, 0x0f0c, 0x0f0d, 0x0f0e, 0x0f0f,
 0x0e0f, 0x0d0f, 0x0c0f, 0x0b0f, 0x0a0f, 0x090f, 0x080f, 0x070f,
 0x060f, 0x050f, 0x040f, 0x030f, 0x020f, 0x010f, 0x000f, 0x001f,
 0x002f, 0x003f, 0x004f, 0x005f, 0x006f, 0x007f, 0x008f, 0x009f,
 0x00af, 0x00bf, 0x00cf, 0x00df, 0x00ef, 0x00ff, 0x00fe, 0x00fd,
 0x00fc, 0x00fb, 0x00fa, 0x00f9, 0x00f8, 0x00f7, 0x00f6, 0x00f5,
 0x00f4, 0x00f3, 0x00f2, 0x00f1, 0x00f0, 0x01f0, 0x02f0, 0x03f0,
 0x04f0, 0x05f0, 0x06f0, 0x07f0, 0x08f0, 0x09f0, 0x0af0, 0x0bf0,
 0x0cf0, 0x0df0, 0x0ef0, 0x0ff0, 0x0fe0, 0x0fd0, 0x0fc0, 0x0fb0,
 0x0fa0, 0x0f90, 0x0f80, 0x0f70, 0x0f60, 0x0f50, 0x0f40, 0x0f30,
 0x0f20, 0x0f10, 0x0b00, 0x0b01, 0x0b02, 0x0b03, 0x0b04, 0x0b05,
 0x0b06, 0x0b07, 0x0b08, 0x0b09, 0x0b0a, 0x0b0b, 0x0a0b, 0x090b,
 0x080b, 0x070b, 0x060b, 0x050b, 0x040b, 0x030b, 0x020b, 0x010b,
 0x000b, 0x001b, 0x002b, 0x003b, 0x004b, 0x005b, 0x006b, 0x007b,
 0x008b, 0x009b, 0x00ab, 0x00bb, 0x00ba, 0x00b9, 0x00b8, 0x00b7,
 0x00b6, 0x00b5, 0x00b4, 0x00b3, 0x00b2, 0x00b1, 0x00b0, 0x01b0,
 0x02b0, 0x03b0, 0x04b0, 0x05b0, 0x06b0, 0x07b0, 0x08b0, 0x09b0,
 0x0ab0, 0x0bb0, 0x0ba0, 0x0b90, 0x0b80, 0x0b70, 0x0b60, 0x0b50,
 0x0b40, 0x0b30, 0x0b20, 0x0b10, 0x0700, 0x0701, 0x0702, 0x0703,
 0x0704, 0x0705, 0x0706, 0x0707, 0x0607, 0x0507, 0x0407, 0x0307,
 0x0207, 0x0107, 0x0007, 0x0017, 0x0027, 0x0037, 0x0047, 0x0057,
 0x0067, 0x0077, 0x0076, 0x0075, 0x0074, 0x0073, 0x0072, 0x0071,
 0x0070, 0x0170, 0x0270, 0x0370, 0x0470, 0x0570, 0x0670, 0x0770,
 0x0760, 0x0750, 0x0740, 0x0730, 0x0720, 0x0710, 0x0400, 0x0401,
 0x0402, 0x0403, 0x0404, 0x0304, 0x0204, 0x0104, 0x0004, 0x0014,
 0x0024, 0x0034, 0x0044, 0x0043, 0x0042, 0x0041, 0x0040, 0x0140,
 0x0240, 0x0340, 0x0440, 0x0430, 0x0420, 0x0410, TTRGB_WHITE, TTRGB_BLACK
};
#endif

/* get monitor type (same encoding as VgetMonitor()) */
WORD get_monitor_type(void)
{
#if CONF_WITH_VIDEL
    if (has_videl) {
        return vmontype();
    }
    else
#endif
    {
        volatile UBYTE *gpip = (UBYTE *)0xfffffa01;

        if (*gpip & 0x80)       /* colour monitor */
            return 1;

        return 0;               /* monochrome monitor */
    }
}


/* xbios routines */

#if CONF_WITH_TT_SHIFTER

/*
 * TT shifter functions
 */

/*
 * Set TT shifter mode
 */
WORD esetshift(WORD mode)
{
    volatile WORD *resreg = (WORD *)TT_SHIFTER;
    WORD oldmode;

    if (!has_tt_shifter)
        return -32;

    oldmode = *resreg & TT_SHIFTER_BITMASK;
    *resreg = mode & TT_SHIFTER_BITMASK;

    return oldmode;
}


/*
 * Get TT shifter mode
 */
WORD egetshift(void)
{
    if (!has_tt_shifter)
        return -32;

    return *(volatile WORD *)TT_SHIFTER & TT_SHIFTER_BITMASK;
}


/*
 * Read/modify TT shifter colour bank number
 */
WORD esetbank(WORD bank)
{
    volatile UBYTE *shiftreg = (UBYTE *)(TT_SHIFTER+1);
    UBYTE old;

    if (!has_tt_shifter)
        return -32;

    old = *shiftreg & 0x0f;
    if (bank >= 0)
        *shiftreg = bank & 0x0f;

    return old;
}


/*
 * Read/modify TT palette colour entry
 */
WORD esetcolor(WORD index,WORD color)
{
    volatile WORD *ttcol_regs = (WORD *) TT_PALETTE_REGS;
    WORD oldcolor;

    if (!has_tt_shifter)
        return -32;

    index &= 0xff;                  /* force valid index number */
    oldcolor = ttcol_regs[index] & TT_PALETTE_BITMASK;
    if (color >= 0)
        ttcol_regs[index] = color & TT_PALETTE_BITMASK;

    return oldcolor;
}


/*
 * Set multiple TT palette colour registers
 */
void esetpalette(WORD index,WORD count,WORD *rgb)
{
    volatile WORD *ttcolour;

    if (!has_tt_shifter)
        return;

    index &= 0xff;              /* force valid index number */

    if ((index+count) > 256)
        count = 256 - index;    /* force valid count */

    ttcolour = (WORD *)TT_PALETTE_REGS + index;
    while(count--)
        *ttcolour++ = *rgb++ & TT_PALETTE_BITMASK;
}


/*
 * Get multiple TT palette colour registers
 */
void egetpalette(WORD index,WORD count,WORD *rgb)
{
    volatile WORD *ttcolour;

    if (!has_tt_shifter)
        return;

    index &= 0xff;              /* force valid index number */

    if ((index+count) > 256)
        count = 256 - index;    /* force valid count */

    ttcolour = (WORD *)TT_PALETTE_REGS + index;
    while(count--)
        *rgb++ = *ttcolour++ & TT_PALETTE_BITMASK;
}


/*
 * Read/modify TT shifter grey mode bit
 */
WORD esetgray(WORD mode)
{
    volatile UBYTE *shiftreg = (UBYTE *)TT_SHIFTER;
    UBYTE old;

    if (!has_tt_shifter)
        return -32;

    old = *shiftreg;
    if (mode > 0)
        *shiftreg = old | 0x10;
    else if (mode == 0)
        *shiftreg = old & 0xef;

    return (old&0x10)?1:0;
}


/*
 * Read/modify TT shifter smear mode bit
 */
WORD esetsmear(WORD mode)
{
    volatile UBYTE *shiftreg = (UBYTE *)TT_SHIFTER;
    UBYTE old;

    if (!has_tt_shifter)
        return -32;

    old = *shiftreg;
    if (mode > 0)
        *shiftreg = old | 0x80;
    else if (mode == 0)
        *shiftreg = old & 0x7f;

    return (old&0x80)?1:0;
}

#endif /* CONF_WITH_TT_SHIFTER */

/*
 * Initialise ST(e) palette registers
 */
static void initialise_ste_palette(WORD mask)
{
    volatile WORD *col_regs = (WORD *) ST_PALETTE_REGS;
    int i;

    for (i = 0; i < 16; i++)
        col_regs[i] = dflt_palette[i] & mask;
}

/*
 * Fixup ST(e) palette registers
 */
static void fixup_ste_palette(WORD rez)
{
    volatile WORD *col_regs = (WORD *) ST_PALETTE_REGS;

    if (rez == ST_MEDIUM)
        col_regs[3] = col_regs[15];
    else if (rez == ST_HIGH)
        col_regs[1] = col_regs[15];
}

#if CONF_WITH_TT_SHIFTER
/*
 * Initialise TT palette
 */
static void initialise_tt_palette(WORD rez)
{
    volatile WORD *col_regs = (WORD *) ST_PALETTE_REGS;
    volatile WORD *ttcol_regs = (WORD *) TT_PALETTE_REGS;
    int i;

    for (i = 0; i < 256; i++)
        ttcol_regs[i] = tt_dflt_palette[i];

    if (rez == TT_HIGH) {
        col_regs[1] = col_regs[15];
        ttcol_regs[1] = ttcol_regs[15];
    }
}
#endif

#endif

/*
 * Check specified mode/rez to see if we should change; used in early
 * emudesk.inf processing.  We only indicate a change if all of the
 * following are true:
 *  . the resolution is changeable
 *  . the specified & current values are the same type (both modes
 *    or both rezs)
 *  . the specified value differs from the current value
 * If these are all true, we return the new mode/rez; otherwise we
 * return zero.
 *
 * Mode/rez values are encoded as follows:
 *      0xFFnn: ST/TT resolution nn
 *      otherwise, Falcon mode value
 */
WORD check_moderez(WORD moderez)
{
#if CONF_WITH_SHIFTER
WORD return_rez;
int tt;

    if (!rez_changeable())
        return 0;

#if CONF_WITH_VIDEL
    if (has_videl) {
        WORD current_mode, return_mode;

        current_mode = get_videl_mode();
        if (current_mode) {
            if (moderez < 0)                /* ignore rez values */
                return 0;
            return_mode = vfixmode(moderez);/* adjust */
            return (return_mode==current_mode)?0:return_mode;
        }
    }
#endif

    /* handle old-fashioned rez :-) */
    if (moderez > 0)                        /* ignore mode values */
        return 0;

    tt = 0;
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        tt = 1;
#endif

    return_rez = moderez & 0x00ff;
    if (tt) {
        if (return_rez == TT_HIGH)
            return_rez = TT_MEDIUM;
    } else {
        if (return_rez > ST_MEDIUM)
            return_rez = ST_MEDIUM;
    }

    return (return_rez==getrez())?0:(0xff00|return_rez);
#else /* CONF_WITH_SHIFTER */
    return 0;
#endif /* CONF_WITH_SHIFTER */
}

/*
 * Initialise palette registers
 * This routine is also used by resolution change
 */
void initialise_palette_registers(WORD rez,WORD mode)
{
#if CONF_WITH_SHIFTER
WORD mask;

#if CONF_WITH_VIDEL
    if (has_videl)
        mask = 0x0fff;
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        mask = 0x0fff;
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter)
        mask = 0x0fff;
    else
#endif
    {
        mask = 0x0777;
    }
    initialise_ste_palette(mask);

#if CONF_WITH_VIDEL
    if (has_videl)
        initialise_falcon_palette(mode);
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        initialise_tt_palette(rez);
    else
#endif
    {
    }

    fixup_ste_palette(rez);
#endif
}

static char rez_was_hacked;

/*
 * In the original TOS there used to be an early screen init, 
 * before memory configuration. This is not used here, and all is
 * done at the same time from C.
 */

void screen_init(void)
{
    ULONG screen_start;
#if CONF_WITH_SHIFTER
    volatile BYTE *rez_reg = (BYTE *) ST_SHIFTER;
#if CONF_WITH_VIDEL
    UWORD boot_resolution = FALCON_DEFAULT_BOOT;
#endif
#if CONF_WITH_TT_SHIFTER
    volatile BYTE *ttrez_reg = (BYTE *) TT_SHIFTER;
#endif
    WORD monitor_type, sync_mode;
    WORD rez = 0;   /* avoid 'may be uninitialized' warning */

/*
 * first, see what we're connected to, and set the
 * resolution / video mode appropriately
 */
    monitor_type = get_monitor_type();
#if DBG_SCREEN
    kprintf("monitor_type = %d\n", monitor_type);
#endif

#if CONF_WITH_VIDEL
    if (has_videl) {
        WORD ret;

        UNUSED(ret);

        /* reset VIDEL on boot-up */
        /* first set the physbase to a safe memory */
#if CONF_VRAM_ADDRESS
        setphys(CONF_VRAM_ADDRESS,0);
#else
        setphys(0x10000L,0);
#endif

#if CONF_WITH_NVRAM && !defined(MACHINE_FIREBEE)
        /* get boot resolution from NVRAM */
        ret = nvmaccess(0, 14, 2, (PTR)&boot_resolution);
        if (ret != 0) {
#if DBG_SCREEN
            kprintf("Invalid NVRAM, defaulting to boot video mode 0x%04x\n", boot_resolution);
#endif
        }
        else {
#if DBG_SCREEN
            kprintf("NVRAM boot video mode is 0x%04x\n", boot_resolution);
#endif
        }
#endif // CONF_WITH_NVRAM

        if (!lookup_videl_mode(boot_resolution,monitor_type)) { /* mode isn't in table */
#if DBG_SCREEN
            kprintf("Invalid video mode 0x%04x changed to 0x%04x\n",
                    boot_resolution,FALCON_DEFAULT_BOOT);
#endif
            boot_resolution = FALCON_DEFAULT_BOOT;  /* so pick one that is */
        }

        if (!VALID_VDI_BPP(boot_resolution)) {      /* mustn't confuse VDI */
#if DBG_SCREEN
            kprintf("VDI doesn't support video mode 0x%04x, changed to 0x%04x\n",
                    boot_resolution,FALCON_DEFAULT_BOOT);
#endif
            boot_resolution = FALCON_DEFAULT_BOOT;  /* so use default */
        }

        /* initialise the current video mode, for vfixmode()/vsetmode() */
        current_video_mode = boot_resolution;

        /* fix the video mode according to the actual monitor */
        boot_resolution = vfixmode(boot_resolution);
#if DBG_SCREEN
        kprintf("Fixed boot video mode is 0x%04x\n", boot_resolution);
#endif
        vsetmode(boot_resolution);
        rez = FALCON_REZ;   /* fake value indicates Falcon/Videl */
    }
    else
#endif // CONF_WITH_VIDEL
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        rez = monitor_type?TT_MEDIUM:TT_HIGH;
        *ttrez_reg = rez;
    }
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter) {
        rez = monitor_type?ST_LOW:ST_HIGH;
        *rez_reg = rez;
    }
    else
#endif
    {
        rez = monitor_type?ST_LOW:ST_HIGH;
        *rez_reg = rez;
    }

#if CONF_WITH_VIDEL
    if (rez == FALCON_REZ) {    /* detected a Falcon */
        sync_mode = (boot_resolution&VIDEL_PAL)?0x02:0x00;
    }
    else
#endif
    {
        sync_mode = (os_conf&0x01)?0x02:0x00;
    }
    *(volatile BYTE *) SYNCMODE = sync_mode;

/*
 * next, set up the palette(s)
 */
#if CONF_WITH_VIDEL
    initialise_palette_registers(rez,boot_resolution);
#else
    initialise_palette_registers(rez,0);
#endif
    sshiftmod = rez;

#endif /* CONF_WITH_SHIFTER */

#if CONF_VRAM_ADDRESS
    screen_start = CONF_VRAM_ADDRESS;
#else
    /* videoram is placed just below the phystop */
    screen_start = (ULONG)phystop - initial_vram_size();
    /* round down to 256 byte boundary */
    screen_start &= 0x00ffff00;
    /* Original TOS leaves a gap of 768 bytes between screen ram and phys_top...
     * ... we normally don't need that, but some old software relies on that fact,
     * so we use this gap, too. */
#if CONF_WITH_VIDEL
    if (has_videl)
        ;
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        ;
    else
#endif
    {
        screen_start -= 0x300;
    }
#endif /* CONF_VRAM_ADDRESS */
    /* set new v_bas_ad */
    v_bas_ad = (UBYTE *)screen_start;
#ifdef MACHINE_AMIGA
    amiga_screen_init();
#endif
    /* correct physical address */
    setphys(screen_start,1);
    rez_was_hacked = FALSE; /* initial assumption */

    /* interrupt handlers (interrupts will be enabler later) */
#if CONF_WITH_SHIFTER
    VEC_HBL = int_hbl;
    VEC_VBL = int_vbl;
#endif
}

/*
 * Mark resolution as hacked
 * 
 * called by bios_init() if a cartridge application has altered key
 * lineA variables
 */
void set_rez_hacked(void)
{
    rez_was_hacked = TRUE;
}

/*
 * Check if resolution can be changed
 *
 * returns 1 iff TRUE
 */
int rez_changeable(void)
{
#if CONF_WITH_SHIFTER
int rez;

    if (rez_was_hacked)
        return FALSE;

#if CONF_WITH_VIDEL
    if (has_videl)      /* can't change if real ST monochrome monitor */
        return VgetMonitor() ? TRUE : FALSE;
#endif

    rez = Getrez();

#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter)
        return (rez != TT_HIGH)? TRUE : FALSE;
#endif

    if (rez != ST_HIGH) /* can't change if mono monitor */
        return TRUE;
#endif
    return FALSE;
}

/* calculate initial VRAM size based on video hardware */
static unsigned long initial_vram_size()
{
#if CONF_WITH_VIDEL
    if (has_videl) {
        return FALCON_VRAM_SIZE;
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        return TT_VRAM_SIZE;
    }
    else
#endif
    return ST_VRAM_SIZE;
}

/* calculate VRAM size for current video mode */
static unsigned long vram_size()
{
#if CONF_WITH_VIDEL
    if (has_videl) {
        return get_videl_width() / 8L * get_videl_height() * get_videl_bpp();
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        WORD rez = getrez();
        if (rez >= 4)
            return TT_VRAM_SIZE;
        return ST_VRAM_SIZE;
    }
    else
#endif
    return ST_VRAM_SIZE;
}

/* hardware independant xbios routines */

LONG physbase(void)
{
    LONG addr;

#if CONF_WITH_SHIFTER
    addr = *(volatile UBYTE *) VIDEOBASE_ADDR_HI;
    addr <<= 8;
    addr += *(volatile UBYTE *) VIDEOBASE_ADDR_MID;
    addr <<= 8;
#if CONF_WITH_VIDEL
    if (has_videl) {
        addr += *(volatile UBYTE *) VIDEOBASE_ADDR_LOW;
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        addr += *(volatile UBYTE *) VIDEOBASE_ADDR_LOW;
    }
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter) {
        addr += *(volatile UBYTE *) VIDEOBASE_ADDR_LOW;
    }
#endif
#else /* CONF_WITH_SHIFTER */
    /* No real physical screen, fall back to Logbase() */
    addr = logbase();
#endif /* #if CONF_WITH_SHIFTER */

    return (addr);
}

/* Set physical screen address */

static void setphys(LONG addr,int checkaddr)
{
    if (checkaddr) {
        if (addr > ((ULONG)phystop - vram_size())) {
            panic("VideoRAM covers ROM area!!\n");
        }
    }

#if CONF_WITH_SHIFTER
    *(volatile UBYTE *) VIDEOBASE_ADDR_HI = ((ULONG) addr) >> 16;
    *(volatile UBYTE *) VIDEOBASE_ADDR_MID = ((ULONG) addr) >> 8;
#if CONF_WITH_VIDEL
    if (has_videl) {
        *(volatile UBYTE *) VIDEOBASE_ADDR_LOW = ((ULONG) addr);
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        *(volatile UBYTE *) VIDEOBASE_ADDR_LOW = ((ULONG) addr);
    }
    else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter) {
        *(volatile UBYTE *) VIDEOBASE_ADDR_LOW = ((ULONG) addr);
    }
#endif
#endif /* CONF_WITH_SHIFTER */
}

LONG logbase(void)
{
    return ((ULONG) v_bas_ad);
}

WORD getrez(void)
{
    UBYTE rez;

#if CONF_WITH_SHIFTER
#if CONF_WITH_VIDEL
    if (has_videl) {
        /* Get the video mode for Falcon-hardware */
        WORD vmode = vsetmode(-1);
        if (vmode & VIDEL_COMPAT) {
            switch(vmode&VIDEL_BPPMASK) {
            case VIDEL_1BPP:
                rez = 2;
                break;
            case VIDEL_2BPP:
                rez = 1;
                break;
            case VIDEL_4BPP:
                rez = 0;
                break;
            default:
                kprintf("Problem - unsupported video mode\n");
                rez = 0;
            }
        } else
            rez = 2;
    }
    else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        /* Get the video mode for TT-hardware */
        rez = *(volatile UBYTE *)TT_SHIFTER & 0x07;
    }
    else
#endif
    {
        rez = *(volatile UBYTE *)ST_SHIFTER & 0x03;
    }
#else /* CONF_WITH_SHIFTER */
    /* No video hardware, return the logical video mode */
    rez = sshiftmod;
#endif /* CONF_WITH_SHIFTER */

    return rez;
}


void setscreen(LONG logLoc, LONG physLoc, WORD rez, WORD videlmode)
{
    if (logLoc >= 0) {
        v_bas_ad = (UBYTE *) logLoc;
    }
    if (physLoc >= 0) {
        setphys(physLoc,1);
    }
    if (rez >= 0 && rez < 8) {
        if (FALSE) {
            /* Dummy case for conditional compilation */
        }
#if CONF_WITH_VIDEL
        else if (has_videl) {
            if (rez == FALCON_REZ) {
                vsetmode(videlmode);
                sshiftmod = rez;
            } else if (rez < 3) {   /* ST compatible resolution */
                *(volatile UWORD *)SPSHIFT = 0;
                *(volatile UBYTE *)ST_SHIFTER = sshiftmod = rez;
            }
        }
#endif
#if CONF_WITH_TT_SHIFTER
        else if (has_tt_shifter) {
            if ((rez != 3) && (rez != 5))
                *(volatile UBYTE *)TT_SHIFTER = sshiftmod = rez;
        }
#endif
#if CONF_WITH_SHIFTER
        else if (rez < 3) {      /* ST resolution */
            *(volatile UBYTE *)ST_SHIFTER = sshiftmod = rez;
        }
#endif

        /* Re-initialize line-a, VT52 etc: */
        linea_init();
        font_set_default();
        vt52_init();
    }
}

void setpalette(LONG palettePtr)
{
#if DBG_SCREEN
    int i, max;
    WORD *p = (WORD *)palettePtr;
    max = getrez() == 0 ? 15 : getrez() == 1 ? 3 : 1;
    kprintf("Setpalette(");
    for(i = 0 ; i <= max ; i++) {
        kprintf("%03x", p[i]);
        if(i < 15) kprintf(" ");
    }
    kprintf(")\n");
#endif
    /* next VBL will do this */
    colorptr = (WORD *) palettePtr;
}

WORD setcolor(WORD colorNum, WORD color)
{
    WORD oldcolor;
#if CONF_WITH_SHIFTER
    WORD mask;
    volatile WORD *palette = (WORD *) ST_PALETTE_REGS;
#if CONF_WITH_TT_SHIFTER
    volatile WORD *ttpalette = (WORD *) TT_PALETTE_REGS;
#endif

#if DBG_SCREEN
    kprintf("Setcolor(0x%04x, 0x%04x)\n", colorNum, color);
#endif

    colorNum &= 0x000f;         /* just like real TOS */

#if CONF_WITH_VIDEL
    if (has_videl) {
        mask = 0xfff;
    } else
#endif
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        mask = 0xfff;
    } else
#endif
#if CONF_WITH_STE_SHIFTER
    if (has_ste_shifter) {
        mask = 0xfff;
    } else
#endif
    {
        mask = 0x777;
    }

    oldcolor = palette[colorNum] & mask;
    if (color == -1)
        return oldcolor;

#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        WORD msb, lsb;
        msb = (color << 1) & 0x0eee;    /* move most significant bits to left */
        lsb = (color >> 3) & 0x0111;    /* move least significant bit to right */
        ttpalette[colorNum] = msb | lsb;/* update TT-compatible palette */
    }
#endif

    palette[colorNum] = color;          /* update ST(e)-compatible palette */
#else /* CONF_WITH_SHIFTER */
    /* No hardware, fake value */
    oldcolor = 0;
#endif /* CONF_WITH_SHIFTER */
    return oldcolor;
}


void vsync(void)
{
    LONG a;
#if CONF_WITH_SHIFTER
    WORD old_sr = set_sr(0x2300);       /* allow VBL interrupt */
#endif

    a = frclock;
    while (frclock == a) {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        stop_until_interrupt();
#endif
        /* Wait */
    }

#if CONF_WITH_SHIFTER
    set_sr(old_sr);
#endif /* CONF_WITH_SHIFTER */
}
