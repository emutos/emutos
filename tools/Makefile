#
# Makefile for user-facing tools (boot-delay, tos-lang-change)
#
# Copyright (C) 2022 The EmuTOS development team.
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

override CFLAGS += -O2

user-tools: boot-delay tos-lang-change

boot-delay: boot-delay.c

tos-lang-change: tos-lang-change.c

clean:
	-rm -f tos-lang-change boot-delay tos-lang-change.exe boot-delay.exe

install: user-tools
	mkdir -p dist/${ARCH}
	cp boot-delay${SUFFIX} tos-lang-change${SUFFIX} dist/${ARCH}/

.PHONY: clean user-tools install