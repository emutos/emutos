/*
 * dmasound.c - STe/TT/Falcon DMA sound routines
 *
 * Copyright (C) 2011-2022 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *  THH   Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "dmasound.h"
#include "vectors.h"
#include "gemerror.h"
#include "asm.h"
#include "machine.h"

#if CONF_WITH_DMASOUND

struct dmasound
{
    /* STe/TT DMA registers */
    UBYTE interrupt;            /* Buffer interrupts */
    UBYTE control;              /* DMA Control Register */
    UBYTE filler02;
    UBYTE frame_start_high;     /* Frame start address (high byte) */
    UBYTE filler04;
    UBYTE frame_start_mid;      /* Frame start address (mid byte) */
    UBYTE filler06;
    UBYTE frame_start_low;      /* Frame start address (low byte) */
    UBYTE filler08;
    UBYTE frame_counter_high;   /* Frame address counter (high byte) */
    UBYTE filler0a;
    UBYTE frame_counter_mid;    /* Frame address counter (mid byte) */
    UBYTE filler0c;
    UBYTE frame_counter_low;    /* Frame address counter (low byte) */
    UBYTE filler0e;
    UBYTE frame_end_high;       /* Frame end address (high byte) */
    UBYTE filler10;
    UBYTE frame_end_mid;        /* Frame end address (mid byte) */
    UBYTE filler12;
    UBYTE frame_end_low;        /* Frame end address (low byte) */
    UBYTE filler14[12];
    UBYTE track_control;        /* DMA Track Control */
    UBYTE mode_control;         /* Sound mode control */
    /* STe/TT Microwire interface registers */
    UWORD microwire_data;       /* Microwire data register */
    UWORD microwire_mask;       /* Microwire mask register */
    UBYTE filler26[10];
#if !MPS_STE_SOUND_ONLY
    /* Falcon DMA registers */
    UWORD crossbar_src;         /* Crossbar Source Controller */
    UWORD crossbar_dest;        /* Crossbar Destination Controller */
    UBYTE freq_ext;             /* Frequency Divider External Clock */
    UBYTE freq_int;             /* Frequency Divider Internal Sync */
    UBYTE record_tracks;        /* Record Tracks Select */
    UBYTE codec_16bit_source;   /* CODEC Input Source from 16bit adder */
    UBYTE codec_adc_source;     /* CODEC ADC-Input for L+R Channel */
    UBYTE channel_amplification;/* Channel amplification */
    UWORD channel_attenuation;  /* Channel attenuation */
    UWORD codec_status;         /* CODEC-Status */
    UBYTE filler3e[3];
    UBYTE gpx_data_direction;   /* GPx Data Direction */
    UBYTE filler42;
    UBYTE gpx_data_port;        /* GPx Data Port */
#endif
};

#define DMASOUND ((volatile struct dmasound*)0xffff8900)

/* Generic Microwire macros */
#define MICROWIRE_MASK              0x0ffe
#define MICROWIRE_ADDRESS(x)        ((x) << 9)
#define MICROWIRE_COMMAND(a,c)      (MICROWIRE_ADDRESS(a) | (c))

/* LMC1992 macros */
#define LMC1992_MICROWIRE_ADDRESS   2
#define LMC1992_FUNCTION(x)         ((x) << 6)
#define LMC1992_PARAMETER(x)        (x)
#define LMC1992_COMMAND(f,p)        MICROWIRE_COMMAND(LMC1992_MICROWIRE_ADDRESS, LMC1992_FUNCTION(f) | LMC1992_PARAMETER(p))

/* LMC1992 functions */
#define LMC1992_FUNCTION_INPUT_SELECT       0
#define LMC1992_FUNCTION_BASS               1
#define LMC1992_FUNCTION_TREBLE             2
#define LMC1992_FUNCTION_VOLUME             3
#define LMC1992_FUNCTION_RIGHT_FRONT_FADER  4
#define LMC1992_FUNCTION_LEFT_FRONT_FADER   5
#define LMC1992_FUNCTION_RIGHT_REAR_FADER   6   /* these two functions are not available */
#define LMC1992_FUNCTION_LEFT_REAR_FADER    7   /*  on Atari systems                     */

