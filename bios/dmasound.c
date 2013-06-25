/*
 * dmasound.c - STe/TT/Falcon DMA sound routines
 *
 * Copyright (c) 2011-2013 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *  THH   Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_DMASOUND 0

#include "config.h"
#include "portab.h"
#include "dmasound.h"
#include "vectors.h"
#include "kprint.h"
#include "gemerror.h"

#if CONF_WITH_DMASOUND

struct dmasound
{
    /* STe/TT DMA registers */
    UBYTE interrupt; /* Buffer interrupts */
    UBYTE control; /* DMA Control Register */
    UBYTE filler02;
    UBYTE frame_start_high; /* Frame start address (high byte) */
    UBYTE filler04;
    UBYTE frame_start_mid; /* Frame start address (mid byte) */
    UBYTE filler06;
    UBYTE frame_start_low; /* Frame start address (low byte) */
    UBYTE filler08;
    UBYTE frame_counter_high; /* Frame address counter (high byte) */
    UBYTE filler0a;
    UBYTE frame_counter_mid; /* Frame address counter (mid byte) */
    UBYTE filler0c;
    UBYTE frame_counter_low; /* Frame address counter (low byte) */
    UBYTE filler0e;
    UBYTE frame_end_high; /* Frame end address (high byte) */
    UBYTE filler10;
    UBYTE frame_end_mid; /* Frame end address (mid byte) */
    UBYTE filler12;
    UBYTE frame_end_low; /* Frame end address (low byte) */
    UBYTE filler14[12];
    UBYTE track_control; /* DMA Track Control */
    UBYTE mode_control; /* Sound mode control */
    /* STe/TT Microwire interface registers */
    UWORD microwire_data; /* Microwire data register */
    UWORD microwire_mask; /* Microwire mask register */
    UBYTE filler26[10];
    /* Falcon DMA registers */
    UWORD crossbar_src; /* Crossbar Source Controller */
    UWORD crossbar_dest; /* Crossbar Destination Controller */
    UBYTE freq_ext; /* Frequency Divider External Clock */
    UBYTE freq_int; /* Frequency Divider Internal Sync */
    UBYTE record_tracks; /* Record Tracks Select */
    UBYTE codec_16bit_source; /* CODEC Input Source from 16bit adder */
    UBYTE codec_adc_source; /* CODEC ADC-Input for L+R Channel */
    UBYTE channel_amplification; /* Channel amplification */
    UWORD channel_attenuation; /* Channel attenuation */
    UWORD codec_status; /* CODEC-Status */
    UBYTE filler3e[3];
    UBYTE gpx_data_direction; /* GPx Data Direction */
    UBYTE filler42;
    UBYTE gpx_data_port; /* GPx Data Port */
};

#define DMASOUND ((volatile struct dmasound*)0xffff8900)

/* Generic Microwire macros */
#define MICROWIRE_BIT_OFFSET 1 /* As seen in TOS 1.62 */
#define MICROWIRE_UNKNWON_BITS 0x0001 /* Something like a stop bit ?? */
#define MICROWIRE_MASK (0x07ff << MICROWIRE_BIT_OFFSET)
#define MICROWIRE_ADDRESS(x) ((x) << 9)
#define MICROWIRE_COMMAND(a,c) (((MICROWIRE_ADDRESS(a) | (c)) << MICROWIRE_BIT_OFFSET) | MICROWIRE_UNKNWON_BITS)

/* LMC1992 macros */
#define LMC1992_MICROWIRE_ADDRESS 2
#define LMC1992_FUNCTION(x) ((x) << 6)
#define LMC1992_PARAMETER(x) (x)
#define LMC1992_COMMAND(f,p) MICROWIRE_COMMAND(LMC1992_MICROWIRE_ADDRESS, LMC1992_FUNCTION(f) | LMC1992_PARAMETER(p))

/* LMC1992 functions */
#define LMC1992_FUNCTION_INPUT_SELECT 0
#define LMC1992_FUNCTION_BASS 1
#define LMC1992_FUNCTION_TREEBLE 2
#define LMC1992_FUNCTION_VOLUME 3
#define LMC1992_FUNCTION_RIGHT_FRONT_FADER 4
#define LMC1992_FUNCTION_LEFT_FRONT_FADER 5
#define LMC1992_FUNCTION_RIGHT_REAR_FADER 6
#define LMC1992_FUNCTION_LEFT_REAR_FADER 7

/* LMC1992 parameters for function INPUT_SELECT */
#define LMC1992_INPUT_OPEN 0
#define LMC1992_INPUT_1 1
#define LMC1992_INPUT_2 2
#define LMC1992_INPUT_3 3
#define LMC1992_INPUT_4 4

/* LMC1992 parameter for Bass and Treeble functions */
#define LMC1992_TONE(x) (((x) + 12) / 2) /* Range from -12 to +12 dB */

/* LMC1992 parameter for Volume function */
#define LMC1992_VOLUME(x) (((x) + 80) / 2) /* Range from -80 to 0 dB */

