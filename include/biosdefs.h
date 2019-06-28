/*
 * biosdefs.h - Public BIOS defines and structures
 *
 * Copyright (C) 2016 The EmuTOS development team
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

#include "portab.h"

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
#define B_FIX   2       /* device has fixed media */

/*
 * Flags for Kbshift()
 */
#define MODE_RSHIFT 0x01        /* Right Shift key is down */
#define MODE_LSHIFT 0x02        /* Left Shift key is down  */
#define MODE_CTRL   0x04        /* Control is down         */
#define MODE_ALT    0x08        /* Alternate is down       */
#define MODE_CAPS   0x10        /* CapsLock is down        */
                        /* the following bits are ONLY set if the Alt key is already */
                        /* down.  however, they remain set until the corresponding   */
                        /* key is released, even if the Alt key is released first.   */
#define MODE_HOME   0x20        /* Clr/Home is down        */
#define MODE_INSERT 0x40        /* Insert is down          */

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

#endif /* BIOSDEFS_H */
