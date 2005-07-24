#
# Makefile - the EmuTOS overbloated Makefile
#
# Copyright (c) 2001, 02, 03 EmuTOS development team.
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

# The Makefile is suitable for Linux and Cygwin setups.
# only GCC (cross-mint) is supported. GNU-make *is* required.
#
# for a list of main targets do
#   make help
#
# C code (C) and assembler (S) source go in directories bios/, bdos/, ...
# To modify the list of source code files, update the variables xxx_csrc 
# and xxx_ssrc below; each directories has a different set of build flags 
# indicated in variables xxx_copts and xxx_sopts below. 
# (xxx being the directory name)


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
# Also include EmuCON when already using AES as main interface?
#

WITH_CLI = 1


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

ifneq (,$(wildcard localconf.h))
LOCALCONF = -DLOCALCONF
else
LOCALCONF = 
endif

#
# TOCLEAN will accumulate over thie Makefile the names of files to remove 
# when doing make clean; temporary Makefile files are *.tmp
#

TOCLEAN := *~ */*~ $(CORE) *.tmp

# 
# compilation flags
#

# indent flags
INDENT = indent -kr

# Linker with relocation information and binary output (image)
LD = $(CC) -nostartfiles -nostdlib
LDFLAGS = -Xlinker --oformat -Xlinker binary -lgcc 
LDFLAGS_T1 = -Xlinker -Ttext=0xfc0000 -Xlinker -Tbss=0x000000 
LDFLAGS_T2 = -Xlinker -Ttext=0xe00000 -Xlinker -Tbss=0x000000 

# C compiler for MiNT
CC = m68k-atari-mint-gcc
INC = -Iinclude
OPTFLAGS = -Os -fomit-frame-pointer
WARNFLAGS = -Wall #-fno-common -Wshadow -Wmissing-prototypes -Wstrict-prototypes #-Werror
CFLAGS =  $(OPTFLAGS) $(WARNFLAGS) -ffreestanding -mshort -m68000 $(DEF) \
  $(LOCALCONF) $(INC) -DWITH_AES=$(WITH_AES) -DWITH_CLI=$(WITH_CLI)

CPPFLAGS = $(INC)

# The objdump utility (disassembler)
OBJDUMP = m68k-atari-mint-objdump

# the native C compiler, for tools
NATIVECC = gcc -Wall -W -pedantic -ansi -O

# 
# source code in bios/
# Note: tosvars.o must be the first object linked.

bios_csrc = kprint.c xbios.c chardev.c blkdev.c bios.c clock.c \
            mfp.c parport.c biosmem.c acsi.c \
            midi.c ikbd.c sound.c floppy.c disk.c screen.c lineainit.c \
            mouse.c initinfo.c cookie.c machine.c nvram.c country.c \
            xhdi.c natfeats.c font.c conout.c vt52.c
bios_ssrc = tosvars.S startup.S aciavecs.S vectors.S lineavars.S \
            processor.S memory.S linea.S panicasm.S kprintasm.S \
            natfeat.S

#
# source code in bdos/
#

bdos_csrc = console.c fsdrive.c fshand.c fsopnclo.c osmem.c \
            umem.c bdosmain.c fsbuf.c fsfat.c fsio.c iumem.c proc.c \
            fsdir.c fsglob.c fsmain.c kpgmld.c time.c 
bdos_ssrc = rwa.S

#
# source code in util/
#

util_csrc = doprintf.c nls.c langs.c string.c intmath.c
util_ssrc = memset.S memmove.S nlsasm.S setjmp.S miscasm.S stringasm.S

#
# source code in vdi/
#

vdi_csrc = vdi_main.c vdi_col.c vdi_control.c vdi_esc.c vdi_fill.c vdi_gdp.c \
           vdi_line.c vdi_marker.c vdi_misc.c vdi_mouse.c vdi_raster.c \
           vdi_input.c vdi_text.c vdi_bezier.c
vdi_ssrc = vdi_asm.S vdi_blit.S vdi_tblit.S


#
# source code in aes/
#

aes_csrc = gemaplib.c gemasync.c gemctrl.c gemdisp.c gemevlib.c \
           gemflag.c gemfmalt.c gemfmlib.c gemfslib.c gemgraf.c \
           gemgrlib.c gemgsxif.c geminit.c geminput.c gemmnlib.c gemobed.c \
           gemobjop.c gemoblib.c gempd.c gemqueue.c gemrslib.c gemsclib.c \
           gemshlib.c gemsuper.c gemwmlib.c gemwrect.c optimize.c \
           rectfunc.c gemdos.c gem_rsc.c
aes_ssrc = gemstart.S gemdosif.S gemasm.S gsx2.S large.S optimopt.S

#
# source code in desk/
#

desk_csrc = deskact.c deskapp.c deskdir.c deskfpd.c deskfun.c deskglob.c \
            deskinf.c deskins.c deskmain.c deskobj.c deskpro.c deskrsrc.c \
            desksupp.c deskwin.c gembind.c icons.c desk_rsc.c desk1.c
            #taddr.c deskgraf.c deskgsx.c
desk_ssrc = deskstart.S

#
# source code in cli/ for EmuTOS console EmuCON
#

cli_csrc = command.c
cli_ssrc = coma.S

#
# specific CC -c options for specific directories
#

bios_copts =
bdos_copts =
util_copts = -Ibios
cli_copts  = -Ibios
vdi_copts  = -Ibios
aes_copts  = -Ibios
desk_copts = -Ibios -Iaes -Idesk/icons -DDESK1

#
# Directory selection depending on the user interface (EmuCON or AES)
#

ifeq ($(WITH_AES),0)
 ui_dirs := vdi cli
 other_dirs := aes desk
else
 ifeq ($(WITH_CLI),0)
  ui_dirs := vdi aes desk
  other_dirs := cli
 else
  ui_dirs := vdi aes desk cli
  other_dirs :=
 endif 
endif

dirs := bios bdos util $(ui_dirs)

vpath %.c $(dirs)
vpath %.S $(dirs)

#
# country-specific settings
#

include country.mk

#
# everything should work fine below.
# 

COBJ = $(foreach d,$(dirs),$(patsubst %.c,obj/%.o,$($(d)_csrc)))
SOBJ = $(foreach d,$(dirs),$(patsubst %.S,obj/%.o,$($(d)_ssrc)))

CSRC = $(foreach d,$(dirs) $(other_dirs),$(addprefix $(d)/,$($(d)_csrc)))
SSRC = $(foreach d,$(dirs) $(other_dirs),$(addprefix $(d)/,$($(d)_ssrc)))

OBJECTS = $(SOBJ) $(COBJ) $(FONTOBJ) obj/version.o

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
	@echo "aranym  etos512k.img, suitable for ARAnyM" 
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

TOCLEAN += *.img *.map

emutos1.img emutos1.map: $(OBJECTS) Makefile
	$(LD) -o emutos1.img -Xlinker -Map -Xlinker emutos1.map \
	  $(OBJECTS) $(LDFLAGS) $(LDFLAGS_T1)

emutos2.img emutos2.map: $(OBJECTS) Makefile
	$(LD) -o emutos2.img -Xlinker -Map -Xlinker emutos2.map \
	  $(OBJECTS) $(LDFLAGS) $(LDFLAGS_T2)


#
# generic sized images handling
#

define sized_image
@goal=`echo $@ | sed -e 's/[^0-9]//g'`; \
size=`wc -c < $<`; goalbytes=`expr $$goal \* 1024`; \
if [ $$size -gt $$goalbytes ]; \
then \
  echo "EmuTOS too big for $${goal}K ($$goalbytes bytes): size = $$size"; \
  false; \
else \
  dd if=/dev/zero of=$@ bs=1024 count=$$goal 2>/dev/null; \
  dd if=$< of=$@ conv=notrunc 2>/dev/null; \
  echo "$@ done"; \
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

ifeq (,$(UNIQUE))
256: 
	@echo building $(COUNTRY)-only EmuTOS in etos256k.img; \
	make UNIQUE=$(COUNTRY) etos256k.img; 
else
256: etos256k.img
endif

etos256k.img: emutos2.img
	$(sized_image)

#
# 512kB Image (for Aranym or Falcon)
#

aranym:
	@echo building ARAnyM EmuTOS in etos512k.img; \
	make DEF='-DRTC_TOS_VER=0x404 -DUSE_STOP_INSN_TO_FREE_HOST_CPU=1 -DCONF_WITH_ACSI=0' 512; \

512: etos512k.img
falcon: help

etos512k.img: emutos2.img
	$(sized_image)


#
# ram - In two stages. first link emutos2.img to know the top address of bss, 
# then use this value (taken from the map) to relocate the RamTOS. 
#

TOCLEAN += boot.prg

ram: ramtos.img boot.prg

ramtos.img ramtos.map: $(OBJECTS) emutos2.map 
	@topbss=`sed -e '/__end/!d;s/^ *//;s/ .*//' emutos2.map`; \
	echo $(LD) -o ramtos.img $(OBJECTS) $(LDFLAGS) \
		-Xlinker -Ttext=$$topbss -Xlinker -Tbss=0; \
	$(LD) -o ramtos.img $(OBJECTS) $(LDFLAGS) \
		-Xlinker -Map -Xlinker ramtos.map \
		-Xlinker -Ttext=$$topbss -Xlinker -Tbss=0

