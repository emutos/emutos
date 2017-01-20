/*
 * tosvars.S - TOS System variables
 *
 * Copyright (C) 2005-2016 by the EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

        .globl  _stkbot
        .globl  _stktop
        .globl  bssstart
        .globl  _shifty

// ===========================================================================
// ==== BSS segment ==========================================================
// ===========================================================================

        .bss

// This is the very beginning of the EmuTOS BSS segment.
// It usually starts at 0x800, just after the system variables.

// BIOS internal stack
// Be sure to keep it under bssstart to avoid clearing it
// when clearing the BSS segment.
// As this area is never cleared, util/amigaboot.S will use it to transfer
// the Alt-RAM region list to ramtos.
_stkbot:        .ds.b   2048
_stktop:

// memory past this address will be cleared on startup or on reset.
bssstart:

// Note: _shifty was moved here to ensure that it lies within the first 32K
// of memory.  This is not required by the architecture, but GFA Basic
// assumes that it is half-word addressable, and may crash the system
// otherwise.
_shifty:        .ds.b   1       // reflects the status up/down of mode keys
