#
# Makefile suitable for Linux and Cygwin setups
# only GCC (cross-mint) is supported. 
# Some features like pattern substitution probably
# require GNU-make.
#
# for a list of main targets do  
#   make help
#
# C code (C) and assembler (S) source go in directories 
# bios/, bdos/, util/, ... ; To add source code files, update
# the variables XXXXCSRC and XXXXSSRC below; each directories has
# a different set of build flags indicated in variables 
# xxxx_copts and xxxx_sopts below. 
# Here xxxx is the directory name, and XXXX is the same in uppercase.
#


#
# the country. should be a lowercase two-letter code as found in
# the table in tools/mkheader.c and bios/country.c
#

COUNTRY = us

#
# Unique-country support: if UNIQUE is defined, then
# EmuTOS will be built with only one country.
#
# example: make UNIQUE=fr 256
#

DEF =
ifneq (,$(UNIQUE))
COUNTRY = $(UNIQUE)
endif

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
CORE = *.stackdump
else
# ordinary Unix stuff
EXE = 
CORE = core
endif

#
# test for localconf.h
#

ifeq (localconf.h,$(shell echo localconf.h*))
LOCALCONF = -DLOCALCONF
else
LOCALCONF = 
endif

# 
# compilation flags
#

# indent flags
INDENT = indent -kr

# Linker with relocation information and binary output (image)
LD = m68k-atari-mint-gcc -nostartfiles -nostdlib
LDFLAGS = -Xlinker -oformat -Xlinker binary -lgcc 
LDFLAGS_T1 = -Xlinker -Ttext=0xfc0000 -Xlinker -Tbss=0x000000 
LDFLAGS_T2 = -Xlinker -Ttext=0xe00000 -Xlinker -Tbss=0x000000 

# (relocation for RAM TOS is derived dynamically from the BSS size)

# Assembler with options for Motorola like syntax (68000 cpu)
# AS = m68k-atari-mint-gcc -x assembler
# ASINC = -Iinclude
# ASFLAGS = --register-prefix-optional -m68000 $(ASINC) 

# C compiler for MiNT
CC = m68k-atari-mint-gcc
INC = -Iinclude
CFLAGS = -Os -fomit-frame-pointer -Wall -mshort -m68000 $(DEF) $(LOCALCONF) $(INC)

CPPFLAGS = $(INC)

# The objdump utility (disassembler)
OBJDUMP = m68k-atari-mint-objdump

# the native C compiler, for tools
NATIVECC = gcc -Wall -W -pedantic -ansi -O

# 
# source code in bios/
# Note: tosvars.o must be the first object linked.

BIOSCSRC = kprint.c xbios.c chardev.c blkdev.c bios.c clock.c \
           mfp.c parport.c biosmem.c acsi.c \
           midi.c ikbd.c sound.c floppy.c disk.c screen.c lineainit.c \
           mouse.c initinfo.c cookie.c machine.c nvram.c country.c 
BIOSSSRC = tosvars.S startup.S lineavars.S vectors.S aciavecs.S \
           processor.S memory.S linea.S conout.S panicasm.S kprintasm.S

#
# source code in bdos/
#

BDOSCSRC = console.c fsdrive.c fshand.c fsopnclo.c osmem.c \
           umem.c bdosmain.c fsbuf.c fsfat.c fsio.c iumem.c proc.c \
           fsdir.c fsglob.c fsmain.c kpgmld.c time.c 
BDOSSSRC = rwa.S

#
# source code in util/
#

UTILCSRC = doprintf.c nls.c langs.c string.c
UTILSSRC = memset.S memmove.S nlsasm.S setjmp.S miscasm.S stringasm.S

#
# source code in vdi/
#

VDICSRC = vdimain.c vdiinput.c monobj.c monout.c text.c seedfill.c bezier.c \
          vdiesc.c
VDISSRC = entry.S bitblt.S bltfrag.S copyrfm.S gsxasm1.S gsxasm2.S \
          vdimouse.S textblt.S #tranfm.S esclisa.S

#
# source code in aes/
#

