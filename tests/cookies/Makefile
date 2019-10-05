# Copyright (C) 2019 The EmuTOS development team
#
# Authors:
#  THH   Thomas Huth
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.

CC = m68k-atari-mint-gcc
CFLAGS = -Wall -mshort -I../include

all: cj2txt.tos

cj2txt.tos: cj2txt.c
	$(CC) $(CFLAGS) cj2txt.c -o cj2txt.tos

clean:
	$(RM) cj2txt.tos COOKIES.TXT

.PHONY : test
test: all
	@if command -v hatari >/dev/null 2>&1; then \
		./hatari.sh || exit 1; \
	else \
		echo "Skipped cookie test with Hatari (not installed)."; \
	fi
	@if command -v aranym >/dev/null 2>&1; then \
		if command -v mcopy >/dev/null 2>&1; then \
			./aranym.sh || exit 1; \
		else \
			echo "Skipped cookie test with Aranym (no mtools)."; \
		fi \
	else \
		echo "Skipped cookie test with Aranym (not installed)."; \
	fi
