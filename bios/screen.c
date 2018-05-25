/*
 * screen.c - low-level screen routines
 *
 * Copyright (C) 2001-2017 The EmuTOS development team
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

/*#define ENABLE_KDEBUG*/

#include "config.h"
#include "portab.h"
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
#include "country.h"
#include "header.h"
#include "biosmem.h"
#ifdef MACHINE_AMIGA
#include "amiga.h"
#endif

static ULONG initial_vram_size(void);
static void setphys(const UBYTE *addr);

#if CONF_WITH_ATARI_VIDEO

/* Define palette */

static const UWORD dflt_palette[] = {
    RGB_WHITE, RGB_RED, RGB_GREEN, RGB_YELLOW,
    RGB_BLUE, RGB_MAGENTA, RGB_CYAN, RGB_LTGRAY,
    RGB_GRAY, RGB_LTRED, RGB_LTGREEN, RGB_LTYELLOW,
    RGB_LTBLUE, RGB_LTMAGENTA, RGB_LTCYAN, RGB_BLACK
};

/*
 * Initialise ST(e) palette registers
 */
static void initialise_ste_palette(UWORD mask)
{
    volatile UWORD *col_regs = (UWORD *) ST_PALETTE_REGS;
    int i;

    for (i = 0; i < 16; i++)
        col_regs[i] = dflt_palette[i] & mask;
}

/*
 * Fixup ST(e) palette registers
 */
static void fixup_ste_palette(WORD rez)
{
    volatile UWORD *col_regs = (UWORD *) ST_PALETTE_REGS;

    if (rez == ST_MEDIUM)
        col_regs[3] = col_regs[15];
    else if (rez == ST_HIGH)
        col_regs[1] = col_regs[15];
}

static WORD shifter_check_moderez(WORD moderez)
{
    WORD return_rez;

    /* handle old-fashioned rez :-) */
    if (moderez > 0)                        /* ignore mode values */
        return 0;

    return_rez = moderez & 0x00ff;

    if (HAS_TT_SHIFTER) {
        if (return_rez == TT_HIGH)
            return_rez = TT_MEDIUM;
    } else {
        if (return_rez > ST_MEDIUM)
            return_rez = ST_MEDIUM;
    }

    return (return_rez==getrez())?0:(0xff00|return_rez);
}

static int shifter_rez_changeable(void)
{
    int rez = Getrez();

    if (HAS_TT_SHIFTER)
        return (rez != TT_HIGH);

    return (rez != ST_HIGH);    /* can't change if mono monitor */
}

static WORD shifter_get_monitor_type(void)
{
    volatile UBYTE *gpip = (UBYTE *)0xfffffa01;

    if (*gpip & 0x80)
        return MON_COLOR;
    else
        return MON_MONO;
}

#endif /* CONF_WITH_ATARI_VIDEO */

#if CONF_WITH_TT_SHIFTER

