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
# crude machine detection (Unix or Cygwin)
#

ifneq (,$(findstring CYGWIN,$(shell uname)))
# CYGWIN-dependent stuff
EXE = .exe
else
# ordinary Unix stuff
EXE = 
BUILDDATE=$(shell LANG=C date +"%d. %b. %Y")
endif

#
# test for localconf.h
#

ifeq (localconf.h,$(shell ls localconf.h))
LOCALCONF = -DLOCALCONF
else
LOCALCONF = 
endif

# 
# compilation flags
#

# Linker with relocation information and binary output (image)
LD = m68k-atari-mint-ld
LDFLAGS = -L/usr/lib/gcc-lib/m68k-atari-mint/2.95.3/mshort -lgcc
LDFLROM = -Ttext=0xfc0000 -Tdata=0xfd0000 -Tbss=0x000000 
LDFLRAM = -Ttext=0x10000 -Tdata=0x20000 -Tbss=0x000000 


# Assembler with options for Motorola like syntax (68000 cpu)
AS = m68k-atari-mint-gcc -x assembler
ASINC = -Iinclude
ASFLAGS = --register-prefix-optional -m68000 $(ASINC) 

# C compiler for MiNT
CC = m68k-atari-mint-gcc
INC = -Iinclude
# no -Wall for bdos right now...
CFLAGS = -O -mshort -m68000 $(LOCALCONF) -DBUILDDATE="\"$(BUILDDATE)\"" $(INC) 
CFLAGS020 = -O -mshort -m68020 $(LOCALCONF) -DBUILDDATE="\"$(BUILDDATE)\"" $(INC) 

CPPFLAGS = $(INC)
CPP = $(CC) -E -x assembler

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

# the native C compiler, for tools
NATIVECC = gcc -Wall

# 
# source code in bios/
# Note: tosvars.o must be first object linked.

BIOSCSRC = kprint.c xbios.c chardev.c bios.c clock.c \
           fnt8x16.c fnt8x8.c fnt6x6.c mfp.c version.c \
           midi.c ikbd.c sound.c floppy.c screen.c lineainit.c \
           initinfo.c
BIOSSSRC = tosvars.S startup.S lineavars.S vectors.S aciavecs.S \
           processor.S memory.S linea.S conout.S

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
# source code in cli/ for EmuTOS console EmuCON
#

CONSCSRC = command.c
CONSSSRC = coma.S

#
# everything should work fine below.
# P for PATH

PBIOSCSRC = $(BIOSCSRC:%=bios/%)
PBIOSSSRC = $(BIOSSSRC:%=bios/%)
PBDOSCSRC = $(BDOSCSRC:%=bdos/%)
PBDOSSSRC = $(BDOSSSRC:%=bdos/%)
PUTILCSRC = $(UTILCSRC:%=util/%)
PUTILSSRC = $(UTILSSRC:%=util/%)
PCONSCSRC = $(CONSCSRC:%=util/%)
PCONSSSRC = $(CONSSSRC:%=util/%)

CSRC = $(PBIOSCSRC) $(PBDOSCSRC) $(PUTILCSRC) $(PCONSCSRC)
SSRC = $(PBIOSSSRC) $(PBDOSSSRC) $(PUTILSSRC) $(PCONSSSRC)

BIOSCOBJ = $(BIOSCSRC:%.c=obj/%.o)
BIOSSOBJ = $(BIOSSSRC:%.S=obj/%.o)
BDOSCOBJ = $(BDOSCSRC:%.c=obj/%.o)
BDOSSOBJ = $(BDOSSSRC:%.S=obj/%.o)
UTILCOBJ = $(UTILCSRC:%.c=obj/%.o)
UTILSOBJ = $(UTILSSRC:%.S=obj/%.o)
CONSCOBJ = $(CONSCSRC:%.c=obj/%.o)
CONSSOBJ = $(CONSSSRC:%.S=obj/%.o)

SOBJ = $(BIOSSOBJ) $(BDOSSOBJ) $(UTILSOBJ) $(CONSSOBJ)
COBJ = $(BIOSCOBJ) $(BDOSCOBJ) $(UTILCOBJ) $(CONSCOBJ)
OBJECTS = $(SOBJ) $(COBJ) 

#
# production targets 
# 

all:	emutos.img