boot.prg: obj/minicrt.o obj/boot.o obj/bootasm.o
	$(LD) -Xlinker -s -o $@ $+ -lgcc

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
	  -Xlinker -Map -Xlinker compr1.map $(LDFLAGS_T1)

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

TOCLEAN += compr$(EXE) uncompr$(EXE)

#
# flop
#

TOCLEAN += emutos.st mkflop$(EXE)

flop : emutos.st

fd0:	emutos.st
	dd if=$< of=/dev/fd0D360

emutos.st: mkflop$(EXE) bootsect.img ramtos.img
	./mkflop$(EXE)

bootsect.img : obj/bootsect.o
	$(LD) -Xlinker --oformat -Xlinker binary -o $@ obj/bootsect.o

mkflop$(EXE) : tools/mkflop.c
	$(NATIVECC) -o $@ $<

#
# Misc utilities
#

TOCLEAN += date.prg dumpkbd.prg

date.prg: obj/minicrt.o obj/doprintf.o obj/date.o
	$(LD) -Xlinker -s -o $@ $+ -lgcc

dumpkbd.prg: obj/minicrt.o obj/memmove.o obj/dumpkbd.o obj/doprintf.o \
	     obj/string.o
	$(LD) -Xlinker -s -o $@ $+ -lgcc

