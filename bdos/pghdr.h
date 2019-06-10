/*
 * pghdr.h - header file for program loaders, 'size' pgms, etc.
 *
 * Copyright (C) 2001 Lineo, Inc.
 * Copyright (C) 2015-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef PGHDR_H
#define PGHDR_H

typedef struct
{
        /*  magic number is already read  */
        LONG    h01_tlen ;      /*  length of text segment              */
        LONG    h01_dlen ;      /*  length of data segment              */
        LONG    h01_blen ;      /*  length of bss  segment              */
        LONG    h01_slen ;      /*  length of symbol table              */
        LONG    h01_res1 ;      /*  reserved - always zero              */
        ULONG   h01_flags ;     /*  flags                               */
        WORD    h01_abs ;       /*  not zero if no relocation           */
} PGMHDR01;

typedef struct
{
        LONG    pi_tpalen ;             /*  length of tpa area          */
        UBYTE   *pi_tbase ;             /*  start addr of text seg      */
        LONG    pi_tlen ;               /*  length of text seg          */
        UBYTE   *pi_dbase ;             /*  start addr of data seg      */
        LONG    pi_dlen ;               /*  length of data seg          */
        UBYTE   *pi_bbase ;             /*  start addr of bss  seg      */
        LONG    pi_blen ;               /*  length of bss  seg          */
        LONG    pi_slen ;               /*  length of symbol table      */
} PGMINFO;

#endif /* PGHDR_H */
