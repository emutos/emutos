/*
 * tosvars.ld - TOS system variables
 *
 * Copyright (C) 2005-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * The TOS system variables have been officially documented by Atari.
 * They have fixed addresses in the 0x380-0x800 region, so they are only
 * accessible in supervisor mode.
 * Documentation: http://toshyp.atari.org/en/003004.html
 */

/*
 * Note: this linker script fragment is included from emutos.ld.
 * It is preprocessed, to allow #include, #define, #if, etc.
 */

 /* ==== Start of Exception related variables ================================ */
_proc_lives     = 0x380;        /* validates system crash page, if 0x12345678 */
_proc_dregs     = 0x384;        /* saved registers d0-d7 */
_proc_aregs     = 0x3a4;        /* saved registers a0-a7 */
_proc_enum      = 0x3c4;        /* vector number of crash exception */
_proc_usp       = 0x3c8;        /* saved user stackpointer */
_proc_stk       = 0x3cc;        /* 16 words from exception stack */

/* ==== Start of System variables =========================================== */
sysvars_start   = 0x400;

_etv_timer      = 0x400;        /* GEM event timer vector */
_etv_critic     = 0x404;        /* GEM critical error handler */
_etv_term       = 0x408;        /* GEM program termination vector */
_etv_xtra       = 0x40c;        /* GEM additional vectors (unused) */
_memvalid       = 0x420;        /* memory conf valid, if 0x752019f3 */
memvalid        = _memvalid;
memctrl         = 0x424;        /* copy of contents of 0xffff8001 */
resvalid        = 0x426;        /* validates resvector, if 0x31415926 */
resvector       = 0x42a;        /* reset vector */
_phystop        = 0x42e;        /* physical top of RAM */
_membot         = 0x432;        /* start of TPA (user memory) */
_memtop         = 0x436;        /* end of TPA (user memory) */
_memval2        = 0x43a;        /* validates memcntrl/memconf, if 0x237698aa */
memval2         = _memval2;
_flock          = 0x43e;        /* if != 0, VBL floppy routine is disabled */
_seekrate       = 0x440;        /* floppy seek rate */
_timer_ms       = 0x442;        /* time between timer calls in ms */
_fverify        = 0x444;        /* if != 0, verify floppy writes */
_bootdev        = 0x446;        /* default boot drive */
_palmode        = 0x448;        /* 0 = NTSC, else PAL */
_defshiftmod    = 0x44a;        /* default video resolution */
_sshiftmod      = 0x44c;        /* copy of contents of 0xffff8260 */
_v_bas_ad       = 0x44e;        /* screen base address */
_vblsem         = 0x452;        /* if > 0, the VBL routine is executed */
_nvbls          = 0x454;        /* number of VBL routines */
_vblqueue       = 0x456;        /* pointer to list of VBL routines */
_colorptr       = 0x45a;        /* pointer to color palette to be loaded */
_screenpt       = 0x45e;        /* pointer to video RAM for next VBL */
_vbclock        = 0x462;        /* number of VBL routines executed */
_frclock        = 0x466;        /* number of VBL routines processed */
_hdv_init       = 0x46a;        /* vector for hard disk initialization */
_swv_vec        = 0x46e;        /* vector for resolution change */
_hdv_bpb        = 0x472;        /* vector for getbpb for harddisk */
_hdv_rw         = 0x476;        /* vector for read/write for harddisk */
_hdv_boot       = 0x47a;        /* vector for hard disk boot */
_hdv_mediach    = 0x47e;        /* vector for hard disk media change */
_cmdload        = 0x482;        /* if not 0, load command.prg after boot */
_conterm        = 0x484;        /* attribute vector for console output */
trp14ret        = 0x486;        /* return address for trap #14 (unused) */
criticret       = 0x48a;        /* return address for critical error hndlr */
_themd          = 0x48e;        /* first memory descriptor block */
____md          = 0x49e;        /* space for additional memory descriptors */
_savptr         = 0x4a2;        /* pointer to BIOS save register block */
_nflops         = 0x4a6;        /* number of connected floppy drives */
con_state       = 0x4a8;        /* vector for screen output */
_save_row       = 0x4ac;        /* temporary storage for cursor line pos. */
sav_context     = 0x4ae;        /* ptr to save area for exception processing */
_bufl           = 0x4b2;        /* pointers to buffer ctrl blocks for GEMDOS */
_hz_200         = 0x4ba;        /* counter for 200 Hz system clock */
the_env         = 0x4be;        /* pointer to default environment string */
_drvbits        = 0x4c2;        /* bit allocation for physical drives */
_dskbufp        = 0x4c6;        /* pointer to disk buffer */
_autopath       = 0x4ca;        /* pointer to auto-execute path */
_vbl_list       = 0x4ce;        /* pointers to 8 VBl routines */
_dumpflg        = 0x4ee;        /* flag for screen dump (unsused) */
_prtabt         = 0x4f0;        /* printer abort flag */
_sysbase        = 0x4f2;        /* pointer to start of OS */
_shell_p        = 0x4f6;        /* pointer to shell */
_end_os         = 0x4fa;        /* pointer to end of OS */
_exec_os        = 0x4fe;        /* pointer to entry point of OS */
_dump_vec       = 0x502;        /* pointer to screen dump routine */
_prt_stat       = 0x506;        /* pointer to prv_lsto */
_prt_vec        = 0x50a;        /* pointer to prv_lst */
_aux_stat       = 0x50e;        /* pointer to prv_auxo */
_aux_vec        = 0x512;        /* pointer to prv_aux */
_pun_ptr        = 0x516;        /* if AHDI, pointer to pun_info */
_memval3        = 0x51a;        /* memory conf valid if = $5555aaaa */
memval3         = _memval3;
_bconstat_vec   = 0x51e;        /* 8 pointers to input-status routines */
_bconin_vec     = 0x53e;        /* 8 pointers to input routines */
_bcostat_vec    = 0x55e;        /* 8 pointers to output-status routines */
_bconout_vec    = 0x57e;        /* 8 pointers to output routines */
_longframe      = 0x59e;        /* if not 0, then not 68000 - use long stack frames */
_p_cookies      = 0x5a0;        /* pointer to cookie jar */
_ramtop         = 0x5a4;        /* top of TT-RAM, or NULL if no TT-RAM is present */
_ramvalid       = 0x5a8;        /* if equal to RAMVALID_MAGIC, then ramtop is valid */
_bell_hook      = 0x5ac;        /* pointer to routine for system bell */
_kcl_hook       = 0x5b0;        /* pointer to routine for system keyclick */

/*
 * We can add any undocumented variables here, they will have fixed addresses
 * and will be accessible in supervisor mode only.
 */

#if CONF_DETECT_FIRST_BOOT_WITHOUT_MEMCONF
                                /* any location not cleared on reset is ok */
_warm_magic     = 0x6fc;        /* set to WARM_MAGIC if next boot must not */
                                /* be considered as a first boot (reset) */
#endif

#if CONF_WITH_68030_PMMU
_pmmutree       = PMMUTREE_ADDRESS_68030;   /* for pmmu tree on 68030 */
#endif

/*
 * The sysvars region ends at 0x800.
 */