/* LMC1992 parameters for function INPUT_SELECT ("Set Mix" in Atari documentation) */
#define LMC1992_INPUT_OPEN  0                   /* Atari: -12dB */
#define LMC1992_INPUT_1     1                   /* Atari: Mix GI sound chip output */
#define LMC1992_INPUT_2     2                   /* Atari: Do not mix GI sound chip output */
#define LMC1992_INPUT_3     3                   /* Atari: reserved */
#define LMC1992_INPUT_4     4                   /* Atari: undocumented */

/* LMC1992 parameter for Bass and Treble functions */
#define LMC1992_TONE(x)     (((x) + 12) / 2)    /* Range from -12 to +12 dB */

/* LMC1992 parameter for Volume function */
#define LMC1992_VOLUME(x)   (((x) + 80) / 2)    /* Range from -80 to 0 dB */

/* LMC1992 parameter for Fader functions */
#define LMC1992_FADER(x)    (((x) + 40) / 2)    /* Range from -40 to 0 dB */

static int sound_locked;

int has_dmasound;
int has_microwire;
int has_falcon_dmasound;

/*
 * if we're emulating the sound TSR, we just need basic sound;
 * otherwise we need the full Falcon stuff.  we use the following
 * macro to test for this at the beginning of each main function.
 */
#if CONF_WITH_XBIOS_SOUND
#define SOUND_IS_AVAILABLE  (has_dmasound)
#else
#define SOUND_IS_AVAILABLE  (has_falcon_dmasound)
#endif

void detect_dmasound(void)
{
    /* First, detect basic STe/TT DMA sound */
    has_dmasound = check_read_byte((long)&DMASOUND->control);
    KDEBUG(("has_dmasound = %d\n", has_dmasound));

#if !MPS_STE_SOUND_ONLY
    /* Then detect advanced Falcon DMA sound */
    if (has_dmasound)
    {
        /* Then detect advanced Falcon DMA sound */
        /* Warning: The Falcon registers below GPx do not exist on STe/TT
         * but they don't cause a bus error. */
        has_falcon_dmasound = check_read_byte((long)&DMASOUND->gpx_data_port);

        /* Stop all sound */
        DMASOUND->control = 0;
    }
    KDEBUG(("has_falcon_dmasound = %d\n", has_falcon_dmasound));

    /* The Falcon has no microwire interface.
     * This is not detectable through a bus error. */
    has_microwire = has_dmasound && !has_falcon_dmasound;
    KDEBUG(("has_microwire = %d\n", has_microwire));
#else
    has_microwire = 1;
    has_falcon_dmasound = 0;
#endif
}

static void write_microwire(UWORD data)
{
    /*
     * Use LSB as 'stop' bit. It is not shifted out because of MICROWIRE_MASK.
     */
    DMASOUND->microwire_data = (data<<1) | 1;

    /*
     * Wait for data transfer to finish: since the LSB is always 1, the data
     * register can only become 0 after everything has been shifted out.
     */
    while (DMASOUND->microwire_data != 0)
        /* wait */ ;
}

