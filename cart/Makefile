#
# Makefile - Compile STonX's cartridge from its assembler sources
#
# Copyright (c) 2001, STonX Team - http://stonx.sourceforge.net
#
# Authors:
#  MAD   Martin Doering 
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

# Setup for crosscompiling under Linux only at this time. :-(
# You don't need to change anything in here.              :-)
# See cartridge.txt for how to do it!


# Assembler with options for Motorola like syntax (68000 cpu)
AS = m68k-atari-mint-as
ASFLAGS = --register-prefix-optional -m68000

# Linker with relocation information and binary output (image)
LD = m68k-atari-mint-ld
LDFLAGS =-Ttext=0xfa0000 -oformat binary

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

cartridge.img: cartridge.o 
	${LD} ${LDFLAGS} -o $@ cartridge.o

# Does just work without -oformat binary of Linker!!!
show: cartridge.img
	$(OBJDUMP) --target=binary --architecture=m68k  --adjust-vma=0xfa0000 -D cartridge.img

# Assembling the assembler code
cartridge.o: cartridge.s
	$(AS) $(ASFLAGS) $? -o $@
        
clean:
	rm -f *.o *~ cartridge.img core