AESCSRC = gemaplib.c gemasync.c gemctrl.c gemdisp.c gemevlib.c \
          gemflag.c gemfmalt.c gemfmlib.c gemfslib.c gemgraf.c \
          gemgrlib.c gemgsxif.c geminit.c geminput.c gemmnlib.c gemobed.c \
          gemobjop.c gemoblib.c gempd.c gemqueue.c gemrslib.c gemsclib.c \
          gemshlib.c gemsuper.c gemwmlib.c gemwrect.c optimize.c rectfunc.c \
          gemdos.c gem_rsc.c
AESSSRC = gemstart.S gemdosif.S gemasm.S gsx2.S large.S optimopt.S

#
# source code in desk/
#

DESKCSRC = deskact.c deskapp.c deskdir.c deskfpd.c deskfun.c deskglob.c \
           deskinf.c deskins.c deskmain.c deskobj.c deskpro.c deskrsrc.c \
           desksupp.c deskwin.c gembind.c icons.c desk_rsc.c
           #taddr.c deskgraf.c deskgsx.c
DESKSSRC = deskstart.S

#
# source code in cli/ for EmuTOS console EmuCON
#

CONSCSRC = command.c
CONSSSRC = coma.S

#
# specific CC -c options for specific directories
#

vpath % bios:bdos:util:cli:vdi:aes:desk

bios_copts =
bdos_copts =
util_copts = -Ibios
cli_copts  = -Ibios
vdi_copts  = -Ibios
aes_copts  = -Ibios
desk_copts = -Ibios -Iaes -Idesk/icons

#
# country-specific settings
#

include country.mk

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
PVDICSRC  = $(VDICSRC:%=vdi/%)
PVDISSRC  = $(VDISSRC:%=vdi/%)
PAESCSRC  = $(AESCSRC:%=aes/%)
PAESSSRC  = $(AESSSRC:%=aes/%)
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
DESKCOBJ = $(DESKCSRC:%.c=obj/%.o)
DESKSOBJ = $(DESKSSRC:%.S=obj/%.o)

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
OBJECTS = $(SOBJ) $(COBJ) $(FONTOBJ)

#
# temporary variables, for internal Makefile use
# anything ending in .tmp will be removed by make clean.

TMP1 = make1.tmp
TMP2 = make2.tmp
TMP3 = make3.tmp

#
# production targets 
# 

all:	emutos2.img

help:	
	@echo "target  meaning"
	@echo "------  -------"
	@echo "help    this help message"
	@echo "all     emutos2.img, a TOS 2 ROM image (0xE00000)"
	@echo "192     etos192k.img, EmuTOS ROM padded to size 192 KB (starting at 0xFC0000)"
	@echo "256     etos256k.img, EmuTOS ROM padded to size 256 KB (starting at 0xE00000)"
	@echo "512     etos512k.img, EmuTOS ROM padded to size 512 KB (starting at 0xE00000)" 
	@echo "ram     ramtos.img + boot.prg, a RAM tos"
	@echo "flop    emutos.st, a bootable floppy with RAM tos"
	@echo "clean"
	@echo "cvsready  put files in canonic format before committing to cvs"
	@echo "tgz     bundles almost it all into a tgz archive"
	@echo "depend  creates dependancy file (makefile.dep)"
	@echo "dsm     dsm.txt, an edited desassembly of emutos.img"
	@echo "fdsm    fal_dsm.txt, like above, but for 0xE00000 ROMs"
	@echo "*.dsm   desassembly of any .c or almost any .img file"

#
# the maps must be built at the same time as the images, to enable
# one generic target to deal with all edited desassembly.
#

emutos1.img emutos1.map: $(OBJECTS)
	$(LD) -o emutos1.img -Xlinker -Map -Xlinker emutos1.map \
	  $(OBJECTS) $(LDFLAGS) $(LDFLAGS_T1)

emutos2.img emutos2.map: $(OBJECTS)
	$(LD) -o emutos2.img -Xlinker -Map -Xlinker emutos2.map \
	  $(OBJECTS) $(LDFLAGS) $(LDFLAGS_T2)


#
# generic sized images handling
#

define sized_image
@goal=`echo $@ | sed -e 's/[^0-9]//g'`; \
size=`wc -c < $<`; \
if [ $$size -gt `expr $$goal \* 1024` ]; \
then \
  rm -f $<; \
  goalbytes=`expr $${goal} \* 1024`; \
  echo "EmuTOS too big for $${goal}K ($$goalbytes bytes): size = $$size"; \
  false ; \