static const UWORD tt_dflt_palette[] = {
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

/*
 * TT shifter functions
 */

/* xbios routines */

/*
 * Set TT shifter mode
 */
WORD esetshift(WORD mode)
{
    volatile WORD *resreg = (WORD *)TT_SHIFTER;
    WORD oldmode;

    if (!has_tt_shifter)
        return 0x50;    /* unimplemented xbios call: return function # */

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
        return 0x51;    /* unimplemented xbios call: return function # */

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
        return 0x52;    /* unimplemented xbios call: return function # */

    old = *shiftreg & 0x0f;
    if (bank >= 0)
        *shiftreg = bank & 0x0f;

    return old;
}


/*
 * Read/modify TT palette colour entry
 */
WORD esetcolor(WORD index,UWORD color)
{
    UWORD oldcolor;

    if (!has_tt_shifter)
        return 0x53;    /* unimplemented xbios call: return function # */

    index &= 0xff;                  /* force valid index number */
    oldcolor = TT_PALETTE_REGS[index] & TT_PALETTE_BITMASK;
    if ((WORD)color >= 0)
        TT_PALETTE_REGS[index] = color & TT_PALETTE_BITMASK;

    return oldcolor;
}


/*
 * Set multiple TT palette colour registers
 *
 * This function is defined by Atari to return void; however, if the TT
 * shifter is not present, it should return the function number in a WORD,
 * which is the de facto TOS standard for unimplemented xbios functions.
 * Therefore internally we make it return a WORD.
 */
WORD esetpalette(WORD index,WORD count,UWORD *rgb)
{
    volatile UWORD *ttcolour;

    if (!has_tt_shifter)
        return 0x54;    /* unimplemented xbios call: return function # */

    index &= 0xff;              /* force valid index number */

    if ((index+count) > 256)
        count = 256 - index;    /* force valid count */

    ttcolour = &TT_PALETTE_REGS[index];
    while(count--)
        *ttcolour++ = *rgb++ & TT_PALETTE_BITMASK;

    return 0;
}


/*
 * Get multiple TT palette colour registers
 *
 * See the comments for esetpalette() above
 */
WORD egetpalette(WORD index,WORD count,UWORD *rgb)
{
    volatile UWORD *ttcolour;

    if (!has_tt_shifter)
        return 0x55;    /* unimplemented xbios call: return function # */

    index &= 0xff;              /* force valid index number */

    if ((index+count) > 256)
        count = 256 - index;    /* force valid count */

    ttcolour = &TT_PALETTE_REGS[index];
    while(count--)
        *rgb++ = *ttcolour++ & TT_PALETTE_BITMASK;

    return 0;
}


/*
 * Read/modify TT shifter grey mode bit
 */
WORD esetgray(WORD mode)
{
    volatile UBYTE *shiftreg = (UBYTE *)TT_SHIFTER;
    UBYTE old;

    if (!has_tt_shifter)
        return 0x56;    /* unimplemented xbios call: return function # */

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
        return 0x57;    /* unimplemented xbios call: return function # */

    old = *shiftreg;
    if (mode > 0)
        *shiftreg = old | 0x80;
    else if (mode == 0)
        *shiftreg = old & 0x7f;

    return (old&0x80)?1:0;
}

/*
 * Initialise TT palette
 */
static void initialise_tt_palette(WORD rez)
{
    int i;

    for (i = 0; i < 256; i++)
        TT_PALETTE_REGS[i] = tt_dflt_palette[i];

    if (rez == TT_HIGH) {
        /* TT_PALETTE_REGS[1] is updated by h/w */
        TT_PALETTE_REGS[1] = TT_PALETTE_REGS[15];
    }
}

#endif /* CONF_WITH_TT_SHIFTER */

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
    if (!rez_changeable())
        return 0;

#ifdef MACHINE_AMIGA
    return amiga_check_moderez(moderez);
#endif

#if CONF_WITH_VIDEL
    if (has_videl)
        return videl_check_moderez(moderez);
#endif

#if CONF_WITH_ATARI_VIDEO
    return shifter_check_moderez(moderez);
#else
    return 0;
#endif
}

/*
 * Initialise palette registers
 * This routine is also used by resolution change
 */
void initialise_palette_registers(WORD rez,WORD mode)
{
#if CONF_WITH_ATARI_VIDEO
UWORD mask;

    if (HAS_VIDEL || HAS_TT_SHIFTER || HAS_STE_SHIFTER)
        mask = 0x0fff;
    else
        mask = 0x0777;

    initialise_ste_palette(mask);

    if (FALSE) {
        /* Dummy case for conditional compilation */
    }
#if CONF_WITH_VIDEL
    else if (has_videl)
        initialise_falcon_palette(mode);
#endif
#if CONF_WITH_TT_SHIFTER
    else if (has_tt_shifter)
        initialise_tt_palette(rez);
#endif

    fixup_ste_palette(rez);
#endif /* CONF_WITH_ATARI_VIDEO */
}

static char rez_was_hacked;

/* Get the default PAL/NTSC mode according to OS header.
 * Returns TRUE for PAL 50 Hz, or FALSE for NTSC 60 Hz.
 */
static BOOL get_default_palmode(void)
{
    if (os_conf == OS_CONF_MULTILANG)
    {
        /* No country/mode specified in OS header.
         * The mode is inferred from the COUNTRY Makefile variable. */
        return OS_PAL;
    }
    else
    {
        /* Use the mode specified in OS header */
        return os_conf & 0x0001;
    }
}

