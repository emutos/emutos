#
# Makefile suitable for Linux and Cygwin setups
# only GCC (cross-mint) is supported. 
# some features like pattern substitution probably
# require GNU-make.
#
# targets are:
# - all:    creates emutos.img
# - 192:    creates emutos as a 192kb rom image file
# - 512:    creates a falcon compatible 512 kb rom image
# - falcon: the same like above
# - depend: updates automatic dependencies
# - clean:  removes all compiled and tmeporary stuff
# - show:   disassembles the emutos.img 
# - tgz:    bundles all in a tgz archive.
#
# C code (C) and assembler (S) source go in directories 
# bios/, bdos/, util/ ; To add source code files, update
# the variables xxxxCSRC and xxxxSSRC below, 
# where xxxx is one of BIOS, BDOS, UTIL.
#
# Laurent.


#
# the country. should be a lowercase two-letter code as found in
# the table in tools/mkheader.c and bios/country.c
#

COUNTRY = us

#
# Choose the user interface that should be included into EmuTOS
# (0=command line "EmuCON" , 1=AES)

WITH_AES = 1


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
LDFLFAL = -Ttext=0xe00000 -Tbss=0x000000 

# (relocation for RAM TOS is derived dynamically from the BSS size)

# Assembler with options for Motorola like syntax (68000 cpu)
AS = m68k-atari-mint-gcc -x assembler
ASINC = -Iinclude
ASFLAGS = --register-prefix-optional -m68000 $(ASINC) 

# C compiler for MiNT
CC = m68k-atari-mint-gcc
INC = -Iinclude
CFLAGS = -O2 -fomit-frame-pointer -Wall -mshort -m68000 $(LOCALCONF) $(INC)

CPPFLAGS = $(INC)

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

# the native C compiler, for tools
NATIVECC = gcc -Wall

# 
# source code in bios/
# Note: tosvars.o must be first object linked.

BIOSCSRC = kprint.c xbios.c chardev.c blkdev.c bios.c clock.c \
           fnt8x16.c fnt8x8.c fnt6x6.c mfp.c version.c \
           midi.c ikbd.c sound.c floppy.c disk.c screen.c lineainit.c \
           mouse.c initinfo.c cookie.c machine.c nvram.c country.c \
	   fntlat2_6.c fntlat2_8.c fntlat2_16.c biosmem.c
BIOSSSRC = tosvars.S startup.S lineavars.S vectors.S aciavecs.S \
           processor.S memory.S linea.S conout.S mousedrv.S \
           detect.S panicasm.S \
           kprintasm.S

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

UTILCSRC = doprintf.c nls.c langs.c string.c
UTILSSRC = memset.S memmove.S nlsasm.S setjmp.S

#
# source code in vdi/
#

VDICSRC = cbssdefs.c isin.c jmptbl.c lisastub.c lisatabl.c \
           monobj.c monout.c opnwkram.c seedfill.c text.c cbssdefs.c
VDISSRC = entry.S bitblt.S bltfrag.S copyrfm.S esclisa.S  \
          gsxasm1.S gsxasm2.S lisagem.S vdimouse.S newmono.S  \
          textblt.S tranfm.S gsxvars.S

#
# source code in aes/
#

AESCSRC = gemaplib.c gemasync.c gembase.c gemctrl.c gemdisp.c gemevlib.c \
   gemflag.c gemfmalt.c gemfmlib.c gemfslib.c gemglobe.c gemgraf.c gemgrlib.c \
   gemgsxif.c geminit.c geminput.c gemmnlib.c gemobed.c gemobjop.c gemoblib.c \
   gempd.c gemqueue.c gemrslib.c gemsclib.c gemshlib.c gemsuper.c gemwmlib.c \
   gemwrect.c optimize.c rectfunc.c gemdos.c gem_rsc.c
AESSSRC = gemstart.S gemdosif.S gemasm.S gsx2.S large.S optimopt.S

#
# source code in desk/
#

DESKCSRC = deskact.c deskapp.c deskdir.c deskfpd.c deskfun.c \
    deskglob.c deskinf.c deskins.c deskmain.c deskobj.c deskpro.c \
    deskrsrc.c desksupp.c deskwin.c gembind.c icons.c
    #taddr.c deskgraf.c deskgsx.c desk_rsc.c
