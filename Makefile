#
# Makefile suitable for Linux and Cygwin setups
# only GCC (cross-mint) is supported. 
# some features like pattern substitution probably
# require GNU-make.
#
# targets are:
# - all: creates emutos.img
# - depend: updates automatic dependencies
# - clean
# - show: disassembles the emutos.img 
# - tgz: bundles all in a tgz archive.
#
# C code (C) and assembler (S) source go in directories 
# bios/, bdos/, util/ ; To add source code files, update
# the variables xxxxCSRC and xxxxSSRC below, 
# where xxxx is one of BIOS, BDOS, UTIL.
#
# Laurent.

# 
# compilation flags
#

# Linker with relocation information and binary output (image)
LD = m68k-atari-mint-ld
LDFLAGS = -Ttext=0xfc0000 -Tdata=0xfd0000 -Tbss=0x000000 \
	-L/usr/lib/gcc-lib/m68k-atari-mint/2.95.3/mshort -lgcc

# Assembler with options for Motorola like syntax (68000 cpu)
AS = m68k-atari-mint-gcc -x assembler
ASINC = -Iinclude
ASFLAGS = --register-prefix-optional -m68000 $(ASINC) 

# C compiler for MiNT
CC = m68k-atari-mint-gcc
INC = -Iinclude
# no -Wall for bdos right now...
CFLAGS = -O -mshort -m68000 $(INC) 

CPPFLAGS = $(INC)
CPP = $(CC) -E -x assembler

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

# 
# source code in bios/
# Note: tosvars.o must be first object linked.

BIOSCSRC = kprint.c xbios.c chardev.c bios.c clock.c \
           fnt8x16.c fnt8x8.c fnt6x6.c mfp.c version.c \
           midi.c ikbd.c sound.c floppy.c screen.c lineainit.c
BIOSSSRC = tosvars.S startup.S lineavars.S vectors.S aciavecs.S \
           memory.S linea.S conout.S

#
# source code in bdos/
#

BDOSCSRC = bdosinit.c console.c fsdrive.c fshand.c fsopnclo.c osmem.c \
         umem.c bdosmain.c fsbuf.c fsfat.c fsio.c iumem.c proc.c \
         bdosts.c fsdir.c fsglob.c fsmain.c kpgmld.c time.c
BDOSSSRC = rwa.S

#
# source code in util/
#

UTILCSRC = doprintf.c 
UTILSSRC = memset.S memmove.S

#
# everything should work fine below.
# P for PATH

PBIOSCSRC = $(BIOSCSRC:%=bios/%)
PBIOSSSRC = $(BIOSSSRC:%=bios/%)
PBDOSCSRC = $(BDOSCSRC:%=bdos/%)
PBDOSSSRC = $(BDOSSSRC:%=bdos/%)
PUTILCSRC = $(UTILCSRC:%=util/%)
PUTILSSRC = $(UTILSSRC:%=util/%)

CSRC = $(PBIOSCSRC) $(PBDOSCSRC) $(PUTILCSRC)
SSRC = $(PBIOSSSRC) $(PBDOSSSRC) $(PUTILSSRC)

BIOSCOBJ = $(BIOSCSRC:%.c=obj/%.o)
BIOSSOBJ = $(BIOSSSRC:%.S=obj/%.o)
BDOSCOBJ = $(BDOSCSRC:%.c=obj/%.o)
BDOSSOBJ = $(BDOSSSRC:%.S=obj/%.o)
UTILCOBJ = $(UTILCSRC:%.c=obj/%.o)
UTILSOBJ = $(UTILSSRC:%.S=obj/%.o)

SOBJ = $(BIOSSOBJ) $(BDOSSOBJ) $(UTILSOBJ)
COBJ = $(BIOSCOBJ) $(BDOSCOBJ) $(UTILCOBJ)
OBJECTS = $(SOBJ) $(COBJ) 

all:	emutos.img

emutos.img: $(OBJECTS) obj/end.o
	${LD} -oformat binary -o $@ $(OBJECTS) ${LDFLAGS} obj/end.o
	
etos192k.img: $(OBJECTS) obj/end.o
	${LD} -oformat binary -o emutos.tmp $(OBJECTS) ${LDFLAGS} obj/end.o
	dd if=/dev/zero of=empty.tmp bs=1024 count=192 
	cat empty.tmp >> emutos.tmp                    # Make real tos.img...
	dd if=emutos.tmp of=$@ bs=1024 count=192       # with right length.
	rm -f emutos.tmp empty.tmp
	
obj/%.o : bios/%.c
	${CC} ${CFLAGS} -Wall -c -Ibios $< -o $@

obj/%.o : bios/%.S
	${CC} ${CFLAGS} -Wall -c -Ibios $< -o $@

obj/%.o : bdos/%.c
	${CC} ${CFLAGS} -c -Ibdos $< -o $@

obj/%.o : bdos/%.S
	${CC} ${CFLAGS} -Wall -c -Ibios $< -o $@

obj/%.o : util/%.c
	${CC} ${CFLAGS} -Wall -c -Iutil $< -o $@

obj/%.o : util/%.S
	${CC} ${CFLAGS} -Wall -c -Ibios $< -o $@



#
# show
# Does just work without -oformat binary of Linker!!!
#
show: emutos.img
	$(OBJDUMP) --target=binary --architecture=m68k -D emutos.img

#
# clean and distclean 
# (distclean is called before creating a tgz archive)
#

clean:
	rm -f obj/*.o obj/*.s *~ */*~ core emutos.img

distclean: clean
	rm -f Makefile.bak
	$(MAKE) -C cli clean

#
# create a tgz archive named emutos-nnnnnn.tgz,
# where nnnnnn is the date.
#

HERE = $(shell pwd)
HEREDIR = $(shell basename $(HERE))
TGZ = $(shell echo $(HEREDIR)-`date +%y%m%d`|tr A-Z a-z).tgz

tgz:	distclean
	cd ..;\
	tar -cf - --exclude '*CVS' --exclude 'doc' $(HEREDIR) | gzip -c -9 >$(TGZ)

#
# automatic dependencies. (this is ugly)
#

depend: 
	cp Makefile Makefile.bak
	chmod +w Makefile
	sed -n '1,/^# DO NOT DELETE/p' < Makefile > Makefile.new
	for i in $(CSRC); do\
	  j=`basename $$i|sed -e s/c$$/o/`;\
	  (echo -n obj/;$(CC) -MM $(INC) $(DEF) $$i )>>Makefile.new;\
	done
	for i in $(ASMSRC); do\
	  j=`basename $$i|sed -e s/S$$/o/`;\
	  $(AS) $(ASINC) --MD Makefile.tmp -o FOO $$i;\
	  sed -e "s/FOO/obj\\/$$j/" Makefile.tmp >>Makefile.new;\
	done
	rm -f FOO Makefile.tmp
	mv Makefile.new Makefile

# DO NOT DELETE
