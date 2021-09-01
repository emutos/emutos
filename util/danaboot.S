/*
 * danaboot.S - AlphaSmart Dana decompressing RAM booter
 *
 * Copyright (C) 2021 David Given
 *
 * Authors:
 *  DG    David Given
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

entry:
	lea 0x100000, sp
	lea 0x2140, a4			// text area start
	lea data_start, a5		// compressed data start
	bsr inflate
	jmp 0x2140

#include "inflate.S"

data_start:
.incbin "emutos.img.deflate"
data_end:

