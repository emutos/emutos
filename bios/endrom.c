/*
 * endrom.c - data to put at the end of the ROM
 *
 * Copyright (C) 2019-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "biosdefs.h"
#include "bios.h"
#if WITH_AES
#include "../aes/aesstub.h"
#endif
#if WITH_CLI
#include "../cli/clistub.h"
#endif

/* GEM memory usage parameters block.
 * This *must* live after the exec_os contents for Geneva compatibility.
 * In other words, the address of ui_mupb must be greater than the value
 * of its third field. */
const GEM_MUPB ui_mupb =
{
    GEM_MUPB_MAGIC, /* Magic value identifying this structure */
    _end_os_stram,  /* End of the static ST-RAM used by the OS */
#if WITH_AES
    ui_start        /* AES entry point */
#elif WITH_CLI
    coma_start      /* EmuCON entry point */
#else
#  error You must provide a main UI
#endif
};