/* LMC1992 parameter for Faders functions */
#define LMC1992_FADER(x) (((x) + 40) / 2) /* Range from -40 to 0 dB */

static int sound_locked;

int has_dmasound;
int has_microwire;
int has_falcon_dmasound;

void detect_dmasound(void)
{
    /* First, detect basic STe/TT DMA sound */
    has_dmasound = check_read_byte((long)&DMASOUND->control);

    /* Then detect advanced Falcon DMA sound */
    if (has_dmasound)
    {
        /* Warning: The Falcon registers below GPx does not exist on STe/TT
         * but they don't cause a bus error. */
        has_falcon_dmasound = check_read_byte((long)&DMASOUND->gpx_data_port);
    }

    /* The Falcon has no microwire interface.
     * This is not detectable through a bus error. */
    has_microwire = has_dmasound && !has_falcon_dmasound;

#if DBG_DMASOUND
    kprintf("has_dmasound = %d\n", has_dmasound);
    kprintf("has_microwire = %d\n", has_microwire);
    kprintf("has_falcon_dmasound = %d\n", has_falcon_dmasound);
#endif
}

static void write_microwire(UWORD data)
{
    UWORD oldmask;

    do
    {
        /* Wait for previous data transfer to finish */
        oldmask = DMASOUND->microwire_mask;
        asm volatile (" nop " ::: "memory");
    }
    while (oldmask != DMASOUND->microwire_mask);

    DMASOUND->microwire_data = data;
}

static void lmc1992_init(void)
{
    if (!has_microwire)
        return;

    DMASOUND->microwire_mask = MICROWIRE_MASK;

    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_VOLUME, LMC1992_VOLUME(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_LEFT_FRONT_FADER, LMC1992_FADER(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_RIGHT_FRONT_FADER, LMC1992_FADER(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_TREEBLE, LMC1992_TONE(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_BASS, LMC1992_TONE(0)));
    write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_INPUT_SELECT, LMC1992_INPUT_1));
}

void dmasound_init(void)
{
    sound_locked = 0;

    lmc1992_init();

    /* TODO: Initialize the other DMA sound hardware */
}

/* *** XBIOS DMA sound functions *** */

/**
 * Lock the XBIOS DMA sound subsystem
 */
LONG locksnd(void)
{
    if (sound_locked || !has_dmasound)
        return -129;

    sound_locked = 1;
    return 1;
}

/**
 * Free the XBIOS DMA sound subsystem
 */
LONG unlocksnd(void)
{
    if (!sound_locked)
        return -128;

    sound_locked = 0;
    return 0;
}


/**
 * Configure various sound settings of Falcon DMA sound
 */
static LONG sndcmd_falcon(WORD mode, WORD data)
{
    UWORD val;

    switch (mode)
    {
     case 0:                // LTATTEN
       val = DMASOUND->channel_attenuation;
       if (data != -1)
       {
           data = (data & 0x00f0) << 4;
           val = (val & 0xf0ff) | data;
           DMASOUND->channel_attenuation = val;
       }
       return (val >> 4) & 0x00f0;
     case 1:                // RTATTEN
       val = DMASOUND->channel_attenuation;
       if (data != -1)
       {
           data = data & 0x00f0;
           val = (val & 0xff0f) | data;
           DMASOUND->channel_attenuation = val;
       }
       return val & 0x00f0;
     case 2:                // LTGAIN
       val = DMASOUND->channel_amplification;
       if (data != -1)
       {
           data = data & 0xf0;
           val = (val & 0x0f) | data;
           DMASOUND->channel_attenuation = val;
       }
       return val & 0x0f0;
     case 3:                // RTGAIN
       val = DMASOUND->channel_amplification;
       if (data != -1)
       {
           data = (data & 0xf0) >> 4;
           val = (val & 0xf0) | data;
           DMASOUND->channel_attenuation = val;
       }
       return (val & 0x0f) << 4;
     case 4:                // ADDERIN
       if (data != -1)
       {
           DMASOUND->codec_16bit_source = data;
       }
       return DMASOUND->codec_16bit_source & 0x3;
     case 5:                // ADCINPUT
       if (data != -1)
       {
           DMASOUND->codec_adc_source = data;
       }
       return DMASOUND->codec_adc_source & 0x3;
    }

    return 0;
}

/**
 * Configure various sound settings of STE DMA sound
 */