static void lmc1992_init(void)
{
    if (!has_microwire)
        return;

    DMASOUND->microwire_mask = MICROWIRE_MASK;

    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_VOLUME, LMC1992_VOLUME(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_LEFT_FRONT_FADER, LMC1992_FADER(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_RIGHT_FRONT_FADER, LMC1992_FADER(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_TREBLE, LMC1992_TONE(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_BASS, LMC1992_TONE(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_INPUT_SELECT, LMC1992_INPUT_1));
}

static void falcon_dmasound_init(void)
{
#if !MPS_STE_SOUND_ONLY
    /*
     * connect DMA playback to DAC (headphone/speaker)
     * set clock = internal 25.175MHz
     * set TT/STe compatibility mode
     * disable handshaking
     */
    devconnect(0,8,0,0,1);

    setsndmode(0);          /* set 8-bit stereo */
    soundcmd(2,0x0080);     /* set left gain = 8 */
    soundcmd(3,0x0080);     /* set right gain = 8 */
    soundcmd(4,0x0003);     /* set ADDER input to ADC & connection matrix */
    soundcmd(5,0x0003);     /* set L & R channel ADC source to PSG */
    soundcmd(6,0x0003);     /* set TT-compatible prescale to /160 = 50MHz */
#endif
}

void dmasound_init(void)
{
    sound_locked = 0;

    lmc1992_init();

#if !MPS_STE_SOUND_ONLY
    if (has_falcon_dmasound)
        falcon_dmasound_init();
#endif
}


/* *** XBIOS DMA sound functions *** */

/**
 * Lock the XBIOS DMA sound subsystem
 */
LONG locksnd(void)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x80;    /* unimplemented xbios call: return function # */

    if (sound_locked)
        return SNDLOCKED;

    sound_locked = 1;
    return 1;
}

/**
 * Free the XBIOS DMA sound subsystem
 */
LONG unlocksnd(void)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x81;    /* unimplemented xbios call: return function # */

    if (!sound_locked)
        return SNDNOTLOCK;

    sound_locked = 0;
    return 0;
}

#if !MPS_STE_SOUND_ONLY
/**
 * Configure various sound settings of Falcon DMA sound
 */
static LONG sndcmd_falcon(WORD mode, WORD data)
{
    UWORD val;

    switch (mode)
    {
     case 0:                /* LTATTEN */
       val = DMASOUND->channel_attenuation;
       if (data != -1)
       {
           data = (data & 0x00f0) << 4;
           val = (val & 0xf0ff) | data;
           DMASOUND->channel_attenuation = val;
       }
       return (val >> 4) & 0x00f0;
     case 1:                /* RTATTEN */
       val = DMASOUND->channel_attenuation;
       if (data != -1)
       {
           data = data & 0x00f0;
           val = (val & 0xff0f) | data;
           DMASOUND->channel_attenuation = val;
       }
       return val & 0x00f0;
     case 2:                /* LTGAIN */
       val = DMASOUND->channel_amplification;
       if (data != -1)
       {
           data = data & 0xf0;
           val = (val & 0x0f) | data;
           DMASOUND->channel_amplification = val;
       }
       return val & 0x0f0;
     case 3:                /* RTGAIN */
       val = DMASOUND->channel_amplification;
       if (data != -1)
       {
           data = (data & 0xf0) >> 4;
           val = (val & 0xf0) | data;
           DMASOUND->channel_amplification = val;
       }
       return (val & 0x0f) << 4;
     case 4:                /* ADDERIN */
       if (data != -1)
       {
           DMASOUND->codec_16bit_source = data;
       }
       return DMASOUND->codec_16bit_source & 0x3;
     case 5:                /* ADCINPUT */
       if (data != -1)
       {
           DMASOUND->codec_adc_source = data;
       }
       return DMASOUND->codec_adc_source & 0x3;
    }

    return 0;
}
#endif

/**
 * Configure various sound settings of STE DMA sound
 */
static LONG sndcmd_ste(WORD mode, WORD data)
{
    static UWORD ltatten, rtatten;

    switch (mode)
    {
     case 0:                /* LTATTEN */
        if (data != -1)
        {
            ltatten = ((UWORD)data & 0xff)/6;
            if (ltatten > 40)
                ltatten = 40;
            write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_LEFT_FRONT_FADER,
                                            LMC1992_FADER(-ltatten)));
        }
        return ltatten * 6;
     case 1:                /* RTATTEN */
        if (data != -1)
        {
            rtatten = ((UWORD)data & 0xff)/6;
            if (rtatten > 40)
                rtatten = 40;
            write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_RIGHT_FRONT_FADER,
                                            LMC1992_FADER(-rtatten)));
        }
        return rtatten * 6;
    }

    return 0;
}

/**
 * Configure various sound settings
 */
LONG soundcmd(WORD mode, WORD data)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x82;    /* unimplemented xbios call: return function # */

    if (mode == 6)   /* Set STE/TT compatible prescale? */
    {
        UWORD modectrl = DMASOUND->mode_control;
        if (data >= 0 && data <= 3)
        {
            modectrl = (modectrl & 0xFC) | data;
            DMASOUND->mode_control = modectrl;
        }
        return modectrl & 0x3;
    }

#if !MPS_STE_SOUND_ONLY
    if (has_falcon_dmasound)
    {
        return sndcmd_falcon(mode, data);
    }
#endif

    return sndcmd_ste(mode, data);
}

/**
 * Set DMA sound frame buffer pointers
 */
LONG setbuffer(UWORD mode, ULONG startaddr, ULONG endaddr)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x83;    /* unimplemented xbios call: return function # */

    if (mode > 1 || (mode == 1 && !has_falcon_dmasound))
        return EBADRQ;

