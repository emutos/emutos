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
           midi.c ikbd.c sound.c floppy.c screen.c lineainit.c \
           initinfo.c
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

help:	
	@echo "target  meaning"
	@echo "------  -------"
	@echo "help    this help message"
	@echo "all     emutos.img, a TOS 1 ROM image (0xFC0000)"
	@echo "192     etos192k.img, i.e. emutos.img padded to size 192 KB"
	@echo "ram     ramtos.img + boot.prg, a RAM tos"
	@echo "flop    emutos.st, a bootable floppy with RAM tos (TODO)"
	@echo "clean"
	@echo "tgz     bundles all except doc into a tgz archive"
	@echo "depend  creates dependancy section in Makefile"
	@echo "dsm     dsm.txt, an edited desassembly of emutos.img"

emutos.img: $(OBJECTS)
	${LD} -oformat binary -o $@ $(OBJECTS) ${LDFLAGS} $(LDFLROM)

192: etos192k.img

ram: ramtos.img boot.prg

ramtos.img: $(OBJECTS)
	$(LD) -oformat binary -o $@ $(OBJECTS) $(LDFLAGS) $(LDFLRAM)

boot.prg: obj/minicrt.o obj/boot.o obj/bootasm.o
	$(LD) -s -o $@ obj/minicrt.o obj/boot.o obj/bootasm.o $(LDFLAGS) 

etos192k.img: emutos.img
	cp emutos.img emutos.tmp
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
	rm -f ramtos.img boot.prg etos192.img

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
	for i in $(SSRC); do\
	  j=`basename $$i|sed -e s/S$$/o/`;\
	  (echo -n obj/;$(CC) -MM $(INC) $(DEF) $$i )>>Makefile.new;\
	done
	rm -f Makefile.tmp
	mv Makefile.new Makefile

# DO NOT DELETE
obj/kprint.o: bios/kprint.c bios/portab.h bios/bios.h
obj/xbios.o: bios/xbios.c bios/portab.h bios/kprint.h bios/iorec.h \
 bios/tosvars.h bios/ikbd.h bios/midi.h bios/mfp.h bios/screen.h \
 bios/sound.h bios/floppy.h include/asm.h
obj/chardev.o: bios/chardev.c bios/portab.h bios/bios.h bios/gemerror.h \
 bios/kprint.h bios/chardev.h
obj/bios.o: bios/bios.c bios/portab.h bios/bios.h bios/gemerror.h \
 bios/config.h bios/kprint.h bios/tosvars.h bios/initinfo.h \
 bios/ikbd.h bios/midi.h bios/mfp.h bios/floppy.h bios/sound.h \
 bios/screen.h bios/vectors.h include/asm.h bios/chardev.h
obj/clock.o: bios/clock.c bios/portab.h bios/bios.h bios/kprint.h
obj/fnt8x16.o: bios/fnt8x16.c bios/portab.h bios/fontdef.h
obj/fnt8x8.o: bios/fnt8x8.c bios/portab.h bios/fontdef.h
obj/fnt6x6.o: bios/fnt6x6.c bios/portab.h bios/fontdef.h
obj/mfp.o: bios/mfp.c bios/portab.h bios/bios.h bios/kprint.h bios/mfp.h \
 bios/tosvars.h
obj/version.o: bios/version.c
obj/midi.o: bios/midi.c bios/portab.h bios/bios.h bios/kprint.h \
 bios/acia.h bios/iorec.h include/asm.h bios/midi.h
obj/ikbd.o: bios/ikbd.c bios/portab.h bios/bios.h bios/acia.h \
 bios/kprint.h bios/tosvars.h bios/iorec.h include/asm.h bios/ikbd.h \
 bios/sound.h
obj/sound.o: bios/sound.c bios/portab.h bios/sound.h bios/psg.h \
 include/asm.h
obj/floppy.o: bios/floppy.c bios/portab.h bios/floppy.h bios/dma.h \
 bios/fdc.h bios/psg.h bios/mfp.h include/asm.h bios/tosvars.h \
 bios/kprint.h
obj/screen.o: bios/screen.c bios/screen.h bios/portab.h bios/tosvars.h \
 include/asm.h
obj/lineainit.o: bios/lineainit.c bios/tosvars.h bios/portab.h \
 bios/lineavars.h bios/fontdef.h bios/kprint.h
obj/initinfo.o: bios/initinfo.c bios/portab.h bios/kprint.h \
 bios/lineavars.h bios/tosvars.h bios/initinfo.h
obj/bdosinit.o: bdos/bdosinit.c bdos/gportab.h bdos/bios.h
obj/console.o: bdos/console.c bdos/gportab.h bdos/fs.h bdos/bios.h
obj/fsdrive.o: bdos/fsdrive.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h bdos/../bios/kprint.h
obj/fshand.o: bdos/fshand.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h
obj/fsopnclo.o: bdos/fsopnclo.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h include/btools.h
obj/osmem.o: bdos/osmem.c bdos/gportab.h bdos/fs.h bdos/bios.h bdos/mem.h \
 bdos/gemerror.h
obj/umem.o: bdos/umem.c bdos/gportab.h bdos/fs.h bdos/bios.h bdos/mem.h \
 bdos/gemerror.h bdos/../bios/kprint.h
obj/bdosmain.o: bdos/bdosmain.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h bdos/../bios/kprint.h
obj/fsbuf.o: bdos/fsbuf.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h
obj/fsfat.o: bdos/fsfat.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h
obj/fsio.o: bdos/fsio.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h
obj/iumem.o: bdos/iumem.c bdos/gportab.h bdos/fs.h bdos/bios.h bdos/mem.h \
 bdos/gemerror.h bdos/../bios/kprint.h
obj/proc.o: bdos/proc.c bdos/gportab.h bdos/fs.h bdos/bios.h bdos/mem.h \
 bdos/gemerror.h include/btools.h bdos/../bios/kprint.h
obj/bdosts.o: bdos/bdosts.c
obj/fsdir.o: bdos/fsdir.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h include/btools.h bdos/../bios/kprint.h
obj/fsglob.o: bdos/fsglob.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h
obj/fsmain.o: bdos/fsmain.c bdos/gportab.h bdos/fs.h bdos/bios.h \
 bdos/gemerror.h
obj/kpgmld.o: bdos/kpgmld.c bdos/gportab.h bdos/bdos.h bdos/fs.h \
 bdos/bios.h bdos/mem.h bdos/gemerror.h bdos/pghdr.h include/btools.h \
 bdos/../bios/kprint.h
obj/time.o: bdos/time.c bdos/gportab.h bdos/gemerror.h bdos/bios.h
obj/doprintf.o: util/doprintf.c
obj/tosvars.o: bios/tosvars.S
obj/startup.o: bios/startup.S bios/asmdefs.h bios/config.h
obj/lineavars.o: bios/lineavars.S
obj/vectors.o: bios/vectors.S
obj/aciavecs.o: bios/aciavecs.S
obj/memory.o: bios/memory.S
obj/linea.o: bios/linea.S
obj/conout.o: bios/conout.S
obj/rwa.o: bdos/rwa.S
obj/memset.o: util/memset.S
obj/memmove.o: util/memmove.S
