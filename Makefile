##############################################################################
# EmuTOS Makefile
##############################################################################

TOP = $(shell pwd)

######################## Additional Flags section ############################

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

all:	emutos.img

emutos.img:
	$(MAKE) -C bios
	$(MAKE) -C bdos
	$(MAKE) -C link
	mv link/gemdos.img emutos.img

        
# Does just work without -oformat binary of Linker!!!
show: emutos.img
	$(OBJDUMP) --target=binary --architecture=m68k -D emutos.img


clean:
	$(MAKE) -C bios clean
	$(MAKE) -C bdos clean
	$(MAKE) -C link clean
	rm -f emutos.img *~