#if !MPS_STE_SOUND_ONLY
    if (has_falcon_dmasound)
    {
        if (mode == 1)
            DMASOUND->control |= 0x80;  /* Select recording frame registers */
        else
            DMASOUND->control &= 0x7f;  /* Select replay frame registers */
    }
#endif

    /* Set frame start address */
    DMASOUND->frame_start_high = (UBYTE)(startaddr >> 16);
    DMASOUND->frame_start_mid = (UBYTE)(startaddr >> 8);
    DMASOUND->frame_start_low = (UBYTE)startaddr;

    /* Set frame end address */
    DMASOUND->frame_end_high = (UBYTE)(endaddr >> 16);
    DMASOUND->frame_end_mid = (UBYTE)(endaddr >> 8);
    DMASOUND->frame_end_low = (UBYTE)endaddr;

    return 0;
}

/**
 * Set sound mode (stereo/mono, 8-bit/16-bit)
 */
LONG setsndmode(UWORD mode)
{
    UBYTE modectrl;

    if (!SOUND_IS_AVAILABLE)
        return 0x84;    /* unimplemented xbios call: return function # */

    if (mode > 2)
        return EBADRQ;

    modectrl = DMASOUND->mode_control & 0x3f;
    if (mode == 2)
        modectrl |= 0x80;   /* Select mono */
    if (mode == 1)
        modectrl |= 0x40;   /* Select 16-bit mode */
    DMASOUND->mode_control = modectrl;

    return 0;
}

/**
 * Set amount of tracks
 */
LONG settracks(UWORD playtracks, UWORD rectracks)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x85;    /* unimplemented xbios call: return function # */

#if !MPS_STE_SOUND_ONLY
    if (playtracks > 3 || rectracks > 3 || !has_falcon_dmasound)
        return EBADRQ;

    DMASOUND->track_control = (DMASOUND->track_control & 0xfc) | playtracks;
    DMASOUND->record_tracks = rectracks;
#endif

    return 0;
}

/**
 * Set DAC track
 */
LONG setmontracks(UWORD montrack)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x86;    /* unimplemented xbios call: return function # */

#if !MPS_STE_SOUND_ONLY
    return EBADRQ;
#else
    if (montrack > 3 || !has_falcon_dmasound)
        return EBADRQ;

    DMASOUND->track_control = (DMASOUND->track_control&0xcf) | (montrack << 4);

    return 0;
#endif
}

/**
 * Set interrupt mode (Timer-A or MFP-i7)
 */
LONG setinterrupt(UWORD mode, WORD cause)
{
    UBYTE irqreg;

    if (!SOUND_IS_AVAILABLE)
        return 0x87;    /* unimplemented xbios call: return function # */

    if (mode > 1)
        return EBADRQ;

    if (!has_falcon_dmasound)
    {
        /* On STE and TT, interrupts are always enabled, so just return success
         * when "enabling" them for playback, or an error code otherwise */
        if (cause == 1)
            return 0;
        else
            return EBADRQ;
    }

    irqreg = DMASOUND->interrupt;
    cause &= 0x3;

    if (mode == 0)
    {
        irqreg &= 0xF3;
        irqreg |= cause << 2;
    }
    else
    {
        irqreg &= 0xFC;
        irqreg |= cause;
    }

    DMASOUND->interrupt = irqreg;

    return 0;
}

/**
 * Enable/disable frame replay/recording and set looping mode
 */
LONG buffoper(WORD mode)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x88;    /* unimplemented xbios call: return function # */

    if (mode < 0)
    {
        LONG ret;
        UBYTE ctrl = DMASOUND->control;
        ret = ctrl & 0x3;
#if !MPS_STE_SOUND_ONLY
        if (has_falcon_dmasound)
            ret |= ((ctrl >> 2) & 0xc);
#endif
        return ret;
    }

    DMASOUND->control = ((mode & 0xc) << 2) | (mode & 0x3);
    return 0;
}

/**
 * Connect/disconnect the DSP from the sound matrix
 */
LONG dsptristate(WORD dspxmit, WORD dsprec)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x89;    /* unimplemented xbios call: return function # */

#if !MPS_STE_SOUND_ONLY
    if (!has_falcon_dmasound)