#
# NLS support
#

POFILES = po/fr.po po/de.po po/cs.po

TOCLEAN += bug$(EXE) util/langs.c po/messages.pot

bug$(EXE): tools/bug.c
	$(NATIVECC) -o $@ $<

ifeq (us,$(UNIQUE))

util/langs.c:
	echo > $@

else

util/langs.c: $(POFILES) po/LINGUAS bug$(EXE) po/messages.pot
	./bug$(EXE) make

obj/langs.o: include/config.h include/i18nconf.h

endif

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
	  make DEF='-DTOS_VERSION=0x102' WITH_CLI=0 UNIQUE=$$i 192; \
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

TRANS_SRC = $(shell sed -e '/^[^a-z]/d;s/\.c/.tr&/' <po/POTFILES.in)

TOCLEAN += */*.tr.c

ifneq (,$(UNIQUE))
ifneq (us,$(ETOSLANG))
emutos1.img emutos2.img ramtos.img: $(TRANS_SRC)

%.tr.c : %.c po/$(ETOSLANG).po bug$(EXE) po/LINGUAS obj/country
	./bug$(EXE) translate $(ETOSLANG) $<
endif
endif

#
# obj/country contains the current values of $(COUNTRY) and $(UNIQUE). 
# whenever it changes, whatever necessary steps are taken so that the
# correct files get re-compiled, even without doing make depend.
#

TOCLEAN += obj/country

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
	for i in $(TRANS_SRC); \
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

TOCLEAN += include/i18nconf.h

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
# ctables.h - the country tables, generated from country.mk, and only
# included in bios/country.c
#

TOCLEAN += bios/ctables.h

bios/ctables.h: country.mk tools/genctables.awk
	awk -f tools/genctables.awk < country.mk > $@

obj/country.o: bios/ctables.h

#
# OS header
#

TOCLEAN += bios/header.h

obj/startup.o: bios/header.h

obj/comprimg.o: bios/header.h

obj/country.o: bios/header.h

bios/header.h: tools/mkheader.awk obj/country include/i18nconf.h
	awk -f tools/mkheader.awk $(COUNTRY) > $@

#
# build rules - the little black magic here allows for e.g.
# $(bios_copts) to specify additional options for C source files
# in bios/, and $(vdi_sopts) to specify additional options for
# ASM source files in vdi/
#

TOCLEAN += obj/*.o */*.dsm

obj/%.o : %.tr.c
	$(CC) $(CFLAGS) $($(subst /,_,$(dir $<))copts) -c $< -o $@

obj/%.o : %.c
	$(CC) $(CFLAGS) $($(subst /,_,$(dir $<))copts) -c $< -o $@

obj/%.o : %.S
	$(CC) $(CFLAGS) $($(subst /,_,$(dir $<))sopts) -c $< -o $@

%.dsm : %.c
	$(CC) $(CFLAGS) $($(subst /,_,$(dir $<))copts) -S $< -o $@

#
# version string
# 