help:	
	@echo "target  meaning"
	@echo "------  -------"
	@echo "help    this help message"
	@echo "all     emutos.img, a TOS 1 ROM image (0xFC0000)"
	@echo "192     etos192k.img, i.e. emutos.img padded to size 192 KB"
	@echo "ram     ramtos.img + boot.prg, a RAM tos"
	@echo "flop    emutos.st, a bootable floppy with RAM tos"
	@echo "clean"
	@echo "tgz     bundles all except doc into a tgz archive"
	@echo "depend  creates dependancy section in Makefile"
	@echo "dsm     dsm.txt, an edited desassembly of emutos.img"

emutos.img: $(OBJECTS)
	${LD} -oformat binary -o $@ $(OBJECTS) ${LDFLAGS} $(LDFLROM)

#
# 192
#

192: etos192k.img

etos192k.img: emutos.img
	cp emutos.img emutos.tmp
	dd if=/dev/zero of=empty.tmp bs=1024 count=192 
	cat empty.tmp >> emutos.tmp                    # Make real tos.img...
	dd if=emutos.tmp of=$@ bs=1024 count=192       # with right length.
	rm -f emutos.tmp empty.tmp

#
# ram
#

ram: ramtos.img boot.prg

ramtos.img: $(OBJECTS)
	$(LD) -oformat binary -o $@ $(OBJECTS) $(LDFLAGS) $(LDFLRAM)

boot.prg: obj/minicrt.o obj/boot.o obj/bootasm.o
	$(LD) -s -o $@ obj/minicrt.o obj/boot.o obj/bootasm.o $(LDFLAGS) 

#
# flop
#

flop : emutos.st

emutos.st: mkflop$(EXE) bootsect.img ramtos.img
	./mkflop$(EXE)

bootsect.img : obj/bootsect.o
	$(LD) -oformat binary -o $@ obj/bootsect.o

mkflop$(EXE) : tools/mkflop.c
	$(NATIVECC) -o $@ $<

#
# Misc utilities
#

date.prg: obj/minicrt.o obj/doprintf.o obj/date.o
	$(LD) -s -o $@ obj/minicrt.o obj/doprintf.o obj/date.o $(LDFLAGS) 

#
# automatic build rules
#

obj/%.o : bios/%.c
	${CC} ${CFLAGS} -Wall -c -Ibios $< -o $@

obj/processor.o : bios/processor.S
	${CC} ${CFLAGS020} -Wall -c -Ibios $< -o $@

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

obj/%.o : cli/%.c
	${CC} ${CFLAGS} -Wall -c $< -o $@

obj/%.o : cli/%.S
	${CC} ${CFLAGS} -Wall -c $< -o $@



#
# dsm, show
#

DESASS = dsm.txt

dsm: $(DESASS)

show: $(DESASS)
	cat $(DESASS)

TMP1 = tmp1
TMP2 = tmp2

map: $(OBJECTS)
	${LD} -Map map -oformat binary -o /dev/null $(OBJECTS) $(LDFLAGS) \
		$(LDFLROM)

$(DESASS): map emutos.img
	$(OBJDUMP) --target=binary --architecture=m68k \
	--adjust-vma=0x00fc0000 -D emutos.img | grep '^  f' \
	| sed -e 's/^  //' -e 's/:	/: /' > $(TMP1)
	grep '^ \+0x' map | sort | sed -e 's/ \+/ /g' \
	| sed -e 's/^ 0x00//' -e 's/ /:  /' > $(TMP2)
	cat $(TMP1) $(TMP2) | LC_ALL=C sort > $@
	rm $(TMP1) $(TMP2)

#
# clean and distclean 
# (distclean is called before creating a tgz archive)
#

clean:
	rm -f obj/*.o obj/*.s *~ */*~ core emutos.img map $(DESASS)
	rm -f ramtos.img boot.prg etos192.img mkflop$(EXE) bootsect.img
	rm -f emutos.st date.prg

distclean: clean nodepend
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

nodepend:
	cp Makefile Makefile.bak
	chmod +w Makefile
	sed -n '1,/^# DO NOT DELETE/p' < Makefile > Makefile.new
	mv Makefile.new Makefile

depend: 
	cp Makefile Makefile.bak
	chmod +w Makefile
	sed -n '1,/^# DO NOT DELETE/p' < Makefile > Makefile.new
	for i in $(CSRC); do\
	  j=`basename $$i|sed -e s/c$$/o/`;\
	  (echo -n obj/;$(CC) -MM $(INC) $(DEF) $$i )>>Makefile.new;\
	done
	for i in $(SSRC); do\
	  j=`basename $$i|sed -e s/S$$/o/`;\
	  (echo -n obj/;$(CC) -MM $(INC) $(DEF) $$i )>>Makefile.new;\
	done
	rm -f Makefile.tmp
	mv Makefile.new Makefile

# DO NOT DELETE
