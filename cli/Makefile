
# Setup for crosscompiling under Linux only at this time :-(

BUILDDATE=$(shell LANG=C date +"%d. %b. %Y")

# Linker 
LD = m68k-atari-mint-ld
LDFLAGS = -L/usr/lib/gcc-lib/m68k-atari-mint/2.95.3/mshort -lgcc

# Assembler with options for Motorola like syntax (68000 cpu)
AS = m68k-atari-mint-as
ASFLAGS = --register-prefix-optional -m68000

# C compiler for MiNT
CC = m68k-atari-mint-gcc
CFLAGS = -c -O -mshort  -m68000 -Wall -DBUILDDATE="\"$(BUILDDATE)\"" -DNO_ROM
#CC = gcc
#CFLAGS = -c -O -I./include -Wall

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

OBJECTS = coma.o command.o

all:	command.prg


command.prg: $(OBJECTS)
	${LD} -o $@ $(OBJECTS) ${LDFLAGS}


command.o: command.c
	${CC} ${CFLAGS} -c $?

coma.o: coma.S
	${CC} ${CFLAGS} -c $?
#	$(AS) $(ASFLAGS) $? -o $@

clean:
	rm -f *.o *~ lib*.a core command.prg
