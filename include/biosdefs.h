/*
 * biosdefs.h - Public BIOS defines and structures
 *
 * Copyright (C) 2016-2022 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOSDEFS_H
#define BIOSDEFS_H

/* Number of hz_200 ticks per second */
#define CLOCKS_PER_SEC 200UL

/*
 * Universal constant (for older drives, anyway)
 */
#define SECTOR_SIZE 512UL

/* Fast RAM Buffer specifications */
#define FRB_SIZE    (64 * 1024UL)   /* pointed to by _FRB cookie */
#define FRB_SECS    (FRB_SIZE / SECTOR_SIZE)

/* Forward declarations */
struct _pd;

/* GEM memory usage parameters block.
 * This is actually a structure used by the BIOS
 * in order to call the main UI (i.e. AES) */

#define GEM_MUPB_MAGIC 0x87654321

typedef struct
{
    ULONG     gm_magic; /* Magical value, has to be GEM_MUPB_MAGIC */
    UBYTE     *gm_end;  /* End of the static ST-RAM used by the OS */
    PRG_ENTRY *gm_init; /* Start address of the main UI */
} GEM_MUPB;

typedef struct _osheader
{
    UWORD      os_entry;      /* BRA.S to reset handler */
    UWORD      os_version;    /* TOS version number */
    PFVOID     reseth;        /* Pointer to reset handler */
    /* The following 3 longs are actually the default GEM_MUPB structure */
    struct _osheader *os_beg; /* Base address of the operating system */
    UBYTE      *os_end;       /* End of ST-RAM statically used by the OS */
    PRG_ENTRY  *os_rsvl;      /* Entry point of default UI (unused) */
    GEM_MUPB   *os_magic;     /* GEM memory usage parameters block */
    ULONG      os_date;       /* TOS date in BCD format MMDDYYYY */
    UWORD      os_conf;       /* Flag for PAL version + country */
    UWORD      os_dosdate;    /* TOS date in BDOS format */
    WORD       **os_root;     /* Pointer to the BDOS quick pool */
    UBYTE      *os_kbshift;   /* Pointer to the BIOS shifty variable */
    struct _pd **os_run;      /* Pointer to the pointer to current BASEPAGE */
    ULONG      os_dummy;      /* 'ETOS' signature */
} OSHEADER;

/*
 *  BPB - Bios Parameter Block
 */
struct _bpb /* bios parameter block */
{
    UWORD recsiz;       /* sector size in bytes */
    UWORD clsiz;        /* cluster size in sectors */
    UWORD clsizb;       /* cluster size in bytes */
    UWORD rdlen;        /* root directory length in records */
    UWORD fsiz;         /* FAT size in records */
    UWORD fatrec;       /* first FAT record (of last FAT) */
    UWORD datrec;       /* first data record */
    UWORD numcl;        /* number of data clusters available */
    UWORD b_flags;      /* flags (see below) */
};
typedef struct _bpb BPB;

/*
 *  flags for BPB
 */
#define B_16    1       /* device has 16-bit FATs */
#define B_1FAT  2       /* device has only a single FAT */

/*
 * Flags for Kbshift()
 */
#define MODE_RSHIFT 0x01        /* Right Shift key is down */
#define MODE_LSHIFT 0x02        /* Left Shift key is down  */
#define MODE_CTRL   0x04        /* Control is down         */
#define MODE_ALT    0x08        /* Alternate is down       */
#define MODE_SCA    (MODE_RSHIFT|MODE_LSHIFT|MODE_CTRL|MODE_ALT)
#define MODE_CAPS   0x10        /* CapsLock is down        */
                        /* the following bits are ONLY set if the Alt key is already */
                        /* down.  however, they remain set until the corresponding   */
                        /* key is released, even if the Alt key is released first.   */
#define MODE_HOME   0x20        /* Clr/Home is down        */
#define MODE_INSERT 0x40        /* Insert is down          */
#define MODE_SHIFT  (MODE_RSHIFT|MODE_LSHIFT)   /* shifted (convenience */

/*
 * Struct returned by Kbdvbase()
 */
struct kbdvecs
{
    PFVOID midivec;     /* MIDI Input */
    PFVOID vkbderr;     /* IKBD Error */
    PFVOID vmiderr;     /* MIDI Error */
    PFVOID statvec;     /* IKBD Status */
    PFVOID mousevec;    /* IKBD Mouse */
    PFVOID clockvec;    /* IKBD Clock */
    PFVOID joyvec;      /* IKBD Joystick */
    PFVOID midisys;     /* Main MIDI Vector */
    PFVOID ikbdsys;     /* Main IKBD Vector */
};

