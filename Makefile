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
# the country. should be a lowercase two-letter code as found in
# the table in tools/mkheader.c and bios/country.c
#

COUNTRY = us

# 
# compilation flags
#

# indent flags
INDENT = indent -kr

# not needed, job taken by mkheader.
# BUILDDATE=$(shell LANG=C date +"%d. %b. %Y")

# Linker with relocation information and binary output (image)
LD = m68k-atari-mint-ld
LDFLAGS = -L/usr/lib/gcc-lib/m68k-atari-mint/2.95.3/mshort -lgcc
LDFLROM = -Ttext=0xfc0000 -Tbss=0x000000 
LDFLRAM =  -Ttext=0x10000 -Tbss=0x000000 
LDFLFAL = -Ttext=0xe00000 -Tbss=0x000000 


# Assembler with options for Motorola like syntax (68000 cpu)
AS = m68k-atari-mint-gcc -x assembler
ASINC = -Iinclude
ASFLAGS = --register-prefix-optional -m68000 $(ASINC) 

# C compiler for MiNT
CC = m68k-atari-mint-gcc
INC = -Iinclude
CFLAGS = -O -Wall -mshort -m68000 $(LOCALCONF) $(INC)

CPPFLAGS = $(INC)

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
           mouse.c initinfo.c cookie.c machine.c nvram.c country.c
BIOSSSRC = tosvars.S startup.S lineavars.S vectors.S aciavecs.S \
           processor.S memory.S linea.S conout.S detect.S panicasm.S

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

UTILCSRC = doprintf.c nls.c langs.c
UTILSSRC = memset.S memmove.S nlsasm.S

#
# source code in vdi/
#

VDICSRC = cbssdefs.c isin.c jmptbl.c lisastub.c lisatabl.c \
           monobj.c monout.c opnwkram.c seedfill.c text.c
VDISSRC = entry.S bitblt.S bltfrag.S copyrfm.S esclisa.S  \
          gsxasm1.S gsxasm2.S lisagem.S mouse.S newmono.S  \
          textblt.S tranfm.S

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
PCONSCSRC = $(CONSCSRC:%=cli/%)
PCONSSSRC = $(CONSSSRC:%=cli/%)
PVDICSRC = $(VDICSRC:%=vdi/%)
PVDISSRC = $(VDISSRC:%=vdi/%)

CSRC = $(PBIOSCSRC) $(PBDOSCSRC) $(PUTILCSRC) $(PVDICSRC) $(PCONSCSRC)
SSRC = $(PBIOSSSRC) $(PBDOSSSRC) $(PUTILSSRC) $(PVDISSRC) $(PCONSSSRC)

BIOSCOBJ = $(BIOSCSRC:%.c=obj/%.o)
BIOSSOBJ = $(BIOSSSRC:%.S=obj/%.o)
BDOSCOBJ = $(BDOSCSRC:%.c=obj/%.o)
BDOSSOBJ = $(BDOSSSRC:%.S=obj/%.o)
UTILCOBJ = $(UTILCSRC:%.c=obj/%.o)
UTILSOBJ = $(UTILSSRC:%.S=obj/%.o)
CONSCOBJ = $(CONSCSRC:%.c=obj/%.o)
CONSSOBJ = $(CONSSSRC:%.S=obj/%.o)
VDICOBJ  = $(VDICSRC:%.c=obj/%.o)
VDISOBJ  = $(VDISSRC:%.S=obj/%.o)

SOBJ = $(BIOSSOBJ) $(BDOSSOBJ) $(UTILSOBJ) $(CONSSOBJ) #$(VDISOBJ) 
COBJ = $(BIOSCOBJ) $(BDOSCOBJ) $(UTILCOBJ) $(CONSCOBJ) #$(VDICOBJ) 
OBJECTS = $(SOBJ) $(COBJ) 

#
# temporary variables, for internal Makefile use
#

TMP1 = make.tmp1
TMP2 = make.tmp2
TMP3 = make.tmp3

TMPS = $(TMP1) $(TMP2) $(TMP3)

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
	@echo "falcon  etosfalc.img, i.e. emutos beginning at 0xe00000" 
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
# Aranym or Falcon
#

falcon: etosfalc.img

etosfalc.tmp: $(OBJECTS)
	$(LD) -oformat binary -o $@ $(OBJECTS) $(LDFLAGS) $(LDFLFAL)

etosfalc.img: etosfalc.tmp
	dd if=/dev/zero of=empty.tmp bs=1024 count=512 
	cat empty.tmp >> etosfalc.tmp                    # Make real tos.img...
	dd if=etosfalc.tmp of=$@ bs=1024 count=512       # with right length.
	rm -f etosfalc.tmp empty.tmp

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

dumpkbd.prg: obj/minicrt.o obj/memmove.o obj/dumpkbd.o
	$(LD) -s -o $@ $^ $(LDFLAGS)

keytbl2c$(EXE) : tools/keytbl2c.c
	$(NATIVECC) -o $@ $<

#
# NLS support
#

POFILES = po/fr.po

bug$(EXE): tools/bug.c
	$(NATIVECC) -o $@ $<

util/langs.c: $(POFILES) po/LINGUAS bug$(EXE) po/messages.pot
	./bug$(EXE) make
	mv langs.c $@

po/messages.pot: bug$(EXE)
	./bug$(EXE) xgettext

#
# OS header
# (need_header is an ugly trick to make sure the header is generated 
# each time)