DESKSSRC = deskstart.S

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
PAESCSRC = $(AESCSRC:%=aes/%)
PAESSSRC = $(AESSSRC:%=aes/%)
PDESKCSRC = $(DESKCSRC:%=desk/%)
PDESKSSRC = $(DESKSSRC:%=desk/%)

CSRC = $(PBIOSCSRC) $(PBDOSCSRC) $(PUTILCSRC) \
       $(PVDICSRC) $(PAESCSRC) $(PCONSCSRC) $(PDESKCSRC)
SSRC = $(PBIOSSSRC) $(PBDOSSSRC) $(PUTILSSRC) \
       $(PVDISSRC) $(PAESSSRC) $(PCONSSSRC) $(PDESKSSRC)

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
AESCOBJ  = $(AESCSRC:%.c=obj/%.o)
AESSOBJ  = $(AESSSRC:%.S=obj/%.o)
DESKCOBJ  = $(DESKCSRC:%.c=obj/%.o)
DESKSOBJ  = $(DESKSSRC:%.S=obj/%.o)

# Selects the user interface (EmuCON or AES):
ifeq ($(WITH_AES),0)
UICOBJ = $(CONSCOBJ)
UISOBJ = $(CONSSOBJ)
else
UICOBJ = $(AESCOBJ) $(DESKCOBJ)
UISOBJ = $(AESSOBJ) $(DESKSOBJ)
endif

COBJ = $(BIOSCOBJ) $(BDOSCOBJ) $(UTILCOBJ) $(VDICOBJ) $(UICOBJ)
SOBJ = $(BIOSSOBJ) $(BDOSSOBJ) $(UTILSOBJ) $(VDISOBJ) $(UISOBJ)
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
	@echo "tgz     bundles almost it all into a tgz archive"
	@echo "depend  creates dependancy section in Makefile"
	@echo "dsm     dsm.txt, an edited desassembly of emutos.img"

emutos.img: $(OBJECTS)
	$(LD) -oformat binary -o $@ $(OBJECTS) $(LDFLAGS) $(LDFLROM)

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

256: etos256k.img

etos256k.img: etosfalc.tmp
	dd if=/dev/zero of=empty.tmp bs=1024 count=256 
	cat empty.tmp >> etosfalc.tmp
	dd if=etosfalc.tmp of=$@ bs=1024 count=256
	rm -f etosfalc.tmp empty.tmp

#
# Aranym or Falcon
#

512: etosfalc.img
falcon: etosfalc.img

etosfalc.tmp: $(OBJECTS)
	$(LD) -oformat binary -o $@ $(OBJECTS) $(LDFLAGS) $(LDFLFAL)

etosfalc.img: etosfalc.tmp
	dd if=/dev/zero of=empty.tmp bs=1024 count=512 
	cat empty.tmp >> etosfalc.tmp                    # Make real tos.img...
	dd if=etosfalc.tmp of=$@ bs=1024 count=512       # with right length.
	rm -f etosfalc.tmp empty.tmp

#
# ram - In two stages. first link EmuTOS to know the top address of bss, 
# then use this value to relocate the RamTOS. 
# the top bss address is taken from the disassembly map.
#

ram: ramtos.img boot.prg

ramtos.img: $(OBJECTS) map 
	@TOPBSS=`grep __end map | sed -e 's/^ *0x\([^ ]*\) .*$$/\1/'`; \
	echo $(LD) -oformat binary -o $@ $(OBJECTS) $(LDFLAGS) \
		-Ttext=0x$$TOPBSS -Tbss=0 ; \
	$(LD) -oformat binary -o $@ $(OBJECTS) $(LDFLAGS) \
		-Ttext=0x$$TOPBSS -Tbss=0 ;


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

testflop.prg: obj/minicrt.o obj/doprintf.o obj/testflop.o
	$(LD) -s -o $@ $^ $(LDFLAGS)

#
# NLS support
#

POFILES = po/fr.po po/de.po

bug$(EXE): tools/bug.c
	$(NATIVECC) -o $@ $<

util/langs.c: $(POFILES) po/LINGUAS bug$(EXE) po/messages.pot
	./bug$(EXE) make
	mv langs.c $@

po/messages.pot: bug$(EXE) po/POTFILES.in
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
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : cli/%.S
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : vdi/%.c
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : vdi/%.S
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : aes/%.c
	${CC} ${CFLAGS} -c -Ibios $< -o $@

obj/%.o : aes/%.S
	${CC} ${CFLAGS} -c $< -o $@