/*
 * In the original TOS there used to be an early screen init,
 * before memory configuration. This is not used here, and all is
 * done at the same time from C.
 */

/* Initialize the video mode (address will be done later) */
static void screen_init_mode(void)
{
#if CONF_WITH_ATARI_VIDEO
#if CONF_WITH_VIDEL
    UWORD boot_resolution = FALCON_DEFAULT_BOOT;
#endif
    WORD monitor_type, sync_mode;
    WORD rez = 0;   /* avoid 'may be uninitialized' warning */

    /* Initialize the interrupt handlers & the VBL semaphore.
     * It is important to do this first because the initialization code below
     * may call vsync(), which temporarily enables the interrupts. */
    VEC_HBL = int_hbl;
    VEC_VBL = int_vbl;
    vblsem = 0;

/*
 * first, see what we're connected to, and set the
 * resolution / video mode appropriately
 */
    monitor_type = get_monitor_type();
    KDEBUG(("monitor_type = %d\n", monitor_type));

#if CONF_WITH_VIDEL
    if (has_videl) {
        WORD ret;

        MAYBE_UNUSED(ret);

        /* reset VIDEL on boot-up */
        /* first set the physbase to a safe memory */
#if CONF_VRAM_ADDRESS
        setphys((const UBYTE *)CONF_VRAM_ADDRESS);
#else
        setphys((const UBYTE *)0x10000L);
#endif

#if CONF_WITH_NVRAM && !defined(MACHINE_FIREBEE)
        /* This is currently disabled on the FireBee, because the VIDEL is
         * unreliable. Some video modes are not displayed well.
         */
        /* get boot resolution from NVRAM */
        ret = nvmaccess(0, 14, 2, CONST_CAST(UBYTE*, &boot_resolution));
        if (ret != 0) {
            KDEBUG(("Invalid NVRAM, defaulting to boot video mode 0x%04x\n", boot_resolution));
        }
        else {
            KDEBUG(("NVRAM boot video mode is 0x%04x\n", boot_resolution));
        }
#endif /* CONF_WITH_NVRAM */

        if (!lookup_videl_mode(boot_resolution,monitor_type)) { /* mode isn't in table */
            KDEBUG(("Invalid video mode 0x%04x changed to 0x%04x\n",
                    boot_resolution,FALCON_DEFAULT_BOOT));
            boot_resolution = FALCON_DEFAULT_BOOT;  /* so pick one that is */
        }

        if (!VALID_VDI_BPP(boot_resolution)) {      /* mustn't confuse VDI */
            KDEBUG(("VDI doesn't support video mode 0x%04x, changed to 0x%04x\n",
                    boot_resolution,FALCON_DEFAULT_BOOT));
            boot_resolution = FALCON_DEFAULT_BOOT;  /* so use default */
        }

        /* initialise the current video mode, for vfixmode()/vsetmode() */
        current_video_mode = boot_resolution;

        /* fix the video mode according to the actual monitor */
        boot_resolution = vfixmode(boot_resolution);
        KDEBUG(("Fixed boot video mode is 0x%04x\n", boot_resolution));
        vsetmode(boot_resolution);
        rez = FALCON_REZ;   /* fake value indicates Falcon/Videl */
    }
    else
#endif /* CONF_WITH_VIDEL */
#if CONF_WITH_TT_SHIFTER
    if (has_tt_shifter) {
        rez = monitor_type?TT_MEDIUM:TT_HIGH;
        *(volatile BYTE *) TT_SHIFTER = rez;
    }
    else
#endif
    {
        /* On ST, it is important to change the resolution register when nothing
         * is displaying, otherwise the plane shift bug may appear. */
        vsync();

        rez = monitor_type?ST_LOW:ST_HIGH;
        *(volatile BYTE *) ST_SHIFTER = rez;
    }

#if CONF_WITH_VIDEL
    if (rez == FALCON_REZ) {    /* detected a Falcon */
        sync_mode = (boot_resolution&VIDEL_PAL)?0x02:0x00;
    }
    else
#endif
    if (HAS_TT_SHIFTER)
    {
        /* In the "Atari TT030 Hardware Reference Manual" (June 1990),
         * bit 0 of the ST Sync Mode register is noted as 'set to 1'.
         */
        sync_mode = 0x01;
    }
    else
    {
        BOOL palmode = get_default_palmode();
        sync_mode = palmode?0x02:0x00;
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

#endif /* CONF_WITH_ATARI_VIDEO */
    MAYBE_UNUSED(get_default_palmode);
}

/* Initialize the video address (mode is already set) */
static void screen_init_address(void)
{
    ULONG vram_size;
    UBYTE *screen_start;

#if CONF_VRAM_ADDRESS
    UNUSED(vram_size);
    screen_start = (UBYTE *)CONF_VRAM_ADDRESS;
#else
    vram_size = initial_vram_size();
    /* videoram is placed just below the phystop */
    screen_start = balloc_stram(vram_size, TRUE);
#endif /* CONF_VRAM_ADDRESS */

    /* set new v_bas_ad */
    v_bas_ad = screen_start;
    KDEBUG(("v_bas_ad = %p, vram_size = %ld\n", v_bas_ad, vram_size));
#ifdef MACHINE_AMIGA
    amiga_screen_init();
#endif
    /* correct physical address */
    setphys(screen_start);
}

/* Initialize the video mode and screen address */
void screen_init(void)
{
    screen_init_mode();
    screen_init_address();
    rez_was_hacked = FALSE; /* initial assumption */
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
    if (rez_was_hacked)
        return FALSE;

#ifdef MACHINE_AMIGA
    return TRUE;
#endif

#if CONF_WITH_VIDEL
    if (has_videl)  /* can't change if real ST monochrome monitor */
        return (VgetMonitor() != MON_MONO);
#endif

#if CONF_WITH_ATARI_VIDEO
    return shifter_rez_changeable();
#else
    return FALSE;
#endif
}

/* get monitor type (same encoding as VgetMonitor()) */
WORD get_monitor_type(void)
{
#if CONF_WITH_VIDEL
    if (has_videl)
        return vmontype();
#endif

#if CONF_WITH_ATARI_VIDEO
    return shifter_get_monitor_type();
#else
    return MON_MONO;    /* fake monochrome monitor */
#endif
}

/* calculate initial VRAM size based on video hardware */
static ULONG initial_vram_size(void)
{
#ifdef MACHINE_AMIGA
    return amiga_initial_vram_size();
#else
    if (HAS_VIDEL)
        return FALCON_VRAM_SIZE;
    else if (HAS_TT_SHIFTER)
        return TT_VRAM_SIZE;
    else
    {
        /* ST TOS rounds the VRAM size to upper kilobyte, so we do. */
        return (ST_VRAM_SIZE + 1023) & -1024;
    }
#endif
}

/* Settings for the different video modes */
struct video_mode {
    UBYTE       planes;         /* count of color planes (v_planes) */
    UWORD       hz_rez;         /* screen horizontal resolution (v_hz_rez) */
    UWORD       vt_rez;         /* screen vertical resolution (v_vt_rez) */
};

static const struct video_mode video_mode[] = {
    { 4,  320, 200},            /* rez=0: ST low */
    { 2,  640, 200},            /* rez=1: ST medium */
    { 1,  640, 400},            /* rez=2: ST high */
#if CONF_WITH_TT_SHIFTER
    { 0,    0,   0},            /* rez=3: invalid */
    { 4,  640, 480},            /* rez=4: TT medium */
    { 0,    0,  0,},            /* rez=5: invalid */
    { 1, 1280, 960},            /* rez=6: TT high */
    { 8,  320, 480},            /* rez=7: TT low */
#endif
};

static void shifter_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez)
{
    WORD vmode;                         /* video mode */

    vmode = (sshiftmod & 7);            /* Get video mode from copy of hardware */
    KDEBUG(("vmode: %d\n", vmode));

    *planes = video_mode[vmode].planes;
    *hz_rez = video_mode[vmode].hz_rez;
    *vt_rez = video_mode[vmode].vt_rez;
}

