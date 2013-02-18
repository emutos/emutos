/*
 * dmasound.c - STe/TT/Falcon DMA sound routines
 *
 * Copyright (c) 2011 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_DMASOUND 0

#include "config.h"
#include "dmasound.h"
#include "portab.h"
#include "vectors.h"
#include "kprint.h"

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
    UBYTE codec_status; /* CODEC-Status */
    UBYTE filler3d[4];
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
    UWORD olddata = DMASOUND->microwire_data;

    DMASOUND->microwire_data = data;
    while (DMASOUND->microwire_data != olddata)
    {
        /* Wait for data to be tranferred */
    }
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
    lmc1992_init();

    /* TODO: Initialize the other DMA sound hardware */
}

#endif /* CONF_WITH_DMASOUND */