TOCLEAN += obj/*.c

obj/version.c: doc/changelog.txt tools/version.sed
	sed -f tools/version.sed doc/changelog.txt > $@

obj/version.o: obj/version.c
	$(CC) $(CFLAGS) -c $< -o $@

#
# generic dsm handling
#

TOCLEAN += *.dsm dsm.txt fal_dsm.txt

%.dsm: %.map
	vma=`sed -e '/^\.text/!d;s/[^0]*//;s/ .*//;q' $<`; \
	$(OBJDUMP) --target=binary --architecture=m68k \
	  --adjust-vma=$$vma -D $*.img \
	  | sed -e '/:/!d;/^  /!d;s/^  //;s/^ /0/;s/:	/: /' > dsm.tmp
	sed -e '/^ *0x00/!d;s///;s/  */:  /' $< > map.tmp
	cat dsm.tmp map.tmp | LC_ALL=C sort > $@
	rm -f dsm.tmp map.tmp

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
# indent - indents the files except when there are warnings
# checkindent - check for indent warnings, but do not alter files.
#

INDENTFILES = bdos/*.c bios/*.c util/*.c tools/*.c desk/*.c aes/*.c vdi/*.c

checkindent:
	@err=0 ; \
	for i in $(INDENTFILES) ; do \
		$(INDENT) <$$i 2>err.tmp >/dev/null; \
		if test -s err.tmp ; then \
			err=`expr $$err + 1`; \
			echo in $$i:; \
			cat err.tmp; \
		fi \
	done ; \
	rm -f err.tmp; \
	if [ $$err -ne 0 ] ; then \
		echo indent issued warnings on $$err 'file(s)'; \
		false; \
	else \
		echo done.; \
	fi

indent:
	@err=0 ; \
	for i in $(INDENTFILES) ; do \
		$(INDENT) <$$i 2>err.tmp | expand >indent.tmp; \
		if ! test -s err.tmp ; then \
			if ! cmp -s indent.tmp $$i ; then \
				echo indenting $$i; \
				mv $$i $$i~; \
				mv indent.tmp $$i; \
			fi \
		else \
			err=`expr $$err + 1`; \
			echo in $$i:; \
			cat err.tmp; \
		fi \
	done ; \
	rm -f err.tmp indent.tmp; \
	if [ $$err -ne 0 ] ; then \
		echo $$err 'file(s)' untouched because of warnings; \
		false; \
	fi


#
# cvsready
#

TOCLEAN += tounix$(EXE)

expand:
	@for i in `grep -l '	' */*.[chS] */*.awk` ; do \
		echo expanding $$i; \
		expand <$$i >expand.tmp; \
		mv expand.tmp $$i; \
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
	./$< * bios/* bdos/* doc/* util/* tools/* po/* include/* aes/* desk/*

cvsready: expand crlf

#
# create a tgz archive named project-nnnnnn.tgz, where nnnnnn is the date.
#

HEREDIR = $(shell basename $(shell pwd))
TGZ = $(shell echo $(HEREDIR)-`date +%y%m%d`|tr A-Z a-z).tgz

tgz:	distclean
	cd ..;\
	tar -cf - --exclude '*CVS' $(HEREDIR) | gzip -c -9 >$(TGZ)

#
# proposal to create an archive named emutos-0_2a.tgz when
# the EMUTOS_VERSION equals "0.2a" in include/version.h
# (THIS IS CURRENTLY BROKEN)
#
#VERSION = $(shell grep EMUTOS_VERSION include/version.h | cut -f2 -d\")
#RELEASEDIR = emutos-$(VERSION)
#RELEASETGZ = $(shell echo $(RELEASEDIR) | tr A-Z. a-z_).tgz#
#
#release: distclean
#	@tmp=tmpCVS; rm -rf $$tmp; mkdir $$tmp; cd $$tmp; \
#	ln -s ../../$(HEREDIR) $(RELEASEDIR); \
#	tar -h -cf - --exclude '*CVS' $(RELEASEDIR) \
#	| gzip -c -9 >../../$(RELEASETGZ);\
#	cd ..; rm -rf $$tmp
#

#
# file dependencies (makefile.dep)
#

TOCLEAN += makefile.dep

depend: util/langs.c bios/header.h
	( \
	  $(CC) -MM $(INC) -Ibios -Iaes -Idesk/icons $(DEF) $(CSRC); \
	  $(CC) -MM $(INC) $(DEF) $(SSRC) \
	) | sed -e '/:/s,^,obj/,' >makefile.dep

ifneq (,$(wildcard makefile.dep))
include makefile.dep
endif

#
# local Makefile
#

ifneq (,$(wildcard local.mk))
include local.mk
endif

#
# clean and distclean 
# (distclean is called before creating a tgz archive)
#

clean:
	rm -f $(TOCLEAN)

distclean: clean
	rm -f '.#'* */'.#'* 