static void atari_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez)
{
#if CONF_WITH_VIDEL
    if (has_videl) {
        videl_get_current_mode_info(planes, hz_rez, vt_rez);
    } else
#endif
    {
        shifter_get_current_mode_info(planes, hz_rez, vt_rez);
    }
}

void screen_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez)
{
    MAYBE_UNUSED(atari_get_current_mode_info);

#ifdef MACHINE_AMIGA
    amiga_get_current_mode_info(planes, hz_rez, vt_rez);
#else
    atari_get_current_mode_info(planes, hz_rez, vt_rez);
#endif
}

/* returns 'standard' pixel sizes */
static __inline__ void get_std_pixel_size(WORD *width,WORD *height)
{
    *width = (V_REZ_HZ < 640) ? 556 : 278;  /* magic numbers as used */
    *height = (V_REZ_VT < 400) ? 556 : 278; /*  by TOS 3 & TOS 4     */
}

/*
 * used by vdi_v_opnwk()
 *
 * pixel sizes returned here affect (at least) how the following
 * are displayed:
 *  - the output from v_arc()/v_circle()/vie_pslice()
 *  - the size of gl_wbox in pixels
 */
void get_pixel_size(WORD *width,WORD *height)
{
#ifdef MACHINE_AMIGA
    get_std_pixel_size(width,height);
#else
    if (HAS_VIDEL || HAS_TT_SHIFTER)
        get_std_pixel_size(width,height);
    else
    {
        /* ST TOS has its own set of magic numbers */
        if (V_REZ_VT == 400)        /* ST high */
            *width = 372;
        else if (V_REZ_HZ == 640)   /* ST medium */
            *width = 169;
        else *width = 338;          /* ST low */
        *height = 372;
    }
#endif
}