static LONG sndcmd_ste(WORD mode, WORD data)
{
    static UWORD ltatten, rtatten;

    switch (mode)
    {
     case 0:                // LTATTEN
        if (data != -1)
        {
            ltatten = ((UWORD)data & 0xff)/6;
            if (ltatten > 40)
                ltatten = 40;
            write_microwire(LMC1992_COMMAND(LMC1992_FUNCTION_LEFT_FRONT_FADER,
                                            LMC1992_FADER(-ltatten)));
        }
        return ltatten * 6;
     case 1:                // RTATTEN
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

    if (has_falcon_dmasound)
    {
        return sndcmd_falcon(mode, data);
    }
    else if (has_microwire)
    {
        return sndcmd_ste(mode, data);
    }

    return 0;
}

/**
 * Set DMA sound frame buffer pointers
 */
LONG setbuffer(UWORD mode, ULONG startaddr, ULONG endaddr)
{
    if (mode > 1 || (mode == 1 && !has_falcon_dmasound))
        return EBADRQ;

    if (has_falcon_dmasound)
    {
        if (mode == 1)
            DMASOUND->control |= 0x80;  /* Select recording frame registers */
        else
            DMASOUND->control &= 0x7f;  /* Select replay frame registers */
    }

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
    if (playtracks > 3 || rectracks > 3 || !has_falcon_dmasound)
        return EBADRQ;

    DMASOUND->track_control = (DMASOUND->track_control & 0xfc) | playtracks;
    DMASOUND->record_tracks = rectracks;

    return 0;
}

/**
 * Set DAC track
 */
LONG setmontracks(UWORD montrack)
{
    if (montrack > 3 || !has_falcon_dmasound)
        return EBADRQ;

    DMASOUND->track_control = (DMASOUND->track_control&0xcf) | (montrack << 4);

    return 0;
}

/**
 * Set interrupt mode (Timer-A or MFP-i7)
 */
LONG setinterrupt(UWORD mode, WORD cause)
{
    UBYTE irqreg;

    if (mode > 1 || !has_falcon_dmasound)
        return EBADRQ;

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
    if (mode < 0)
    {
        LONG ret;
        UBYTE ctrl = DMASOUND->control;
        ret = ctrl & 0x3;
        if (has_falcon_dmasound)
            ret |= ((ctrl >> 2) & 0xc);
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
    if (!has_falcon_dmasound)
        return EBADRQ;

    if (dspxmit)
        DMASOUND->crossbar_src |= 0x80;
    else
        DMASOUND->crossbar_src &= 0x7f;

    if (dsprec)
        DMASOUND->crossbar_dest |= 0x80;
    else
        DMASOUND->crossbar_dest &= 0x7f;

    return 0;
}

/**
 * Set/get the GPIO pins of the DSP connector
 */
LONG gpio(UWORD mode, UWORD data)
{
    switch (mode)
    {
     case 0:             /* Set direction */
        DMASOUND->gpx_data_direction = data;
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
}

/**
 * Devconnect for Falcon hardware
 */
static LONG devconnect_falcon(WORD source, WORD dest, WORD clk,
                              WORD prescale, WORD protocol)
{
    UWORD data;
    int i, val;
    int ret = 0;

    protocol &= 1;
    source &= 3;

    if (source == 3 && clk > 1) {
        clk = 0;
        ret = EBADRQ;
    }

    data = DMASOUND->crossbar_src;
    data &= ~(0xf << (source*4));   /* Mask old settings */
    val = ((clk&3)<<1) | protocol;
    if (source == 3)
        val &= 0xE;
    data |= val << (source*4);
    if ((dest & 0xD) != 0 && protocol == 0)
        data |= 0x8;
    DMASOUND->crossbar_src = data;

    data = DMASOUND->crossbar_dest;
    for (i = 0; i < 4; i++)
    {
        if ((dest & (1 << i)) != 0)
        {
            data &= ~(0xf << (i*4));   /* Mask old settings */
            val = ((source << 1)| protocol);
            if (i == 3)
                val &= 0xE;
            if (source != 1 && i == 0 && protocol == 0)
                val |= 0x8;
            data |= val << (i*4);
        }
    }
    DMASOUND->crossbar_dest = data;

    if (clk == 1)
        DMASOUND->freq_ext = prescale;
    else
        DMASOUND->freq_int = prescale;

    return ret;
}


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
    if (has_falcon_dmasound)
        return devconnect_falcon(source, dest, clk, prescale, protocol);
    else
        return devconnect_ste(source, dest, clk, prescale, protocol);
}


LONG sndstatus(WORD reset)
{
    if (!has_falcon_dmasound)
        return 0;

    if (reset)
    {
        DMASOUND->channel_attenuation = 0x0000;
        DMASOUND->codec_16bit_source |= 0x08;    // TOS does this, so do we
        DMASOUND->codec_16bit_source &= ~0x08;
    }

    return (DMASOUND->codec_status >> 4) & 0x3f;
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

    sbp = (struct SndBufPtr *)ptr;

    if (has_falcon_dmasound)
    {
        DMASOUND->control |= 0x80;  /* Select recording frame registers */
        hi = DMASOUND->frame_counter_high;
        mid = DMASOUND->frame_counter_mid;
        low = DMASOUND->frame_counter_low;
        sbp->record = ((ULONG)hi << 16) | ((ULONG)mid << 8) | low;
        DMASOUND->control &= 0x7f;  /* Select replay frame registers */
    }

    hi = DMASOUND->frame_counter_high;
    mid = DMASOUND->frame_counter_mid;
    low = DMASOUND->frame_counter_low;
    sbp->play = ((ULONG)hi << 16) | ((ULONG)mid << 8) | low;

    return 0;
}

#endif /* CONF_WITH_DMASOUND */