/*
 * Types for callbacks
 */
typedef void (*ETV_TIMER_T)(int ms); /* Type of BDOS Event Timer */

/* standard Atari resolution values */
#define ST_LOW          0   /* used for ST/STe */
#define ST_MEDIUM       1
#define ST_HIGH         2
#define FALCON_REZ      3   /* used as a Falcon indicator */
#define TT_MEDIUM       4   /* used for TT */
#define TT_HIGH         6
#define TT_LOW          7

#define MIN_REZ         ST_LOW          /* valid range (except that 5 isn't used) */
#define MAX_REZ         TT_LOW

/* monitor types (from VgetMonitor()) */
#define MON_MONO       0    /* ST monochrome */
#define MON_COLOR      1    /* ST colour */
#define MON_VGA        2    /* VGA */
#define MON_TV         3    /* TV via RF modulator */

/* TT shifter defines */
#define TT_HYPER_MONO       0x1000      /* bit usage */
#define TT_DUOCHROME_INVERT 0x0002      /* inversion bit in TT h/w palette reg 0 */

/* bit settings for Falcon videomodes */
#define VIDEL_VALID    0x01ff           /* the only bits allowed in a videomode */
#define VIDEL_VERTICAL 0x0100           /* if set, use interlace (TV), double line (VGA) */
#define VIDEL_COMPAT   0x0080           /* ST-compatible if set */
#define VIDEL_OVERSCAN 0x0040           /* overscan if set (not used with VGA) */
#define VIDEL_PAL      0x0020           /* PAL if set; otherwise NTSC */
#define VIDEL_VGA      0x0010           /* VGA if set; otherwise TV */
#define VIDEL_80COL    0x0008           /* 80-column mode if set; otherwise 40 */
#define VIDEL_BPPMASK  0x0007           /* mask for bits/pixel encoding */
#define VIDEL_1BPP          0               /* 2 colours */
#define VIDEL_2BPP          1               /* 4 colours */
#define VIDEL_4BPP          2               /* 16 colours */
#define VIDEL_8BPP          3               /* 256 colours */
#define VIDEL_TRUECOLOR     4               /* 65536 colours */

/* IDT cookie flag for 24 hour: 0 = 12am/pm or 1 = 24 hour */
#define IDT_12H   0x0000
#define IDT_24H   0x1000
#define IDT_TMASK 0x1000  /* time mask */

/* IDT cookie bit definitions for printing date */
#define IDT_BIT_DM 0x100 /* day before month */
#define IDT_BIT_YM 0x200 /* year before month */
#define IDT_MMDDYY 0x000
#define IDT_DDMMYY IDT_BIT_DM
#define IDT_YYMMDD IDT_BIT_YM
#define IDT_YYDDMM (IDT_BIT_YM | IDT_BIT_DM)
#define IDT_DMASK  (IDT_BIT_YM | IDT_BIT_DM)
#define IDT_SMASK  0xFF  /* separator mask */

/* Mfpint() vector indices */
/* Taken from MiNTLib's include/mint/ostruct.h */
#define MFP_PARALLEL           0
#define MFP_DCD                1
#define MFP_CTS                2
#define MFP_BITBLT             3
#define MFP_TIMERD             4
#define MFP_BAUDRATE  MFP_TIMERD
#define MFP_200HZ              5
#define MFP_ACIA               6
#define MFP_DISK               7
#define MFP_TIMERB             8
#define MFP_HBLANK    MFP_TIMERB
#define MFP_TERR               9
#define MFP_TBE               10
#define MFP_RERR              11
#define MFP_RBF               12
#define MFP_TIMERA            13
#define MFP_DMASOUND  MFP_TIMERA
#define MFP_RING              14
#define MFP_MONODETECT        15

/* Bitmasks for Offgibit()/Ongibit() */
/* Taken from MiNTLib's include/mint/ostruct.h */
#define GI_FLOPPYSIDE   0x01
#define GI_FLOPPYA      0x02
#define GI_FLOPPYB      0x04
#define GI_RTS          0x08
#define GI_DTR          0x10
#define GI_STROBE       0x20
#define GI_GPO          0x40
#define GI_SCCPORT      0x80

#endif /* BIOSDEFS_H */