obj/%.o : desk/%.c
	${CC} ${CFLAGS} -c -Ibios -Iaes -Idesk/icons $< -o $@

obj/%.o : desk/%.S
	${CC} ${CFLAGS} -c -Iaes $< -o $@

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

%.dsm : aes/%.c
	${CC} ${CFLAGS} -S -Iutil $< -o $@

%.dsm : desk/%.c
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
	rm -f obj/*.o obj/*.s *~ */*~ *~ core emutos.img map $(DESASS)
	rm -f ramtos.img boot.prg etos*.img mkflop$(EXE) 
	rm -f bootsect.img emutos.st date.prg dumpkbd.prg keytbl2c$(EXE)
	rm -f bug$(EXE) po/messages.pot util/langs.c bios/header.h
	rm -f mkheader$(EXE) tounix$(EXE) $(TMPS) *.dsm

distclean: clean nodepend
	rm -f Makefile.bak '.#'* */'.#'* 

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
	./tounix$(EXE) * bios/* bdos/* doc/* util/* tools/* po/* include/* aes/* desk/*

cvsready: expand crlf nodepend

#
# create a tgz archive named emutos-nnnnnn.tgz,
# where nnnnnn is the date.
#

HERE = $(shell pwd)
HEREDIR = $(shell basename $(HERE))
TGZ = $(shell echo $(HEREDIR)-`date +%y%m%d`|tr A-Z a-z).tgz
TGZEXCL = #--exclude aes --exclude vdi

tgz:	distclean
	cd ..;\
	tar -cf - --exclude '*CVS' $(TGZEXCL) $(HEREDIR) | gzip -c -9 >$(TGZ)

#
# proposal to create an archive named emutos-0_2a.tgz when
# the EMUTOS_VERSION equals "0.2a" in include/version.h
#

VERSION = $(shell grep EMUTOS_VERSION include/version.h | cut -f2 -d\")
RELEASEDIR = emutos-$(VERSION)
RELEASETGZ = $(shell echo $(RELEASEDIR) | tr A-Z. a-z_).tgz

release: distclean
	cd ..; \
	tmp=tmp$$$$; mkdir $$tmp; cd $$tmp; \
	ln -s ../$(HEREDIR) $(RELEASEDIR); \
	tar -h -cf - --exclude '*CVS' $(TGZEXCL) $(RELEASEDIR) | \
	gzip -c -9 >../$(RELEASETGZ) ;\
	cd ..; rm -rf $$tmp


#
# automatic dependencies. (this is ugly)
#

nodepend:
	sed -n '1,/^# DO NOT DELETE/p' < Makefile > Makefile.new
	@if cmp -s Makefile.new Makefile ; then \
		echo rm -f Makefile.new ; \
		rm -f Makefile.new ; \
	else \
		echo cp Makefile Makefile.bak ; \
		cp Makefile Makefile.bak ; \
		echo mv Makefile.new Makefile ; \
		mv Makefile.new Makefile ; \
	fi

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
obj/kprint.o: bios/kprint.c include/portab.h bios/bios.h bios/kprint.h \
 bios/lineavars.h
obj/xbios.o: bios/xbios.c include/portab.h bios/kprint.h bios/iorec.h \
 bios/tosvars.h bios/lineavars.h bios/ikbd.h bios/midi.h bios/mfp.h \
 bios/screen.h bios/sound.h bios/floppy.h bios/disk.h bios/clock.h \
 bios/nvram.h bios/mouse.h bios/config.h include/asm.h
obj/chardev.o: bios/chardev.c include/portab.h bios/bios.h bios/gemerror.h \
 bios/kprint.h bios/tosvars.h bios/chardev.h
obj/blkdev.o: bios/blkdev.c include/portab.h bios/bios.h bios/gemerror.h \
 bios/kprint.h bios/tosvars.h bios/floppy.h bios/disk.h bios/blkdev.h
obj/bios.o: bios/bios.c include/portab.h bios/bios.h bios/gemerror.h \
 bios/config.h bios/kprint.h bios/tosvars.h bios/lineavars.h \
 bios/processor.h bios/initinfo.h bios/machine.h bios/cookie.h \
 bios/country.h include/nls.h bios/biosmem.h bios/ikbd.h bios/mouse.h \
 bios/midi.h bios/mfp.h bios/floppy.h bios/sound.h bios/screen.h \
 bios/clock.h bios/vectors.h include/asm.h bios/chardev.h \
 bios/blkdev.h include/string.h
obj/clock.o: bios/clock.c include/portab.h bios/bios.h bios/kprint.h \
 bios/clock.h bios/ikbd.h bios/tosvars.h include/string.h \
 bios/detect.h bios/nvram.h
obj/fnt8x16.o: bios/fnt8x16.c include/portab.h bios/fontdef.h
obj/fnt8x8.o: bios/fnt8x8.c include/portab.h bios/fontdef.h
obj/fnt6x6.o: bios/fnt6x6.c include/portab.h bios/fontdef.h
obj/mfp.o: bios/mfp.c include/portab.h bios/bios.h bios/kprint.h \
 bios/mfp.h bios/tosvars.h
obj/version.o: bios/version.c
obj/midi.o: bios/midi.c include/portab.h bios/bios.h bios/kprint.h \
 bios/acia.h bios/iorec.h include/asm.h bios/midi.h
obj/ikbd.o: bios/ikbd.c bios/country.h bios/config.h include/portab.h \
 bios/bios.h bios/acia.h bios/kprint.h bios/tosvars.h bios/lineavars.h \
 bios/iorec.h include/asm.h bios/ikbd.h bios/sound.h bios/keyb_us.h \
 bios/keyb_de.h bios/keyb_fr.h
obj/sound.o: bios/sound.c include/portab.h bios/sound.h bios/psg.h \
 bios/tosvars.h include/asm.h
obj/floppy.o: bios/floppy.c include/portab.h bios/gemerror.h bios/floppy.h \
 bios/dma.h bios/fdc.h bios/psg.h bios/mfp.h include/asm.h \
 bios/tosvars.h bios/machine.h bios/blkdev.h bios/bios.h \
 include/string.h bios/kprint.h
obj/disk.o: bios/disk.c include/portab.h bios/gemerror.h bios/disk.h \
 include/asm.h bios/blkdev.h bios/bios.h bios/config.h bios/kprint.h \
 /usr/lib/gcc-lib/m68k-atari-mint/2.95.3/../../../../m68k-atari-mint/include/ctype.h \
 include/string.h bios/atari_rootsec.h
obj/screen.o: bios/screen.c bios/config.h bios/machine.h bios/screen.h \
 include/portab.h bios/tosvars.h include/asm.h
obj/lineainit.o: bios/lineainit.c bios/tosvars.h include/portab.h \
 bios/lineavars.h bios/fontdef.h bios/kprint.h bios/country.h \
 include/string.h bios/screen.h bios/machine.h
obj/mouse.o: bios/mouse.c bios/config.h include/portab.h bios/kprint.h \
 bios/bios.h bios/tosvars.h bios/lineavars.h bios/ikbd.h bios/mouse.h \
 bios/vectors.h
obj/initinfo.o: bios/initinfo.c include/portab.h bios/kprint.h \
 include/nls.h bios/lineavars.h bios/tosvars.h bios/machine.h \
 bios/clock.h include/version.h bios/initinfo.h
obj/cookie.o: bios/cookie.c include/portab.h bios/cookie.h \
 bios/processor.h bios/kprint.h
obj/machine.o: bios/machine.c include/portab.h bios/cookie.h \
 bios/machine.h bios/biosmem.h bios/bios.h bios/detect.h bios/nvram.h \
 bios/tosvars.h bios/country.h bios/clock.h
obj/nvram.o: bios/nvram.c include/portab.h bios/cookie.h bios/machine.h \
 bios/detect.h bios/nvram.h bios/biosmem.h bios/bios.h
obj/country.o: bios/country.c include/portab.h bios/cookie.h \
 bios/country.h bios/detect.h bios/nvram.h bios/tosvars.h
obj/cz6x6.o: bios/cz6x6.c include/portab.h bios/fontdef.h
obj/cz8x8.o: bios/cz8x8.c include/portab.h bios/fontdef.h
obj/cz8x16.o: bios/cz8x16.c include/portab.h bios/fontdef.h
obj/biosmem.o: bios/biosmem.c include/portab.h bios/bios.h bios/biosmem.h \
 bios/kprint.h bios/tosvars.h
obj/fntlat2_6.o: bios/fntlat2_6.c include/portab.h bios/fontdef.h
obj/fntlat2_8.o: bios/fntlat2_8.c include/portab.h bios/fontdef.h
obj/fntlat2_16.o: bios/fntlat2_16.c include/portab.h bios/fontdef.h
obj/bdosinit.o: bdos/bdosinit.c include/portab.h bdos/bios.h
obj/console.o: bdos/console.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/proc.h bdos/pghdr.h bdos/console.h
obj/fsdrive.o: bdos/fsdrive.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/mem.h bdos/gemerror.h bdos/../bios/kprint.h
obj/fshand.o: bdos/fshand.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/gemerror.h
obj/fsopnclo.o: bdos/fsopnclo.c include/portab.h bdos/fs.h \
 include/setjmp.h bdos/bios.h bdos/gemerror.h include/string.h \
 bdos/mem.h
obj/osmem.o: bdos/osmem.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/mem.h bdos/gemerror.h
obj/umem.o: bdos/umem.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/mem.h bdos/gemerror.h bdos/../bios/kprint.h
obj/bdosmain.o: bdos/bdosmain.c include/portab.h bdos/fs.h \
 include/setjmp.h bdos/bios.h bdos/mem.h bdos/proc.h bdos/pghdr.h \
 bdos/console.h bdos/gemerror.h bdos/../bios/kprint.h
obj/fsbuf.o: bdos/fsbuf.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/gemerror.h
obj/fsfat.o: bdos/fsfat.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/gemerror.h
obj/fsio.o: bdos/fsio.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/gemerror.h bdos/../bios/kprint.h
obj/iumem.o: bdos/iumem.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/mem.h bdos/gemerror.h bdos/../bios/kprint.h
obj/proc.o: bdos/proc.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/mem.h bdos/proc.h bdos/pghdr.h bdos/gemerror.h \
 include/string.h bdos/../bios/kprint.h
obj/bdosts.o: bdos/bdosts.c include/portab.h
obj/fsdir.o: bdos/fsdir.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/mem.h bdos/gemerror.h include/string.h \
 bdos/../bios/kprint.h
obj/fsglob.o: bdos/fsglob.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/gemerror.h
obj/fsmain.o: bdos/fsmain.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/mem.h bdos/gemerror.h
obj/kpgmld.o: bdos/kpgmld.c include/portab.h bdos/fs.h include/setjmp.h \
 bdos/bios.h bdos/mem.h bdos/proc.h bdos/pghdr.h bdos/gemerror.h \
 include/string.h bdos/../bios/kprint.h
obj/time.o: bdos/time.c include/portab.h bdos/gemerror.h bdos/bios.h
obj/doprintf.o: util/doprintf.c
obj/nls.o: util/nls.c include/portab.h include/nls.h util/langs.h \
 include/string.h
obj/langs.o: util/langs.c util/langs.h
obj/string.o: util/string.c include/string.h
obj/cbssdefs.o: vdi/cbssdefs.c include/portab.h vdi/gsxdef.h vdi/fontdef.h \
 vdi/attrdef.h
obj/isin.o: vdi/isin.c include/portab.h
obj/jmptbl.o: vdi/jmptbl.c vdi/gsxdef.h include/portab.h vdi/gsxextrn.h \
 vdi/fontdef.h vdi/attrdef.h include/asm.h vdi/jmptbl.h
obj/lisastub.o: vdi/lisastub.c include/portab.h
obj/lisatabl.o: vdi/lisatabl.c include/portab.h vdi/gsxdef.h vdi/styles.h
obj/monobj.o: vdi/monobj.c include/portab.h vdi/gsxdef.h vdi/gsxextrn.h \
 vdi/fontdef.h vdi/attrdef.h include/asm.h
obj/obj/opnwkram.o: vdi/opnwkram.c include/portab.h vdi/gsxdef.h \
 vdi/gsxextrn.h vdi/fontdef.h vdi/attrdef.h include/asm.h
obj/seedfill.o: vdi/seedfill.c include/portab.h vdi/gsxdef.h \
 vdi/gsxextrn.h vdi/fontdef.h vdi/attrdef.h include/asm.h
obj/text.o: vdi/text.c include/portab.h vdi/gsxdef.h vdi/gsxextrn.h \
 vdi/fontdef.h vdi/attrdef.h include/asm.h vdi/jmptbl.h
obj/cbssdefs.o: vdi/cbssdefs.c include/portab.h vdi/gsxdef.h vdi/fontdef.h \
 vdi/attrdef.h
obj/gemaplib.o: aes/gemaplib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/gem_rsc.h aes/geminit.h aes/gempd.h aes/geminput.h aes/gemflag.h \
 aes/gemevlib.h aes/gemgsxif.h aes/gsxdefs.h aes/gemwmlib.h \
 aes/gemmnlib.h aes/gemdosif.h aes/gemasm.h aes/gemdisp.h \
 aes/gemsclib.h aes/gemrslib.h
obj/gemasync.o: aes/gemasync.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/geminput.h aes/gemflag.h aes/gemqueue.h aes/optimopt.h \
 aes/gemasm.h
obj/gembase.o: aes/gembase.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h
obj/gemctrl.o: aes/gemctrl.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/geminput.h aes/gemevlib.h aes/gemwmlib.h \
 aes/gemglobe.h aes/gemgraf.h aes/gsxdefs.h aes/gemmnlib.h \
 aes/geminit.h aes/gemgsxif.h aes/gemgrlib.h aes/gemoblib.h \
 aes/gemasm.h aes/optimopt.h aes/rectfunc.h
obj/obj/gemevlib.o: aes/gemevlib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/gemasync.h aes/gemdisp.h aes/geminput.h aes/gemaplib.h \
 aes/geminit.h
obj/gemflag.o: aes/gemflag.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/gemasync.h aes/geminput.h aes/gemdosif.h aes/gemasm.h
obj/gemfmalt.o: aes/gemfmalt.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/gem_rsc.h aes/gemgsxif.h aes/gsxdefs.h \
 aes/gemoblib.h aes/gemobed.h aes/geminit.h aes/gemrslib.h \
 aes/gemgraf.h aes/gemfmlib.h aes/gemwmlib.h aes/optimopt.h \
 aes/rectfunc.h
obj/gemfmlib.o: aes/gemfmlib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/gem_rsc.h aes/gemwmlib.h aes/gemrslib.h \
 aes/geminput.h aes/gemctrl.h aes/gemoblib.h aes/gemobjop.h \
 aes/gemgrlib.h aes/gemevlib.h aes/gemgraf.h aes/gsxdefs.h \
 aes/gemobed.h aes/optimize.h aes/gemfmalt.h aes/gemglobe.h \
 aes/gemmnlib.h
obj/gemfslib.o: aes/gemfslib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/obdefs.h aes/taddr.h aes/dos.h \
 aes/gemlib.h aes/gem_rsc.h aes/gemdos.h aes/gemoblib.h aes/gemgraf.h \
 aes/gsxdefs.h aes/gemfmlib.h aes/gemgsxif.h aes/gemgrlib.h \
 aes/gemglobe.h aes/geminit.h aes/gemsuper.h aes/gemshlib.h aes/gsx2.h \
 aes/optimize.h aes/optimopt.h aes/rectfunc.h
obj/gemglobe.o: aes/gemglobe.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/obdefs.h aes/gemlib.h
obj/gemgraf.o: aes/gemgraf.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/obdefs.h aes/gsxdefs.h aes/funcdef.h aes/gemgraf.h \
 aes/gemgsxif.h aes/optimize.h aes/optimopt.h aes/gsx2.h \
 aes/rectfunc.h
obj/gemgrlib.o: aes/gemgrlib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/gemevlib.h aes/gemgraf.h aes/gsxdefs.h aes/gemwmlib.h \
 aes/gemoblib.h aes/geminput.h aes/gemgsxif.h aes/gemasm.h aes/gsx2.h \
 aes/optimopt.h aes/rectfunc.h aes/optimize.h
obj/gemgsxif.o: aes/gemgsxif.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/dos.h aes/obdefs.h aes/gsxdefs.h aes/funcdef.h \
 aes/gemdos.h aes/gemrslib.h aes/gemgraf.h aes/geminput.h aes/struct.h \
 aes/gemlib.h aes/gemdosif.h aes/gsx2.h aes/geminit.h aes/gemctrl.h \
 aes/gemgsxif.h
obj/geminit.o: aes/geminit.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/obdefs.h aes/taddr.h aes/struct.h aes/basepage.h \
 aes/gemlib.h aes/crysbind.h aes/gem_rsc.h aes/dos.h aes/gemgsxif.h \
 aes/gsxdefs.h aes/gemdosif.h aes/gemctrl.h aes/gemshlib.h aes/gempd.h \
 aes/gemdisp.h aes/gemrslib.h aes/gemobed.h aes/gemdos.h aes/gemgraf.h \
 aes/gemevlib.h aes/gemwmlib.h aes/gemglobe.h aes/gemfslib.h \
 aes/gemoblib.h aes/gemsclib.h aes/gemfmlib.h aes/gemasm.h \
 aes/gemaplib.h aes/gemsuper.h aes/geminput.h aes/gemmnlib.h \
 aes/optimize.h aes/optimopt.h
obj/geminput.o: aes/geminput.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/geminput.h aes/gemgraf.h aes/gsxdefs.h aes/gemdosif.h \
 aes/gemctrl.h aes/gemmnlib.h aes/gemaplib.h aes/gemglobe.h \
 aes/gemevlib.h aes/gemwmlib.h aes/gemasync.h aes/gemdisp.h \
 aes/rectfunc.h
obj/gemmnlib.o: aes/gemmnlib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/gemgsxif.h aes/gsxdefs.h aes/gemevlib.h \
 aes/gemoblib.h aes/gemobed.h aes/gemwmlib.h aes/gemgraf.h \
 aes/geminput.h aes/gemsuper.h aes/gemctrl.h aes/gempd.h \
 aes/rectfunc.h
obj/gemobed.o: aes/gemobed.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/gem_rsc.h aes/gemoblib.h aes/gemgraf.h aes/gsxdefs.h \
 aes/gemglobe.h aes/geminit.h aes/gemrslib.h aes/optimize.h \
 aes/optimopt.h aes/rectfunc.h
obj/gemobjop.o: aes/gemobjop.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/obdefs.h aes/taddr.h
obj/gemoblib.o: aes/gemoblib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/funcdef.h aes/gemglobe.h aes/gemgsxif.h \
 aes/gsxdefs.h aes/gemobjop.h aes/gemgraf.h aes/optimopt.h \
 aes/rectfunc.h
obj/gempd.o: aes/gempd.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/gemdosif.h aes/gemglobe.h aes/geminit.h aes/gemasm.h \
 aes/optimize.h aes/optimopt.h
obj/gemqueue.o: aes/gemqueue.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/optimize.h aes/gemasync.h
obj/gemrslib.o: aes/gemrslib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/gem_rsc.h aes/gemdos.h aes/gemshlib.h aes/gemglobe.h \
 aes/gemgraf.h aes/gsxdefs.h aes/geminit.h
obj/gemsclib.o: aes/gemsclib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/crysbind.h aes/dos.h aes/gem_rsc.h aes/gemrslib.h aes/gemdos.h \
 aes/geminit.h aes/gemshlib.h
obj/gemshlib.o: aes/gemshlib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/obdefs.h aes/taddr.h aes/struct.h aes/basepage.h \
 aes/dos.h aes/gemlib.h aes/gem_rsc.h aes/gemdosif.h aes/gemdos.h \
 aes/gemgraf.h aes/gsxdefs.h aes/gemgsxif.h aes/gemoblib.h \
 aes/gemwmlib.h aes/gemfmlib.h aes/gempd.h aes/gemflag.h \
 aes/gemglobe.h aes/geminit.h aes/gemrslib.h aes/optimopt.h
obj/gemsuper.o: aes/gemsuper.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/crysbind.h aes/gem_rsc.h aes/gempd.h aes/gemaplib.h \
 aes/geminit.h aes/gemevlib.h aes/gemmnlib.h aes/gemoblib.h \
 aes/gemobed.h aes/gemfmlib.h aes/gemfslib.h aes/gemgrlib.h \
 aes/gemgraf.h aes/gsxdefs.h aes/gemgsxif.h aes/gemsclib.h \
 aes/gemwmlib.h aes/gemrslib.h aes/gemshlib.h aes/gemglobe.h \
 aes/gemfmalt.h aes/gemdosif.h aes/gemasm.h
obj/gemwmlib.o: aes/gemwmlib.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/taddr.h \
 aes/gemlib.h aes/gempd.h aes/gemaplib.h aes/gemflag.h aes/gemoblib.h \
 aes/gemwrect.h aes/gemmnlib.h aes/geminit.h aes/gemgraf.h \
 aes/gsxdefs.h aes/gemglobe.h aes/gemfmlib.h aes/gemevlib.h \
 aes/gemwmlib.h aes/gemgsxif.h aes/gemobjop.h aes/gemctrl.h aes/gsx2.h \
 aes/rectfunc.h aes/optimopt.h aes/optimize.h
obj/gemwrect.o: aes/gemwrect.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/struct.h aes/basepage.h aes/obdefs.h aes/gemlib.h \
 aes/gemobjop.h aes/gemwmlib.h aes/gemglobe.h aes/optimize.h \
 aes/rectfunc.h
obj/optimize.o: aes/optimize.c aes/portab.h aes/../include/portab.h \
 aes/machine.h aes/taddr.h aes/obdefs.h aes/geminit.h aes/gemrslib.h \
 aes/rectfunc.h aes/optimopt.h
obj/rectfunc.o: aes/rectfunc.c aes/portab.h aes/../include/portab.h \
 aes/obdefs.h
obj/gemdos.o: aes/gemdos.c aes/portab.h aes/../include/portab.h \
 aes/machine.h
obj/gem_rsc.o: aes/gem_rsc.c aes/portab.h aes/../include/portab.h \
 aes/obdefs.h aes/gemrslib.h
obj/command.o: cli/command.c include/nls.h include/string.h \
 include/setjmp.h include/portab.h
obj/obj/obj/obj/obj/obj/deskglob.o: desk/deskglob.c desk/portab.h desk/machine.h desk/obdefs.h \
 desk/deskapp.h desk/deskfpd.h desk/deskwin.h desk/infodef.h \
 desk/desktop.h desk/gembind.h desk/deskbind.h
obj/obj/obj/obj/obj/obj/deskrsrc.o: desk/deskrsrc.c desk/portab.h desk/machine.h desk/obdefs.h \
 desk/aesbind.h
obj/obj/obj/gembind.o: desk/gembind.c desk/portab.h desk/machine.h desk/taddr.h \
 desk/obdefs.h desk/gembind.h
obj/icons.o: desk/icons.c desk/portab.h desk/machine.h desk/obdefs.h \
 desk/deskapp.h
obj/tosvars.o: bios/tosvars.S
obj/startup.o: bios/startup.S bios/asmdefs.h bios/config.h bios/header.h
obj/lineavars.o: bios/lineavars.S
obj/vectors.o: bios/vectors.S
obj/aciavecs.o: bios/aciavecs.S
obj/processor.o: bios/processor.S bios/asmdefs.h bios/config.h
obj/memory.o: bios/memory.S
obj/linea.o: bios/linea.S
obj/conout.o: bios/conout.S
obj/mousedrv.o: bios/mousedrv.S
obj/detect.o: bios/detect.S
obj/panicasm.o: bios/panicasm.S
obj/kprintasm.o: bios/kprintasm.S bios/asmdefs.h bios/config.h
obj/rwa.o: bdos/rwa.S
obj/memset.o: util/memset.S
obj/memmove.o: util/memmove.S
obj/nlsasm.o: util/nlsasm.S
obj/setjmp.o: util/setjmp.S
obj/entry.o: vdi/entry.S vdi/vdiconf.h
obj/bitblt.o: vdi/bitblt.S vdi/vdiconf.h
obj/bltfrag.o: vdi/bltfrag.S vdi/vdiconf.h
obj/copyrfm.o: vdi/copyrfm.S vdi/vdiconf.h
obj/esclisa.o: vdi/esclisa.S vdi/vdiconf.h
obj/gsxasm1.o: vdi/gsxasm1.S vdi/vdiconf.h
obj/gsxasm2.o: vdi/gsxasm2.S vdi/vdiconf.h
obj/lisagem.o: vdi/lisagem.S vdi/vdiconf.h
obj/vdimouse.o: vdi/vdimouse.S vdi/vdiconf.h
obj/newmono.o: vdi/newmono.S vdi/vdiconf.h
obj/textblt.o: vdi/textblt.S vdi/vdiconf.h
obj/tranfm.o: vdi/tranfm.S vdi/vdiconf.h
obj/gsxvars.o: vdi/gsxvars.S
obj/gemstart.o: aes/gemstart.S
obj/gemdosif.o: aes/gemdosif.S
obj/gemasm.o: aes/gemasm.S
obj/gsx2.o: aes/gsx2.S
obj/large.o: aes/large.S
obj/optimopt.o: aes/optimopt.S
obj/coma.o: cli/coma.S
obj/deskstart.o: desk/deskstart.S