need_header:
	@touch $@

obj/startup.o: bios/header.h

bios/header.h: mkheader$(EXE) need_header
	./mkheader$(EXE) $(COUNTRY)
	@rm -f need_header

mkheader$(EXE): tools/mkheader.c
	$(NATIVECC) -o $@ $<

#
# automatic build rules
#

obj/%.o : bios/%.c
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : bios/%.S
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : bdos/%.c
	${CC} ${CFLAGS} -c -Ibdos $< -o $@

obj/%.o : bdos/%.S
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : util/%.c
	${CC} ${CFLAGS} -c -Iutil $< -o $@

obj/%.o : util/%.S
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : cli/%.c
	${CC} ${CFLAGS} -c $< -o $@

obj/%.o : cli/%.S
	${CC} ${CFLAGS} -c $< -o $@

obj/%.o : vdi/%.c
	${CC} ${CFLAGS} -c $< -o $@

obj/%.o : vdi/%.S
	${CC} ${CFLAGS} -c $< -o $@

#
# make bios.dsm will create an assembly-only of bios.c
#

%.dsm : bios/%.c
	${CC} ${CFLAGS} -S -Ibios $< -o $@

%.dsm : bdos/%.c
	${CC} ${CFLAGS} -S -Ibdos $< -o $@

%.dsm : util/%.c
	${CC} ${CFLAGS} -S -Iutil $< -o $@

%.dsm : cli/%.c
	${CC} ${CFLAGS} -S -Iutil $< -o $@

%.dsm : vdi/%.c
	${CC} ${CFLAGS} -S -Iutil $< -o $@

#
# dsm, show
#

DESASS = dsm.txt

dsm: $(DESASS)

show: $(DESASS)
	cat $(DESASS)

map: $(OBJECTS)
	${LD} -Map map -oformat binary -o emutos.img $(OBJECTS) $(LDFLAGS) \
		$(LDFLROM)

$(DESASS): map
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
	rm -f ramtos.img boot.prg etos192k.img etosfalc.img mkflop$(EXE) 
	rm -f bootsect.img emutos.st date.prg dumpkbd.prg keytbl2c$(EXE)
	rm -f bug$(EXE) po/messages.pot util/langs.c bios/header.h
	rm -f mkheader$(EXE) tounix$(EXE) $(TMPS) *.dsm

distclean: clean nodepend
	rm -f Makefile.bak '.#'* */'.#'* 
	$(MAKE) -C cli clean

#
# indent - indents the files except when there are warnings
# checkindent - check for indent warnings, but do not alter files.
#

INDENTFILES = bdos/*.c bios/*.c util/*.c tools/*.c

checkindent:
	@err=0 ; \
	rm -f $(TMP3); touch $(TMP3); \
	for i in $(INDENTFILES) ; do \
		$(INDENT) <$$i 2>$(TMP1) >/dev/null; \
		if cmp -s $(TMP1) $(TMP3) ; then : ; else\
			err=`expr $$err + 1`; \
			echo in $$i:; \
			cat $(TMP1); \
		fi \
	done ; \
	rm -f $(TMP1) $(TMP3); \
	if [ $$err -ne 0 ] ; then \
		echo indent issued warnings on $$err 'file(s)'; \
		false; \
	else \
		echo done.; \
	fi

indent:
	@err=0 ; \
	rm -f $(TMP3); touch $(TMP3); \
	for i in $(INDENTFILES) ; do \
		$(INDENT) <$$i 2>$(TMP1) | expand >$(TMP2); \
		if cmp -s $(TMP1) $(TMP3) ; then \
			if cmp -s $(TMP2) $$i ; then : ; else \
				echo indenting $$i; \
				mv $$i $$i~; \
				mv $(TMP2) $$i; \
			fi \
		else \
			err=`expr $$err + 1`; \
			echo in $$i:; \
			cat $(TMP1); \
		fi \
	done ; \
	rm -f $(TMP1) $(TMP2) $(TMP3); \
	if [ $$err -ne 0 ] ; then \
		echo $$err 'file(s)' untouched because of warnings; \
		false; \
	fi


#
# cvsready
#

expand:
	@for i in `grep -l '	' */*.[chS]` ; do \
		echo expanding $$i ; \
		expand <$$i >$(TMP1); \
		mv $(TMP1) $$i; \
	done

tounix$(EXE): tools/tounix.c
	$(NATIVECC) -o $@ $<

# LVL - I checked that both on Linux and Cygwin passing more than 10000 
# arguments on the command line works fine. On other systems it might be 
# necessary to adopt another technique, for example using an find | xargs 
# approach like that below:
#
# HERE = $(shell pwd)
# crlf:	tounix$(EXE)
#     find . -name CVS -prune -or -not -name '*~' | xargs $(HERE)/tounix$(EXE)

crlf: tounix$(EXE)
	./tounix$(EXE) * bios/* bdos/* doc/* util/* tools/* po/* include/*

cvsready: expand crlf nodepend

#
# create a tgz archive named emutos-nnnnnn.tgz,
# where nnnnnn is the date.
#

HERE = $(shell pwd)
HEREDIR = $(shell basename $(HERE))
TGZ = $(shell echo $(HEREDIR)-`date +%y%m%d`|tr A-Z a-z).tgz

tgz:	distclean
	cd ..;\
	tar -cf - --exclude '*CVS' $(HEREDIR) | gzip -c -9 >$(TGZ)

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
