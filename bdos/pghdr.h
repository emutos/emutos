/*
 * pghdr.h - Describes program format as stored on disk.
 *
 * Copyright (C) 2001 Lineo, Inc.
 * Copyright (C) 2015-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef PGHDR_H
#define PGHDR_H

/* Represents the header of a program on disk */
typedef struct
{
        /*  magic number is already skipped as can_load returns 2 */
        LONG    h01_tlen ;      /*  length of text segment    */
        LONG    h01_dlen ;      /*  length of data segment    */
        LONG    h01_blen ;      /*  length of bss  segment    */
        LONG    h01_slen ;      /*  length of symbol table    */
        LONG    h01_res1 ;      /*  reserved - always zero    */
        ULONG   h01_flags ;     /*  flags                     */
        WORD    h01_abs ;       /*  not zero if no relocation */
} PGMHDR01;

#endif /* PGHDR_H */