#if CONF_WITH_ATARI_VIDEO

static const UBYTE *atari_physbase(void)
{
    ULONG addr;

    addr = *(volatile UBYTE *) VIDEOBASE_ADDR_HI;
    addr <<= 8;
    addr |= *(volatile UBYTE *) VIDEOBASE_ADDR_MID;
    addr <<= 8;

    if (HAS_VIDEL || HAS_TT_SHIFTER || HAS_STE_SHIFTER)
        addr |= *(volatile UBYTE *) VIDEOBASE_ADDR_LOW;

    return (UBYTE *)addr;
}

static void atari_setphys(const UBYTE *addr)
{
    *(volatile UBYTE *) VIDEOBASE_ADDR_HI = ((ULONG) addr) >> 16;
    *(volatile UBYTE *) VIDEOBASE_ADDR_MID = ((ULONG) addr) >> 8;

    if (HAS_VIDEL || HAS_TT_SHIFTER || HAS_STE_SHIFTER)
        *(volatile UBYTE *) VIDEOBASE_ADDR_LOW = ((ULONG) addr);
}

static WORD atari_getrez(void)
{
    UBYTE rez;

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

    return rez;
}

static void atari_setrez(WORD rez, WORD videlmode)
{
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
    else if (rez < 3) {         /* ST resolution */
        *(volatile UBYTE *)ST_SHIFTER = sshiftmod = rez;
    }
}

static WORD atari_setcolor(WORD colorNum, WORD color)
{
    WORD oldcolor;

    WORD mask;
    volatile WORD *palette = (WORD *) ST_PALETTE_REGS;

    KDEBUG(("Setcolor(0x%04x, 0x%04x)\n", colorNum, color));

    colorNum &= 0x000f;         /* just like real TOS */

    if (HAS_VIDEL || HAS_TT_SHIFTER || HAS_STE_SHIFTER)
        mask = 0x0fff;
    else
        mask = 0x0777;

    oldcolor = palette[colorNum] & mask;
    if (color >= 0)
        palette[colorNum] = color;  /* update ST(e)-compatible palette */

    return oldcolor;
}

#endif /* CONF_WITH_ATARI_VIDEO */

/* hardware independent xbios routines */

