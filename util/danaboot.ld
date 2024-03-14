/*
 * danaboot.ld - Custom linker script for EmuTOS on the Dana
 *
 * Copyright (C) 2021 David Given
 *
 * Authors:
 *  DG    David Given
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

OUTPUT_FORMAT(binary)

MEMORY
{
	ram : ORIGIN = 0, LENGTH = 0x100000
	rom : ORIGIN = 0x100000, LENGTH = 1M
}

SECTIONS
{
	rom : {
	    obj/danaboot.o
	} > rom
}

