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
AS = m68k-atari-mint-as
ASINC = -Iinclude
ASFLAGS = --register-prefix-optional -m68000 $(ASINC) 

# C compiler for MiNT
CC = m68k-atari-mint-gcc
INC = -Iinclude
# no -Wall for bdos right now...
CFLAGS = -O -mshort $(INC) 

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

# 
# source code in bios/
# Note: tosvars.o must be first object linked.

BIOSCSRC = conio.c kbd.c kprint.c xbios.c \
         bios.c clock.c fnt8x8.c fnt8x16.c kbq.c mfp.c version.c midi.c
BIOSSSRC = tosvars.s startup.s lineavars.s vectors.s aciavecs.s \
           linea.s conout.s

#
# source code in bdos/
#

BDOSCSRC = bdosinit.c console.c fsdrive.c fshand.c fsopnclo.c osmem.c \
         umem.c bdosmain.c fsbuf.c fsfat.c fsio.c iumem.c proc.c \
         bdosts.c fsdir.c fsglob.c fsmain.c kpgmld.c time.c
BDOSSSRC = rwa.s

#
# source code in util/
#

UTILCSRC = doprintf.c 
UTILSSRC = 

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
BIOSSOBJ = $(BIOSSSRC:%.s=obj/%.o)
BDOSCOBJ = $(BDOSCSRC:%.c=obj/%.o)
BDOSSOBJ = $(BDOSSSRC:%.s=obj/%.o)
UTILCOBJ = $(UTILCSRC:%.c=obj/%.o)
UTILSOBJ = $(UTILSSRC:%.s=obj/%.o)

SOBJ = $(BIOSSOBJ) $(BDOSSOBJ) $(UTILSOBJ)
COBJ = $(BIOSCOBJ) $(BDOSCOBJ) $(UTILCOBJ)
OBJECTS = $(SOBJ) $(COBJ) 

all:	emutos.img

emutos.img: $(OBJECTS) obj/end.o
	${LD} -oformat binary -o $@ $(OBJECTS) ${LDFLAGS} obj/end.o

obj/%.o : bios/%.c
	${CC} ${CFLAGS} -Wall -c -Ibios $< -o $@

obj/%.o : bios/%.s
	$(AS) $(ASFLAGS) $< -o $@

obj/%.o : bdos/%.c
	${CC} ${CFLAGS} -c -Ibdos $< -o $@

obj/%.o : bdos/%.s
	$(AS) $(ASFLAGS) $< -o $@

obj/%.o : util/%.c
	${CC} ${CFLAGS} -Wall -c -Iutil $< -o $@

obj/%.o : util/%.s
	$(AS) $(ASFLAGS) $< -o $@

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
	rm -f obj/*.o *~ */*~ core emutos.img

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
	  j=`basename $$i|sed -e s/s$$/o/`;\
	  $(AS) $(ASINC) --MD Makefile.tmp -o FOO $$i;\
	  sed -e "s/FOO/obj\\/$$j/" Makefile.tmp >>Makefile.new;\
	done
	rm -f FOO Makefile.tmp
	mv Makefile.new Makefile

# DO NOT DELETE