#endif
        return EBADRQ;

#if !MPS_STE_SOUND_ONLY
    if (dspxmit)
        DMASOUND->crossbar_src |= 0x0080;
    else
        DMASOUND->crossbar_src &= ~0x0080;

    if (dsprec)
        DMASOUND->crossbar_dest |= 0x0080;
    else
        DMASOUND->crossbar_dest &= ~0x0080;

    return 0;
#endif
}

/**
 * Set/get the GPIO pins of the DSP connector
 */
LONG gpio(UWORD mode, UWORD data)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x8a;    /* unimplemented xbios call: return function # */

#if MPS_STE_SOUND_ONLY
    return EBADRQ;
#else
    switch (mode)
    {
     case 0:             /* Set direction */
        DMASOUND->gpx_data_direction = data & 0x07;
        break;
     case 1:             /* Read the pins */
        return DMASOUND->gpx_data_port & 0x07;
     case 2:             /* Set output pins */
        DMASOUND->gpx_data_port = data;
        break;
     default:
        return EBADRQ;
    }

    return 0;
#endif
}

#if !MPS_STE_SOUND_ONLY
/**
 * Devconnect for Falcon hardware
 *
 * NOTE: the TOS 4.04 implementation of Devconnect() has a few problems:
 * 1. there is little error checking:
 *      . only 'source' is validated,
 *      . 'clk' is assumed to be 0-2
 *      . 'prescale' is assumed to be < 256
 *      . 'protocol' is assumed to be 1 if not 0
 * 2. the return code is ill-defined:
 *      . zero is returned if 'source' is invalid
 *      . garbage (which could include zero) is returned otherwise
 * 3. if an external clock is specified, the output crossbar_dest
 *    register will apparently be set to garbage.
 *
 * The current code only returns zero for invalid 'source'; otherwise
 * it returns 1.  And it does not put garbage in crossbar_dest if an
 * external clock is specified.  However, I do not know if crossbar_dest
 * is set correctly, having no way of testing this.
 */
static LONG devconnect_falcon(WORD source, WORD dest, WORD clk, WORD prescale, WORD protocol)
{

    UWORD data;

    /*
     * handle source specification
     */
    data = DMASOUND->crossbar_src;
    switch (source)
    {
     case 0:    /* DMA playback */
        data = (data & 0xfff0) | (clk << 1);    /* insert clock */
        if (protocol)
            data |= 0x0001;
        else
        {
            data &= ~0x0001;
            if (dest & 0x0002)      /* DSP receive? */
                data &= ~0x0008;
            else
                data |= 0x0008;
        }
        break;
     case 1:    /* DSP transmit */
        data = (data & 0xff8f) | (clk << 5);    /* do not touch bit 7 (DSP) */
        if (protocol)
            data |= 0x0010;
        else
            data &= ~0x0010;
        break;
     case 2:    /* external input */
        data = (data & 0xf0ff) | (clk << 9);
        if (protocol)
            data |= 0x0100;
        else
            data &= ~0x0100;
        break;
     case 3:    /* microphone / PSG */
        data = (data & 0x0fff);
        if (clk & 1)                /* i.e. external clock */
            data |= 0x6000;
        break;
     default:
        return 0;       /* TOS 4.04 does this */
    }
    DMASOUND->crossbar_src = data;

    /*
     * handle destination(s)
     */
    data = DMASOUND->crossbar_dest;
    if (dest & 0x0001)  /* DMA record */
    {
        data = (data & 0xfff0) | (source << 1);
        if (protocol)
            data |= 0x0001;
        else
        {
            data &= ~0x0001;
            if (source == 1)        /* DSP transmit */
                data &= ~0x0008;
            else
                data |= 0x0008;
        }
    }
    if (dest & 0x0002)  /* DSP receive */
    {
        data = (data & 0xff8f) | (source << 5); /* do not touch bit 7 (DSP) */
        if (protocol)
            data |= 0x0010;
        else
            data &= ~0x0010;
    }
    if (dest & 0x0004)  /* external output */
    {
        data = (data & 0xf0ff) | (source << 9);
        if (protocol)
            data |= 0x0100;
        else
            data &= ~0x0100;
    }
    if (dest & 0x0008)  /* DAC */
    {
        data = (data & 0x0fff) | (source << 13);
    }
    DMASOUND->crossbar_dest = data;

    /*
     * finally, handle clock
     */
    if (clk & 1)        /* external clock */
        DMASOUND->freq_ext = prescale;
    else                /* internal clock */
        DMASOUND->freq_int = prescale;

    return 1;
}
#endif