else \
  echo dd if=/dev/zero of=$@ bs=1024 count=$$goal; \
  dd if=/dev/zero of=$@ bs=1024 count=$$goal; \
  echo dd if=$< of=$@ bs=1024 conv=notrunc; \
  dd if=$< of=$@ bs=1024 conv=notrunc; \
  rm -f $<; \
fi
endef

#
# 192kB Image
#

192: etos192k.img

etos192k.img: emutos1.img
	$(sized_image)

#
# 256kB Image
#

256: etos256k.img

etos256k.img: emutos2.img
	$(sized_image)

#
# 512kB Image (for Aranym or Falcon)
#

512: etos512k.img
falcon: etos512k.img

etos512k.img: emutos2.img
	$(sized_image)


#
# ram - In two stages. first link emutos2.img to know the top address of bss, 
# then use this value (taken from the map) to relocate the RamTOS. 
#

ram: ramtos.img boot.prg

ramtos.img ramtos.map: $(OBJECTS) emutos2.map 
	@topbss=`sed -e '/__end/!d;s/^ *//;s/ .*//' emutos2.map`; \
	echo $(LD) -o ramtos.img $(OBJECTS) $(LDFLAGS) \
		-Xlinker -Ttext=$$topbss -Xlinker -Tbss=0; \
	$(LD) -o ramtos.img $(OBJECTS) $(LDFLAGS) \
		-Xlinker -Map -Xlinker ramtos.map \
		-Xlinker -Ttext=$$topbss -Xlinker -Tbss=0

boot.prg: obj/minicrt.o obj/boot.o obj/bootasm.o
	$(LD) -Xlinker -s -o $@ obj/minicrt.o obj/boot.o obj/bootasm.o -lgcc

#
# compressed ROM image
#

COMPROBJ = obj/comprimg.o obj/memory.o obj/uncompr.o obj/processor.o

compr2.img compr2.map: $(COMPROBJ)
	$(LD) -o compr2.img $(COMPROBJ) $(LDFLAGS) \
	  -Xlinker -Map -Xlinker compr2.map $(LDFLAGS_T2)

etoscpr2.img: compr2.img compr$(EXE) ramtos.img
	./compr$(EXE) --rom compr2.img ramtos.img $@

compr1.img compr1.map: $(COMPROBJ)
	$(LD) -o compr1.img $(COMPROBJ) $(LDFLAGS) \
	  -Xlinker -Map -Xlinker compr2.map $(LDFLAGS_T1)

etoscpr1.img: compr1.img compr$(EXE) ramtos.img
	./compr$(EXE) --rom compr1.img ramtos.img $@

ecpr256k.img: etoscpr2.img
	$(sized_image)

ecpr192k.img: etoscpr1.img
	$(sized_image)

compr$(EXE): tools/compr.c
	$(NATIVECC) -o $@ $<

uncompr$(EXE): tools/uncompr.c
	$(NATIVECC) -o $@ $<

comprtest: compr$(EXE) uncompr$(EXE)
	sh tools/comprtst.sh

#
# flop
#

flop : emutos.st

fd0:	emutos.st
	dd if=$< of=/dev/fd0D360

emutos.st: mkflop$(EXE) bootsect.img ramtos.img
	./mkflop$(EXE)

bootsect.img : obj/bootsect.o
	$(LD) -Xlinker -oformat -Xlinker binary -o $@ obj/bootsect.o

mkflop$(EXE) : tools/mkflop.c
	$(NATIVECC) -o $@ $<

#
# Misc utilities
#

date.prg: obj/minicrt.o obj/doprintf.o obj/date.o
	$(LD) -Xlinker -s -o $@ obj/minicrt.o obj/doprintf.o obj/date.o -lgcc

dumpkbd.prg: obj/minicrt.o obj/memmove.o obj/dumpkbd.o
	$(LD) -Xlinker -s -o $@ $^ -lgcc

keytbl2c$(EXE) : tools/keytbl2c.c
	$(NATIVECC) -o $@ $<

#testflop.prg: obj/minicrt.o obj/doprintf.o obj/testflop.o
#	$(LD) -Xlinker -s -o $@ $^ -lgcc

