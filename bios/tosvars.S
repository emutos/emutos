/*
 * tosvars.S - TOS System variables
 *
 * Copyright (c) 2005-2013 by the EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

#include "asmdefs.h"


        .globl  bssstrt
        .globl  bssstart
        .globl  _proc_lives
        .globl  _proc_dregs
        .globl  _proc_aregs
        .globl  _proc_enum
        .globl  _proc_usp
        .globl  _proc_stk

        .globl  sysvars_start
        .globl  _etv_timer
        .globl  _etv_critic
        .globl  _etv_term
        .globl  _etv_xtra
        .globl  _memvalid
        .globl  memvalid
        .globl  memctrl
        .globl  resvalid
        .globl  resvector
        .globl  _phystop
        .globl  _membot
        .globl  _memtop
        .globl  _memval2
        .globl  memval2
        .globl  _flock
        .globl  _seekrate
        .globl  _timer_ms
        .globl  _fverify
        .globl  _bootdev
        .globl  _palmode
        .globl  _defshiftmod
        .globl  _sshiftmod
        .globl  _v_bas_ad
        .globl  _vblsem
        .globl  _nvbls
        .globl  _vblqueue
        .globl  _colorptr
        .globl  _screenpt
        .globl  _vbclock
        .globl  _frclock
        .globl  _hdv_init
        .globl  _swv_vec
        .globl  _hdv_bpb
        .globl  _hdv_rw
        .globl  _hdv_boot
        .globl  _hdv_mediach
        .globl  _cmdload
        .globl  _conterm
        .globl  _themd
        .globl  ____md
        .globl  _savptr
        .globl  _nflops
        .globl  con_state
        .globl  _save_row
        .globl  sav_context
        .globl  _bufl
        .globl  _hz_200
        .globl  the_env
        .globl  _drvbits
        .globl  _dskbufp
        .globl  _autopath
        .globl  _vbl_list
        .globl  _dumpflg
        .globl  _sysbase
        .globl  _shell_p
        .globl  _end_os
        .globl  _exec_os
        .globl  _dump_vec
        .globl  _prt_stat
        .globl  _prt_vec
        .globl  _aux_stat
        .globl  _aux_vec
        .globl  _pun_ptr
        .globl  _memval3
        .globl  memval3
        .globl  _bconstat_vec
        .globl  _bconin_vec
        .globl  _bcostat_vec
        .globl  _bconout_vec
#ifndef __mcoldfire__
        .globl  _longframe
#endif
        .globl  _ramtop
        .globl  _ramvalid
        .globl  _bell_hook
        .globl  _kcl_hook
        .globl  sysvars_end

#ifdef TARGET_FLOPPY
        .globl  _autoboot_magic
#endif

#if CONF_WITH_PSEUDO_COLD_BOOT
        .globl  _warm_magic
#endif

#ifdef CONF_WITH_68030_PMMU
        .globl  _pmmutree
#endif
        .globl  _shifty

        .globl  _sdata          // start adress of data section

// ===========================================================================
// ==== DATA segment =========================================================
// ===========================================================================

        .data
_sdata:

// ===========================================================================
// ==== BSS segment ==========================================================
// ===========================================================================


// ==== Beginning of RAM (used by OS) ========================================
        .bss
        .org 0x00000000         // start of RAM
bssstrt:

// ==== Start of Exception related variables =================================
        .org    0x380
_proc_lives:    .ds.l   1       // validates system crash page, if 0x12345678
_proc_dregs:    .ds.l   8       // saved registers d0-d7
_proc_aregs:    .ds.l   8       // saved registers a0-a7
_proc_enum:     .ds.l   1       // vector number of crash exception
_proc_usp:      .ds.l   1       // saved user stackpointer
_proc_stk:      .ds.w   16      // 16 words from exception stack

// ==== Start of System variables ============================================
        .org 0x400
sysvars_start:
_etv_timer:     .ds.l   1       // GEM event timer vector
_etv_critic:    .ds.l   1       // GEM critical error handler
_etv_term:      .ds.l   1       // GEM program termination vector
_etv_xtra:      .ds.l   5       // GEM additional vectors (unused)
_memvalid:
memvalid:       .ds.l   1       // memory conf valid, if 0x752019f3
        .org 0x424
memctrl:        .ds.b   1       // copy of contents of 0xffff8001
        .org 0x426
resvalid:       .ds.l   1       // validates resvector, if 0x31415926
resvector:      .ds.l   1       // reset vector
_phystop:       .ds.l   1       // physical top of RAM
_membot:        .ds.l   1       // start of TPA (user memory)
_memtop:        .ds.l   1       // end of TPA (user memory)
_memval2:
memval2:        .ds.l   1       // validates memcntrl/memconf, if 0x237698aa
_flock:         .ds.w   1       // if != 0, VBL floppy routine is disabled
_seekrate:      .ds.w   1       // floppy seek rate
_timer_ms:      .ds.w   1       // time between timer calls in ms
_fverify:       .ds.w   1       // if != 0, verify floppy writes
_bootdev:       .ds.w   1       // default boot drive
_palmode:       .ds.w   1       // 0 = NTSC, else PAL
_defshiftmod:   .ds.w   1       // default video resolution
_sshiftmod:     .ds.b   1       // copy of contents of 0xffff8260
        .org 0x44e
_v_bas_ad:      .ds.l   1       // screen base address
_vblsem:        .ds.w   1       // if > 0, the VBL routine is executed
_nvbls:         .ds.w   1       // number of VBL routines
_vblqueue:      .ds.l   1       // pointer to list of VBL routines
_colorptr:      .ds.l   1       // pointer to color palette to be loaded
_screenpt:      .ds.l   1       // pointer to video RAM for next VBL
_vbclock:       .ds.l   1       // number of VBL routines executed
_frclock:       .ds.l   1       // number of VBL routines processed
_hdv_init:      .ds.l   1       // vector for hard disk initialization
_swv_vec:       .ds.l   1       // vector for resolution change
_hdv_bpb:       .ds.l   1       // vector for getbpb for harddisk
_hdv_rw:        .ds.l   1       // vector for read/write for harddisk
_hdv_boot:      .ds.l   1       // vector for hard disk boot
_hdv_mediach:   .ds.l   1       // vector for hard disk media change
_cmdload:       .ds.w   1       // if not 0, load command.prg after boot
_conterm:       .ds.b   1       // attribute vector for console output
trp14ret:       .ds.l   1       // return address for trap #14 (unused)
criticret:      .ds.l   1       // return address for critical error hndlr
        .org 0x48e
_themd:         .ds.l   4       // first memory descriptor block
____md:         .ds.w   2       // space for additional memory descriptors
_savptr:        .ds.l   1       // pointer to BIOS save register block
_nflops:        .ds.w   1       // number of connected floppy drives
con_state:      .ds.l   1       // vector for screen output
_save_row:      .ds.w   1       // temporary storage for cursor line pos.
sav_context:    .ds.l   1       // ptr to save area for exception processing
_bufl:          .ds.l   2       // pointers to buffer ctrl blocks for GEMDOS
_hz_200:        .ds.l   1       // counter for 200 Hz system clock
the_env:        .ds.l   1       // pointer to default environment string
        .org 0x4c2
_drvbits:       .ds.l   1       // bit allocation for physical drives
_dskbufp:       .ds.l   1       // pointer to disk buffer
_autopath:      .ds.l   1       // pointer to auto-execute path
_vbl_list:      .ds.l   8       // pointers to 8 VBl routines
_dumpflg:       .ds.w   1       // flag for screen dump (unsused)
_prtabt:        .ds.w   1       // printer abort flag
_sysbase:       .ds.l   1       // pointer to start of OS
_shell_p:       .ds.l   1       // pointer to shell
_end_os:        .ds.l   1       // pointer to end of OS
_exec_os:       .ds.l   1       // pointer to entry point of OS

        .org 0x502
_dump_vec:      .ds.l   1       // pointer to screen dump routine
_prt_stat:      .ds.l   1       // pointer to prv_lsto
_prt_vec:       .ds.l   1       // pointer to prv_lst
_aux_stat:      .ds.l   1       // pointer to prv_auxo
_aux_vec:       .ds.l   1       // pointer to prv_aux
_pun_ptr:       .ds.l   1       // if AHDI, pointer to pun_info
_memval3:
memval3:        .ds.l   1       // memory conf valid if = $5555aaaa
_bconstat_vec:  .ds.l   8       // 8 pointers to input-status routines
_bconin_vec:    .ds.l   8       // 8 pointers to input routines
_bcostat_vec:   .ds.l   8       // 8 pointers to output-status routines
_bconout_vec:   .ds.l   8       // 8 pointers to output routines
_longframe:     .ds.w   1       // if not 0, then not 68000 - use long stack frames
_p_cookies:     .ds.l   1       // pointer to cookie jar
_ramtop:        .ds.l   1       // pointer to end of FastRam
_ramvalid:      .ds.l   1       // validates ramtop if $1357BD13
_bell_hook:     .ds.l   1       // pointer to routine for system bell
_kcl_hook:      .ds.l   1       // pointer to routine for system keyclick
        .org 0x5b4
sysvars_end:

#ifdef TARGET_FLOPPY
        .org 0x6f8              // must match the value in util/bootsect.S
_autoboot_magic:.ds.l   1
#endif

#if CONF_WITH_PSEUDO_COLD_BOOT
        .org 0x6fc              // any location below bssstart is ok
_warm_magic:    .ds.l   1       // set to WARM_MAGIC if this is a warm boot
#endif

#if CONF_WITH_68030_PMMU
        .org PMMUTREE_ADDRESS_68030
_pmmutree:      .ds.l   64      // for pmmu tree on 68030
#endif

// Memory below 0x800 is only accessible in supervisor mode on the ST.
// We have to make sure that nothing is linked into this region that might
// be accessed from user mode, too.
// (So the gap between 0x5b4 and 0x700 could be filled with system variables
//  or a buffer that is only accessed in supervisor mode.)
        .org 0x800


// memory past this address will be cleared on startup or on reset.
bssstart:
// Note: _shifty was moved here to ensure that it lies within the first 32K
// of memory.  This is not required by the architecture, but GFA Basic
// assumes that it is half-word addressable, and may crash the system
// otherwise.
_shifty:        .ds.b   1       // reflects the status up/down of mode keys

// ===========================================================================
// ==== End ==================================================================
// ===========================================================================
