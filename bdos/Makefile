#
# Makefile - Make BDOS part of GEMDOS
#
# Copyright (c) 2001 Martin Doering <doeringm@gmx.de>
#
# Authors:
#  MAD	   Martin Doering <doeringm@gmx.de>
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#


# Setup for crosscompiling under Linux only at this time :-(


# Assembler with options for Motorola like syntax (68000 cpu)
AS = m68k-atari-mint-as
ASFLAGS = --register-prefix-optional -m68000

# C compiler for MiNT
CC = m68k-atari-mint-gcc
CFLAGS = -c -O -mshort -I.
#CC = gcc
#CFLAGS = -c -O -I. -Wno-pointer-arith

# Archiver with options for Motorola like syntax (68000 cpu)
AR = m68k-atari-mint-ar
ARFLAGS = rs



OBJECTS = bdosinit.o bdosts.o console.o fsbuf.o \
	fsdir.o fsdrive.o fsfat.o fsglob.o \
	fshand.o fsio.o fsmain.o fsopnclo.o \
	iumem.o kpgmld.o osmem.o proc.o \
	bdosmain.o time.o umem.o rwa.o


all:	libbdos.a

libbdos.a: $(OBJECTS)
	 $(AR) $(ARFLAGS) $@ $(OBJECTS)


bdosmain.o: bdosmain.c
	${CC} ${CFLAGS} -c $?

bdosts.o: bdosts.c
	${CC} ${CFLAGS} -c $?

console.o: console.c
	${CC} ${CFLAGS} -c $?

fsdrive.o: fsdrive.c
	${CC} ${CFLAGS} -c $?

fsbuf.o: fsbuf.c
	${CC} ${CFLAGS} -c $?

fsdir.o: fsdir.c
	${CC} ${CFLAGS} -c $?

fsfat.o: fsfat.c
	${CC} ${CFLAGS} -c $?

fsglob.o: fsglob.c
	${CC} ${CFLAGS} -c $?

fshand.o: fshand.c
	${CC} ${CFLAGS} -c $?

fsio.o: fsio.c
	${CC} ${CFLAGS} -c $?

fsmain.o: fsmain.c
	${CC} ${CFLAGS} -c $?

fsopnclo.o: fsopnclo.c
	${CC} ${CFLAGS} -c $?

iumem.o: iumem.c
	${CC} ${CFLAGS} -c $?

kpgmld.o: kpgmld.c
	${CC} ${CFLAGS} -c $?

osmem.o: osmem.c
	${CC} ${CFLAGS} -c $?

proc.o: proc.c
	${CC} ${CFLAGS} -c $?

bdosinit.o: bdosinit.c
	${CC} ${CFLAGS} -c $?

time.o: time.c
	${CC} ${CFLAGS} -c $?

umem.o: umem.c
	${CC} ${CFLAGS} -c $?


# ==== Now all the assembler stuff ==========================================
rwa.o: rwa.s
	$(AS) $(ASFLAGS) $? -o $@


clean:
	rm -f *.o *~ *.a core