#
# NLS support
#

POFILES = po/fr.po po/de.po po/cs.po

bug$(EXE): tools/bug.c
	$(NATIVECC) -o $@ $<

util/langs.c: $(POFILES) po/LINGUAS bug$(EXE) po/messages.pot
	./bug$(EXE) make
	mv langs.c $@

obj/langs.o: include/config.h include/i18nconf.h

po/messages.pot: bug$(EXE) po/POTFILES.in
	./bug$(EXE) xgettext

#
# all binaries
#

allbin: 
	@echo building etos512k.img; \
	make etos512k.img; \
	for i in $(COUNTRIES); \
	do \
	  j=etos256$${i}.img; \
	  echo building $$j; \
	  make UNIQUE=$$i 256; \
	  mv etos256k.img $$j; \
	done

all192:
	@for i in $(COUNTRIES); \
	do \
	  j=etos192$${i}.img; \
	  echo building $$j; \
	  make DEF='-DTOS_VERSION=0x102' UNIQUE=$$i 192; \
	  mv etos192k.img $$j; \
	done


#
# Mono-country translated EmuTOS: translate files only if the language
# is not 'us', and if a UNIQUE EmuTOS is requested. 
#
# If the '.tr.c' files are present the '.o' files are compiled from these
# source files because the '%.o: %.tr.c' rule comes before the normal
# '%.o: %.c' rule. 
# Changing the settings of $(COUNTRY) or $(UNIQUE) will remove both 
# the '.o' files (to force rebuilding them) and the '.tr.c' files 
# (otherwise 'make UNIQUE=fr; make UNIQUE=us' falsely keeps the
# .tr.c french translations). See target obj/country below.
#

TRANS_SRC_CMD = sed -e '/^[^a-z]/d;s/\.c/.tr&/' <po/POTFILES.in

ifneq (,$(UNIQUE))

ifneq (us,$(ETOSLANG))
emutos1.img emutos2.img: $(shell $(TRANS_SRC_CMD))

%.tr.c : %.c po/$(ETOSLANG).po bug$(EXE) po/LINGUAS obj/country
	./bug$(EXE) translate $(ETOSLANG) $<
endif
endif

#
# obj/country contains the current values of $(COUNTRY) and $(UNIQUE). 
# whenever it changes, whatever necessary steps are taken so that the
# correct files get re-compiled, even without doing make depend.
#

needcountry.tmp:
	@touch $@

obj/country: needcountry.tmp
	@rm -f needcountry.tmp
	@echo $(COUNTRY) $(UNIQUE) > last.tmp; \
	if [ -e $@ ]; \
	then \
	  if cmp -s last.tmp $@; \
	  then \
	    rm -f last.tmp; \
	    exit 0; \
	  fi; \
	fi; \
	mv last.tmp $@; \
	for i in `$(TRANS_SRC_CMD)`; \
	do \
	  j=obj/`basename $$i tr.c`o; \
	  echo rm -f $$i $$j; \
	  rm -f $$i $$j; \
	done; \
	echo rm -f include/i18nconf.h; \
	rm -f include/i18nconf.h; 

#
# i18nconf.h - this file is automatically created by the Makefile. This
# is done this way instead of simply passing the flags as -D on the 
# command line because:
# - the command line is shorter
# - it allows #defining CONF_KEYB as KEYB_US with KEYB_US #defined elsewhere
# - explicit dependencies can force rebuilding files that include it
#

ifneq (,$(UNIQUE))
include/i18nconf.h:
	@rm -f $@; touch $@
	@echo \#define CONF_UNIQUE_COUNTRY 1 >> $@
	@echo \#define CONF_NO_NLS 1 >> $@
	@echo \#define CONF_LANG '"$(ETOSLANG)"' >> $@
	@echo \#ifdef KEYB_$(ETOSKEYB) >> $@
	@echo \#define CONF_KEYB KEYB_$(ETOSKEYB) >> $@
	@echo \#endif >> $@
	@echo \#ifdef CHARSET_$(ETOSCSET) >> $@
	@echo \#define CONF_CHARSET CHARSET_$(ETOSCSET) >> $@
	@echo \#endif >> $@
	@if [ "x$(ETOSKEYB)" = "x" -o "x$(ETOSCSET)" = "x" ]; \
	then \
	  echo "Country $(COUNTRY) not properly specified in country.mk"; \
	  false; \
	fi
