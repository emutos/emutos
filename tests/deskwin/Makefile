# Copyright (C) 2026 The EmuTOS development team
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.

CC = m68k-atari-mint-gcc
CFLAGS = -Wall -mshort

all: winchk.tos

winchk.tos: winchk.c
	$(CC) $(CFLAGS) winchk.c -o winchk.tos

clean:
	$(RM) winchk.tos WINCHK.OUT

.PHONY : test
test: all
	@if command -v hatari >/dev/null 2>&1; then \
		./hatari.sh || exit 1; \
	else \
		echo "Skipped desktop window leak test with Hatari (not installed)."; \
	fi