const UBYTE *physbase(void)
{
#ifdef MACHINE_AMIGA
    return amiga_physbase();
#elif CONF_WITH_ATARI_VIDEO
    return atari_physbase();
#else
    /* No real physical screen, fall back to Logbase() */
    return logbase();
#endif
}

/* Set physical screen address */

static void setphys(const UBYTE *addr)
{
    KDEBUG(("setphys(%p)\n", addr));

#ifdef MACHINE_AMIGA
    amiga_setphys(addr);
#elif CONF_WITH_ATARI_VIDEO
    atari_setphys(addr);
#endif
}

UBYTE *logbase(void)
{
    return v_bas_ad;
}

WORD getrez(void)
{
#if CONF_WITH_ATARI_VIDEO
    return atari_getrez();
#else
    /* No video hardware, return the logical video mode */
    return sshiftmod;
#endif
}


/*
 * setscreen(): implement the Setscreen() xbios call
 *
 * implementation details:
 *  . sets the logical screen address, iff logLoc >= 0
 *  . sets the physical screen address, iff physLoc >= 0
 *  . sets the screen resolution iff 0 <= rez <= 7
 *      if a VIDEL is present and rez==3, then the video mode is
 *      set by a call to vsetmode with 'videlmode' as the argument
 *  . *** special EmuTOS-only extension, used by EmuCON ***
 *      if logLoc==physLoc==rez==-1, 'videlmode' is used to select
 *      the cellheight of the default font
 */
void setscreen(UBYTE *logLoc, const UBYTE *physLoc, WORD rez, WORD videlmode)
{
    if (logLoc != (UBYTE *)-1) {
        v_bas_ad = logLoc;
        KDEBUG(("v_bas_ad = %p\n", v_bas_ad));
    }
    if (physLoc != (UBYTE *)-1) {
        setphys(physLoc);
    }
    if (rez >= 0 && rez < 8) {
        /* Wait for the end of display to avoid the plane-shift bug on ST */
        vsync();

#ifdef MACHINE_AMIGA
        amiga_setrez(rez, videlmode);
#elif CONF_WITH_ATARI_VIDEO
        atari_setrez(rez, videlmode);
#endif

        /* Re-initialize line-a, VT52 etc: */
        linea_init();
        font_set_default(-1);
        vt52_init();
    }

    /* handle EmuCON extension */
    if ((logLoc == (UBYTE *)-1) && (physLoc == (UBYTE *)-1) && (rez == -1)) {
        font_set_default(videlmode);
        vt52_init();
    }
}

void setpalette(const UWORD *palettePtr)
{
#ifdef ENABLE_KDEBUG
    int i, max;
    max = getrez() == 0 ? 15 : getrez() == 1 ? 3 : 1;
    KDEBUG(("Setpalette("));
    for(i = 0 ; i <= max ; i++) {
        KDEBUG(("%03x", palettePtr[i]));
        if(i < 15) KDEBUG((" "));
    }
    KDEBUG((")\n"));
#endif
    /* next VBL will do this */
    colorptr = palettePtr;
}

/*
 * setcolor(): implement the Setcolor() xbios call
 *
 * note that this only sets the ST(e)-compatible palette registers.
 * on a TT, the h/w updates the corresponding TT palette registers
 * automagically.
 */
WORD setcolor(WORD colorNum, WORD color)
{
#ifdef MACHINE_AMIGA
    return amiga_setcolor(colorNum, color);
#elif CONF_WITH_ATARI_VIDEO
    return atari_setcolor(colorNum, color);
#else
    /* No hardware, fake return value */
    return 0;
#endif
}


void vsync(void)
{
    LONG a;
#if CONF_WITH_ATARI_VIDEO
    WORD old_sr = set_sr(0x2300);       /* allow VBL interrupt */
    /* Beware: as a side effect, MFP interrupts are also enabled.
     * So the MFP interruptions must be carefully initialized (or disabled)
     * before calling vsync().
     * This is ugly, but Atari TOS does the same.
     */
#endif

    a = frclock;
    while (frclock == a) {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        stop_until_interrupt();
#endif
        /* Wait */
    }

#if CONF_WITH_ATARI_VIDEO
    set_sr(old_sr);
#endif /* CONF_WITH_ATARI_VIDEO */
}