/**
 * Provide STE/TT compatible frequency setting
 */
static LONG devconnect_ste(WORD source, WORD dest, WORD clk,
                           WORD prescale, WORD protocol)
{
    UWORD modectrl;

    if (source != 0 || dest != 8 || clk != 0)
        return EBADRQ;

    modectrl = DMASOUND->mode_control & 0xFC;
    switch (prescale)
    {
     case 0:             /* Set STE compatible mode -> we can abort here */
        return 0;
     case 1:
        modectrl |= 3;         /* 50 kHz */
        break;
     case 2: case 3: case 4:
        modectrl |= 2;         /* 25 kHz */
        break;
     case 5: case 6: case 7: case 8:
        modectrl |= 1;         /* 12 kHz */
        break;
     default:
        break;                 /* leave it at 6 kHz */
    }
    DMASOUND->mode_control = modectrl;

    return 0;
}

LONG devconnect(WORD source, WORD dest, WORD clk, WORD prescale, WORD protocol)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x8b;    /* unimplemented xbios call: return function # */

#if !MPS_STE_SOUND_ONLY
    if (has_falcon_dmasound)
        return devconnect_falcon(source, dest, clk, prescale, protocol);
#endif

    return devconnect_ste(source, dest, clk, prescale, protocol);
}

/**
 * Get sound status or reset sound system
 *
 * To reset the sound system, we toggle bit3 of the byte at $ff8937
 * (codec_16bit_source).  This zeroes the following fields:
 *      $ff8920     track_control
 *      $ff8921     mode_control
 *      $ff8930-31  crossbar_src
 *      $ff8932-33  crossbar_dest
 *      $ff8934     freq_ext
 *      $ff8935     freq_int
 *      $ff8936     record_tracks
 *      $ff8937     codec_16bit_source (except bit 3 which we zero ourselves :-))
 *      $ff8938     codec_adc_source
 *      $ff8939     channel_amplification
 *      $ff893a-3b  channel_attenuation
 * in addition:
 *      $ff893c-3d  codec_status is set to $003f
 */
LONG sndstatus(WORD reset)
{
    if (!SOUND_IS_AVAILABLE)
        return 0x8c;    /* unimplemented xbios call: return function # */

#if !MPS_STE_SOUND_ONLY
    if (!has_falcon_dmasound)
#endif
        return 0;

#if !MPS_STE_SOUND_ONLY
    if (reset)
    {
        DMASOUND->channel_attenuation = 0x0000;
        DMASOUND->codec_16bit_source |= 0x08;    /* TOS does this, so do we */
        DMASOUND->codec_16bit_source &= ~0x08;
    }

    return (DMASOUND->codec_status >> 4) & 0x3f;
#endif
}

/**
 * Get current frame replay/recording positions
 */
LONG buffptr(LONG ptr)
{
    struct SndBufPtr {
        LONG play;
        LONG record;
        LONG res1;
        LONG res2;
    } *sbp;
    UBYTE hi, mid, low;

    if (!SOUND_IS_AVAILABLE)
        return 0x8d;    /* unimplemented xbios call: return function # */

    sbp = (struct SndBufPtr *)ptr;

#if !MPS_STE_SOUND_ONLY
    if (has_falcon_dmasound)
    {
        DMASOUND->control |= 0x80;  /* Select recording frame registers */
        hi = DMASOUND->frame_counter_high;
        mid = DMASOUND->frame_counter_mid;
        low = DMASOUND->frame_counter_low;
        sbp->record = ((ULONG)hi << 16) | ((ULONG)mid << 8) | low;
        DMASOUND->control &= 0x7f;  /* Select replay frame registers */
    }
#endif

    hi = DMASOUND->frame_counter_high;
    mid = DMASOUND->frame_counter_mid;
    low = DMASOUND->frame_counter_low;
    sbp->play = ((ULONG)hi << 16) | ((ULONG)mid << 8) | low;

    return 0;
}

#endif /* CONF_WITH_DMASOUND */
