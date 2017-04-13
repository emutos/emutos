#
# Makefile for EmuCON2
#
# Copyright (C) 2013-2017 The EmuTOS development team.
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

include ../version.mk

TARGET = emucon2.tos
LIBS = -nostartfiles -nostdlib -lgcc
CC = m68k-atari-mint-gcc
CPPFLAGS = -I../include -DUSE_STATIC_INLINES=0 -DSTANDALONE_CONSOLE
CFLAGS = $(CPPFLAGS) -mshort -Os -Wall -Wundef -Wold-style-definition

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = cmdasm.o cmdmain.o cmdedit.o cmdexec.o cmdint.o cmdparse.o cmdutil.o version.o
HEADERS = cmd.h

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -s -o $@

clean:
	-rm -f *.o version.c
	-rm -f $(TARGET)

#
# version string
#

# This temporary file is always generated
version2.c:
	@echo '/* Generated from Makefile */' > $@
	@echo 'const char version[] = "$(VERSION)";' >> $@

# If the official version file is different than the temporary one, update it
version.c: version2.c
	@if ! cmp -s $@ $< ; then \
	  echo '# Updating $@ with VERSION=$(VERSION)' ; \
	  cp $< $@ ; \
	fi ; \
	rm $<