else
include/i18nconf.h:
	@rm -f $@; touch $@
	@echo \#define CONF_KEYB KEYB_ALL > $@
	@echo \#define CONF_CHARSET CHARSET_ALL >> $@
endif

obj/country.o: include/i18nconf.h
obj/langs.o: include/i18nconf.h
obj/nls.o: include/i18nconf.h
obj/nlsasm.o: include/i18nconf.h

#
# OS header
#

obj/startup.o: bios/header.h

obj/comprimg.o: bios/header.h

obj/country.o: bios/header.h 

bios/header.h: mkheader$(EXE) obj/country include/i18nconf.h
	./mkheader$(EXE) $(COUNTRY)

mkheader$(EXE): tools/mkheader.c
	$(NATIVECC) -o $@ $<

#
# build rules - the little black magic here allows for e.g.
# $(bios_copts) to specify additional options for C source files
# in bios/, and $(vdi_sopts) to specify additional options for
# ASM source files in vdi/
#

obj/%.o : %.tr.c
	$(CC) $(CFLAGS) -c $($(subst /,_,$(dir $<))copts) $< -o $@

obj/%.o : %.c
	$(CC) $(CFLAGS) -c $($(subst /,_,$(dir $<))copts) $< -o $@

obj/%.o : %.S
	$(CC) $(CFLAGS) -c $($(subst /,_,$(dir $<))sopts) $< -o $@

%.dsm : %.c
	$(CC) $(CFLAGS) -S $($(subst /,_,$(dir $<))copts) $< -o $@

#
# generic dsm handling
#

%.dsm: %.map
	vma=`grep '^.text' $< | sed -e 's/[^0]*//;s/ .*//'`; \
	$(OBJDUMP) --target=binary --architecture=m68k \
	  --adjust-vma=$$vma -D $(<:%.map=%.img) \
	| sed -e '/:/!d;/^  /!d;s/^  //;s/^ /0/;s/:	/: /' > $(TMP1)
	grep '^ \+0x' $< | sort \
	| sed -e 's/^ *0x00//;s/  */:  /' > $(TMP2)
	cat $(TMP1) $(TMP2) | LC_ALL=C sort > $@
	rm $(TMP1) $(TMP2)

dsm.txt: emutos1.dsm
	cp $< $@

dsm: dsm.txt

show: dsm.txt
	cat dsm.txt

fal_dsm.txt: emutos2.dsm
	cp $< $@

fdsm: fal_dsm.txt

fshow: fal_dsm.txt
	cat fal_dsm.txt

#
# clean and distclean 
# (distclean is called before creating a tgz archive)
#

clean:
	rm -f obj/*.o obj/*.s *~ */*~ $(CORE) *.img *.map *.dsm *.tmp
	rm -f dsm.txt fal_dsm.txt
	rm -f boot.prg date.prg dumpkbd.prg emutos.st
	rm -f mkflop$(EXE) keytbl2c$(EXE) tounix$(EXE) mkheader$(EXE)
	rm -f bug$(EXE) po/messages.pot util/langs.c bios/header.h
	rm -f */*.tr.c obj/country include/i18nconf.h
	rm -f compr$(EXE) uncompr$(EXE)
	rm -f makefile.dep 


distclean: clean
	rm -f '.#'* */'.#'* 

#
# indent - indents the files except when there are warnings
# checkindent - check for indent warnings, but do not alter files.
#

INDENTFILES = bdos/*.c bios/*.c util/*.c tools/*.c desk/*.c aes/*.c vdi/*.c

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

cvsready: expand crlf

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
# file dependencies (makefile.dep)
#

depend: util/langs.c bios/header.h
	( \
	  $(CC) -MM $(INC) -Ibios -Iaes -Idesk/icons $(DEF) $(CSRC); \
	  $(CC) -MM $(INC) $(DEF) $(SSRC) \
	) | sed -e '/:/!s,^,obj/,' >makefile.dep;

ifeq (makefile.dep,$(shell echo makefile.dep*))
include makefile.dep
endif

