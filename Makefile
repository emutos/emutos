#
# Makefile - the EmuTOS overbloated Makefile
#
# Copyright (c) 2001-2015 The EmuTOS development team.
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
# General settings
#

MAKEFLAGS = --no-print-directory

#
# EmuTOS version
#

VERSION = $(shell date +%Y%m%d)
#VERSION = 0.9.5

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
UNIQUE =
ifneq (,$(UNIQUE))
COUNTRY = $(UNIQUE)
endif

#
# Choose the features that should be included into EmuTOS
#

# Include the AES and EmuDesk
WITH_AES=1

# Include EmuCON
WITH_CLI=1

#
# crude machine detection (Unix or Cygwin)
#

ifneq (,$(findstring CYGWIN,$(shell uname)))
# CYGWIN-dependent stuff
#EXE = .exe
CORE = *.stackdump
DDOPTS = iflag=binary oflag=binary
else
# ordinary Unix stuff
EXE =
CORE = core
DDOPTS =
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
# NODEP will accumulate the names of the targets which does not need to include
# makefile.dep, to avoid rebuilding that file when not necessary.
# This includes targets not using $(CC) and targets recrsively invoking $(MAKE).
#

NODEP :=

#
# compilation flags
#

# Override with 1 to use the ELF toolchain instead of the MiNT one
ELF = 0

ifeq (1,$(ELF))
# Standard ELF toolchain
TOOLCHAIN_PREFIX = m68k-elf-
TOOLCHAIN_CFLAGS = -fleading-underscore -Wa,--register-prefix-optional -fno-reorder-functions -DELF_TOOLCHAIN
else
# MiNT toolchain
TOOLCHAIN_PREFIX = m68k-atari-mint-
TOOLCHAIN_CFLAGS =
endif

# indent flags
INDENT = indent -kr

# Linker with relocation information and binary output (image)
LD = $(CC) $(MULTILIBFLAGS) -nostartfiles -nostdlib
VMA_STANDARD = 0x00e00000
VMA_192 = 0x00fc0000
VMA = $(VMA_STANDARD)
LDFLAGS = -lgcc -Wl,--oformat,binary,-Ttext=$(VMA),-Tbss=0x00000000,-e,_main

# C compiler
CC = $(TOOLCHAIN_PREFIX)gcc
ifeq (1,$(COLDFIRE))
CPUFLAGS = -mcpu=5475
else
CPUFLAGS = -m68000
endif
MULTILIBFLAGS = $(CPUFLAGS) -mshort
INC = -Iinclude
OPTFLAGS = -O2
OTHERFLAGS = -ffreestanding -fomit-frame-pointer

WARNFLAGS = -Wall -Wundef
#-fno-common -Wshadow -Wmissing-prototypes -Wstrict-prototypes
#-Werror

GCCVERSION := $(shell $(CC) -dumpversion | cut -d. -f1)
# add warning flags not supported by GCC v2
ifneq (,$(GCCVERSION))
ifneq (2,$(GCCVERSION))
WARNFLAGS += -Wold-style-definition
endif
endif

DEFINES = $(LOCALCONF) -DWITH_AES=$(WITH_AES) -DWITH_CLI=$(WITH_CLI) $(DEF)
CFLAGS = $(MULTILIBFLAGS) $(TOOLCHAIN_CFLAGS) $(OPTFLAGS) $(WARNFLAGS) $(OTHERFLAGS) $(INC) $(DEFINES)

CPPFLAGS = $(INC)

# The objdump utility (disassembler)
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump

# The objcopy utility
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy

# the native C compiler, for tools
NATIVECC = gcc -ansi -pedantic $(WARNFLAGS) -W -O

#
# source code in bios/
# Note: tosvars.o must be the first object linked.

bios_csrc = kprint.c xbios.c chardev.c blkdev.c bios.c clock.c \
            mfp.c parport.c serport.c biosmem.c acsi.c \
            midi.c ikbd.c sound.c dma.c floppy.c disk.c screen.c videl.c lineainit.c \
            mouse.c initinfo.c cookie.c machine.c nvram.c country.c \
            xhdi.c natfeats.c font.c conout.c vt52.c dmasound.c ide.c amiga.c aros.c \
            delay.c pmmu030.c sd.c
bios_ssrc = tosvars.S startup.S aciavecs.S vectors.S lineavars.S \
            processor.S memory.S linea.S panicasm.S kprintasm.S \
            natfeat.S amiga2.S aros2.S delayasm.S 68040_pmmu.S

ifeq (1,$(COLDFIRE))
  bios_csrc += coldfire.c spi.c
  bios_ssrc += coldfire2.S
endif

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
vdi_ssrc = vdi_asm.S

ifeq (1,$(COLDFIRE))
vdi_ssrc += vdi_tblit_cf.S
else
vdi_ssrc += vdi_blit.S vdi_tblit.S
endif


#
# source code in aes/
#

aes_csrc = gemaplib.c gemasync.c gemctrl.c gemdisp.c gemevlib.c \
           gemflag.c gemfmalt.c gemfmlib.c gemfslib.c gemgraf.c \
           gemgrlib.c gemgsxif.c geminit.c geminput.c gemmnlib.c gemobed.c \
           gemobjop.c gemoblib.c gempd.c gemqueue.c gemrslib.c gemsclib.c \
           gemshlib.c gemsuper.c gemwmlib.c gemwrect.c optimize.c \
           rectfunc.c gemdos.c gem_rsc.c gsx2.c
aes_ssrc = gemstart.S gemdosif.S gemasm.S optimopt.S

#
# source code in desk/
#

desk_csrc = deskact.c deskapp.c deskdir.c deskfpd.c deskfun.c deskglob.c \
            deskinf.c deskins.c deskmain.c deskobj.c deskpro.c deskrsrc.c \
            desksupp.c deskwin.c gembind.c desk_rsc.c icons.c desk1.c \
            deskrez.c
desk_ssrc = deskstart.S

#
# source code in cli/ for EmuTOS console EmuCON
#

cli_csrc = cmdedit.c cmdexec.c cmdint.c cmdmain.c cmdparse.c cmdutil.c
cli_ssrc = cmdasm.S

#
# specific CC -c options for specific directories
#

bios_copts =
bdos_copts =
util_copts =
cli_copts  =
vdi_copts  =
aes_copts  =
desk_copts =

#
# Makefile functions
#

# Shell command to get the address of a symbol
FUNCTION_SHELL_GET_SYMBOL_ADDRESS = grep -E ' $(1)( |$$)' $(2) |awk '{print $$1}'

# membot (LONG at 0x432) is the bottom of the free RAM
# We can guess its value from the map file
SHELL_GET_MEMBOT_EMUTOS_MAP = $(call FUNCTION_SHELL_GET_SYMBOL_ADDRESS,_end,emutos.map)
SHELL_GET_MEMBOT_RAMTOS_MAP = $(call FUNCTION_SHELL_GET_SYMBOL_ADDRESS,_edata,ramtos.map)

# The following reference values have been gathered from major TOS versions
MEMBOT_TOS102 = 0x0000ca00
MEMBOT_TOS104 = 0x0000a84e
MEMBOT_TOS162 = 0x0000a832
MEMBOT_TOS206 = 0x0000ccb2
MEMBOT_TOS305 = 0x0000e6fc
MEMBOT_TOS404 = 0x0000f99c

#
# Directory selection depending on the features
#

dirs = bios bdos util vdi

ifeq ($(WITH_AES),1)
 dirs += aes desk
endif

ifeq ($(WITH_CLI),1)
 dirs += cli
endif

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

CSRC = $(foreach d,$(dirs),$(addprefix $(d)/,$($(d)_csrc)))
SSRC = $(foreach d,$(dirs),$(addprefix $(d)/,$($(d)_ssrc)))

OBJECTS = $(SOBJ) $(COBJ) $(FONTOBJ) obj/version.o

#
# production targets
#

.PHONY: all
NODEP += all
all:	help

.PHONY: help
NODEP += help
help: UNIQUE = $(COUNTRY)
help:
	@echo "target  meaning"
	@echo "------  -------"
	@echo "help    this help message"
	@echo "version display the EmuTOS version"
	@echo "192     $(ROM_192), EmuTOS ROM padded to size 192 KB (starting at $(VMA_192))"
	@echo "256     $(ROM_256), EmuTOS ROM padded to size 256 KB (starting at $(VMA_STANDARD))"
	@echo "512     $(ROM_512), EmuTOS ROM padded to size 512 KB (starting at $(VMA_STANDARD))"
	@echo "aranym  $(ROM_ARANYM), suitable for ARAnyM"
	@echo "firebee $(SREC_FIREBEE), to be flashed on the FireBee"
	@echo "firebee-ram emutos.prg, a RAM tos for the FireBee"
	@echo "amiga   $(ROM_AMIGA), EmuTOS ROM for Amiga hardware"
	@echo "amigakd $(AMIGA_KICKDISK), EmuTOS as Amiga 1000 Kickstart disk"
	@echo "m548x-dbug $(SREC_M548X_DBUG), EmuTOS-RAM for dBUG on ColdFire Evaluation Boards"
	@echo "m548x-bas  $(SREC_M548X_BAS), EmuTOS for BaS_gcc on ColdFire Evaluation Boards"
	@echo "all192  all 192 KB images"
	@echo "all256  all 256 KB images"
	@echo "allbin  all 192 KB, 256 KB and 512 KB images"
	@echo "ram     emutos.prg, a RAM tos"
	@echo "flop    emutos.st, a bootable floppy with RAM tos"
	@echo "cart    $(ROM_CARTRIDGE), EmuTOS as a diagnostic cartridge"
	@echo "clean"
	@echo "expand  expand tabs to spaces"
	@echo "crlf    convert all end of lines to LF"
	@echo "charset check the charset of all the source files"
	@echo "gitready same as $(MAKE) expand crlf"
	@echo "depend  creates dependancy file (makefile.dep)"
	@echo "dsm     dsm.txt, an edited disassembly of emutos.img"
	@echo "*.dsm   disassembly of any .c or almost any .img file"
	@echo "release build the release archives into $(RELEASE_DIR)"

# Display the EmuTOS version
.PHONY: version
NODEP += version
version:
	@echo '$(VERSION)'

#
# the maps must be built at the same time as the images, to enable
# one generic target to deal with all edited disassembly.
#

TOCLEAN += *.img *.map

emutos.img emutos.map: $(OBJECTS) Makefile
	$(LD) -o emutos.img $(OBJECTS) $(LDFLAGS) -Wl,-Map,emutos.map

#
# 128kB Image
#

ROM_128 = etos128k.img

$(ROM_128): ROMSIZE = 128
$(ROM_128): emutos.img mkrom$(EXE)
	./mkrom$(EXE) pad $(ROMSIZE)k $< $(ROM_128)

#
# 192kB Image
#

ROM_192 = etos192$(UNIQUE).img

.PHONY: 192
NODEP += 192
192: UNIQUE = $(COUNTRY)
192:
	$(MAKE) DEF='-DTARGET_192' OPTFLAGS=-Os WITH_CLI=0 UNIQUE=$(UNIQUE) ROM_192=$(ROM_192) $(ROM_192)
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS102))) bytes more than TOS 1.02)"

$(ROM_192): ROMSIZE = 192
$(ROM_192): VMA = $(VMA_192)
$(ROM_192): emutos.img mkrom$(EXE)
	./mkrom$(EXE) pad $(ROMSIZE)k $< $(ROM_192)

#
# 256kB Image
#

ROM_256 = etos256$(UNIQUE).img

.PHONY: 256
NODEP += 256
256: UNIQUE = $(COUNTRY)
256:
	$(MAKE) DEF='-DTARGET_256' OPTFLAGS=-Os UNIQUE=$(UNIQUE) ROM_256=$(ROM_256) $(ROM_256)
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS162))) bytes more than TOS 1.62)"

$(ROM_256): ROMSIZE = 256
$(ROM_256): emutos.img mkrom$(EXE)
	./mkrom$(EXE) pad $(ROMSIZE)k $< $(ROM_256)

#
# 512kB Image (for Falcon)
#

ROM_512 = etos512k.img
SYMFILE = $(addsuffix .sym,$(basename $(ROM_512)))

.PHONY: 512
512: DEF = -DTARGET_512
512: $(ROM_512) $(SYMFILE)
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS404))) bytes more than TOS 4.04)"

$(ROM_512): ROMSIZE = 512
$(ROM_512): emutos.img mkrom$(EXE)
	./mkrom$(EXE) pad $(ROMSIZE)k $< $(ROM_512)

.PHONY: falcon
falcon: help

#
# ARAnyM Image
#

ROM_ARANYM = emutos-aranym.img

.PHONY: aranym
NODEP += aranym
aranym:
	@echo "# Building ARAnyM EmuTOS into $(ROM_ARANYM)"
	$(MAKE) CPUFLAGS='-m68040' DEF='-DMACHINE_ARANYM' ROM_512=$(ROM_ARANYM) $(ROM_ARANYM)
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS404))) bytes more than TOS 4.04)"

#
# Diagnostic Cartridge Image
#

TOCLEAN += *.stc
ROM_CARTRIDGE = etoscart.img

.PHONY: cart
NODEP += cart
cart:
	@echo "# Building Diagnostic Cartridge EmuTOS into $(ROM_CARTRIDGE)"
	$(MAKE) OPTFLAGS=-Os DEF='-DTARGET_CART' UNIQUE=$(COUNTRY) WITH_AES=0 VMA=0x00fa0000 ROM_128=$(ROM_CARTRIDGE) $(ROM_CARTRIDGE)
	./mkrom$(EXE) stc emutos.img emutos.stc
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS102))) bytes more than TOS 1.02)"

#
# Amiga Image
#

TOCLEAN += *.rom

ROM_AMIGA = emutos-amiga.rom
AMIGA_DEFS = -DMACHINE_AMIGA

# AROS support is disabled by default due to license issues
AROS = 0
ifeq (1,$(AROS))
AMIGA_DEFS += -DCONF_WITH_AROS=1
endif

.PHONY: amiga
NODEP += amiga
amiga: UNIQUE = $(COUNTRY)
amiga:
	@echo "# Building Amiga EmuTOS into $(ROM_AMIGA)"
	$(MAKE) DEF='$(AMIGA_DEFS)' UNIQUE=$(UNIQUE) VMA=0x00fc0000 $(ROM_AMIGA)
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS162))) bytes more than TOS 1.62)"

$(ROM_AMIGA): emutos.img mkrom$(EXE)
	./mkrom$(EXE) amiga $< $(ROM_AMIGA)

#
# Amiga Kickstart disk image for Amiga 1000
#

TOCLEAN += *.adf

AMIGA_KICKDISK = emutos-kickdisk.adf

.PHONY: amigakd
NODEP += amigakd
amigakd: amiga
	./mkrom$(EXE) amiga-kickdisk $(ROM_AMIGA) $(AMIGA_KICKDISK)

#
# ColdFire images
#

TOCLEAN += *.s19
SRECFILE = emutos.s19
LMA = $(VMA)

$(SRECFILE): emutos.img
	$(OBJCOPY) -I binary -O srec --change-addresses $(LMA) $< $(SRECFILE)

SREC_FIREBEE = emutosfb.s19

.PHONY: firebee
NODEP += firebee
firebee:
	@echo "# Building FireBee EmuTOS into $(SREC_FIREBEE)"
	$(MAKE) COLDFIRE=1 CPUFLAGS='-mcpu=5474' DEF='-DMACHINE_FIREBEE' LMA=0xe0600000 SRECFILE=$(SREC_FIREBEE) $(SREC_FIREBEE)
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS404))) bytes more than TOS 4.04)"

.PHONY: firebee-ram
NODEP += firebee-ram
firebee-ram:
	@echo "# Building FireBee EmuTOS for RAM"
	$(MAKE) COLDFIRE=1 CPUFLAGS='-mcpu=5474' DEF='-DMACHINE_FIREBEE' ram

SREC_M548X_DBUG = emutos-m548x-dbug.s19
.PHONY: m548x-dbug
NODEP += m548x-dbug
m548x-dbug: UNIQUE = $(COUNTRY)
m548x-dbug:
	@echo "# Building M548x dBUG EmuTOS in $(SREC_M548X_DBUG)"
	$(MAKE) COLDFIRE=1 DEF='-DMACHINE_M548X' UNIQUE=$(UNIQUE) SRECFILE=$(SREC_M548X_DBUG) $(SREC_M548X_DBUG)
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS404))) bytes more than TOS 4.04)"

SREC_M548X_BAS = emutos-m548x-bas.s19
.PHONY: m548x-bas
NODEP += m548x-bas
m548x-bas: UNIQUE = $(COUNTRY)
m548x-bas:
	@echo "# Building M548x BaS_gcc EmuTOS in $(SREC_M548X_BAS)"
	$(MAKE) COLDFIRE=1 DEF='-DMACHINE_M548X -DCONF_WITH_BAS_MEMORY_MAP=1 -DCONF_WITH_PSEUDO_COLD_BOOT=1' LMA=0xe0100000 UNIQUE=$(UNIQUE) SRECFILE=$(SREC_M548X_BAS) $(SREC_M548X_BAS)
	@MEMBOT=$$($(SHELL_GET_MEMBOT_EMUTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_TOS404))) bytes more than TOS 4.04)"

#
# ram - In two stages. first link emutos.img to know the top address of bss,
# then use this value (taken from the map) to relocate the RamTOS.
#

TOCLEAN += emutos.prg

.PHONY: ram
ram: override DEF += -DTARGET_PRG
ram: ramtos.img emutos.prg
	@MEMBOT=$$($(SHELL_GET_MEMBOT_RAMTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes"

.PHONY: emutos-ram
emutos-ram:
	@echo '# First pass to build emutos.map and determine the end of the BSS'
	$(MAKE) emutos.map DEF='$(DEF)'

ramtos.img ramtos.map: VMA = $(shell $(SHELL_GET_MEMBOT_EMUTOS_MAP))
ramtos.img ramtos.map: emutos-ram
	@echo '# Second pass to build ramtos.img with TEXT and DATA just after the BSS'
	$(LD) -o ramtos.img $(OBJECTS) $(LDFLAGS) -Wl,-Map,ramtos.map

# incbin dependencies are not automatically generated
obj/ramtos.o: ramtos.img

# Be sure to keep ramtos.o before boot files.
# This simplifies the algorithm in bootasm.S.
emutos.prg: obj/minicrt.o obj/ramtos.o obj/boot.o obj/bootasm.o
	$(LD) -s -o $@ $+ -lgcc

#
# compressed ROM image
#

# The following hack allows to build the shared sources with different
# preprocessor defines
obj/compr-%.o : %.S
	$(CC) $(SFILE_FLAGS) -c $< -o $@

COMPROBJ = obj/compr-tosvars.o obj/compr-comprimg.o obj/compr-memory.o \
           obj/compr-processor.o obj/compr-uncompr.o

# ROM stub for compressed ROM image
compr.img compr.map: OPTFLAGS = -Os
compr.img compr.map: override DEF += -DTARGET_COMPR_STUB
compr.img compr.map: $(COMPROBJ)
	$(LD) -o compr.img $(COMPROBJ) $(LDFLAGS) -Wl,-Map,compr.map

# Compressed ROM: stub + ramtos
.PHONY: etoscpr.img
etoscpr.img: compr.img compr$(EXE)
	$(MAKE) DEF='-DTARGET_COMPRESSED_ROM' OPTFLAGS=-Os UNIQUE=$(UNIQUE) ramtos.img
	./compr$(EXE) --rom compr.img ramtos.img $@

# 256k compressed ROM (intermediate target)
ecpr256k.img: ROMSIZE = 256
ecpr256k.img: etoscpr.img mkrom$(EXE)
	./mkrom$(EXE) pad $(ROMSIZE)k $< $@

# 192k compressed ROM (intermediate target)
ecpr192k.img: ROMSIZE = 192
ecpr192k.img: VMA = $(VMA_192)
ecpr192k.img: etoscpr.img mkrom$(EXE)
	./mkrom$(EXE) pad $(ROMSIZE)k $< $@

# 256k compressed ROM (main target)
.PHONY: 256c
NODEP += 256c
256c: UNIQUE = $(COUNTRY)
256c:
	$(MAKE) UNIQUE=$(UNIQUE) ecpr256k.img

# 192k compressed ROM (main target)
.PHONY: 192c
NODEP += 192c
192c: UNIQUE = $(COUNTRY)
192c:
	$(MAKE) UNIQUE=$(UNIQUE) ecpr192k.img

# Compression tool
NODEP += compr$(EXE)
compr$(EXE): tools/compr.c
	$(NATIVECC) -o $@ $<

# Decompression tool
NODEP += uncompr$(EXE)
uncompr$(EXE): tools/uncompr.c
	$(NATIVECC) -o $@ $<

# Compression test
.PHONY: comprtest
NODEP += comprtest
comprtest: compr$(EXE) uncompr$(EXE)
	$(SHELL) tools/comprtst.sh

TOCLEAN += compr$(EXE) uncompr$(EXE)

#
# flop
#

TOCLEAN += emutos.st mkflop$(EXE)

.PHONY: flop
NODEP += flop
flop: UNIQUE = $(COUNTRY)
flop:
	$(MAKE) UNIQUE=$(UNIQUE) emutos.st
	@MEMBOT=$$($(SHELL_GET_MEMBOT_RAMTOS_MAP));\
	echo "# RAM used: $$(($$MEMBOT)) bytes"

.PHONY: fd0
NODEP += fd0
fd0: flop
	dd if=emutos.st of=/dev/fd0D360

emutos.st: override DEF += -DTARGET_FLOPPY
emutos.st: mkflop$(EXE) bootsect.img ramtos.img
	./mkflop$(EXE)

bootsect.img : obj/bootsect.o
	$(LD) -Wl,--oformat,binary -o $@ obj/bootsect.o

NODEP += mkflop$(EXE)
mkflop$(EXE) : tools/mkflop.c
	$(NATIVECC) -o $@ $<

#
# Misc utilities
#

TOCLEAN += date.prg dumpkbd.prg

date.prg: obj/minicrt.o obj/doprintf.o obj/date.o
	$(LD) -s -o $@ $+ -lgcc

dumpkbd.prg: obj/minicrt.o obj/memmove.o obj/dumpkbd.o obj/doprintf.o \
	     obj/string.o
	$(LD) -s -o $@ $+ -lgcc

#
# NLS support
#

POFILES = $(wildcard po/*.po)

TOCLEAN += bug$(EXE) util/langs.c po/messages.pot

NODEP += bug$(EXE)
bug$(EXE): tools/bug.c
	$(NATIVECC) -o $@ $<

util/langs.c: $(POFILES) po/LINGUAS bug$(EXE) po/messages.pot
	./bug$(EXE) make

po/messages.pot: bug$(EXE) po/POTFILES.in $(shell grep -v '^#' po/POTFILES.in)
	./bug$(EXE) xgettext

#
# Resource support
#

TOCLEAN += erd$(EXE) grd$(EXE) ird$(EXE)

NODEP += erd$(EXE) grd$(EXE) ird$(EXE)
erd$(EXE): tools/erd.c
	$(NATIVECC) -o $@ $<
grd$(EXE): tools/erd.c
	$(NATIVECC) -DGEM_RSC -o grd $<
ird$(EXE): tools/erd.c
	$(NATIVECC) -DICON_RSC -o ird $<

DESKRSC_BASE = desk/desktop
DESKRSCGEN_BASE = desk/desk_rsc
GEMRSC_BASE = aes/gem
GEMRSCGEN_BASE = aes/gem_rsc
ICONRSC_BASE = desk/icon
ICONRSCGEN_BASE = desk/icons
TOCLEAN += $(DESKRSCGEN_BASE).c $(DESKRSCGEN_BASE).h $(GEMRSCGEN_BASE).c $(GEMRSCGEN_BASE).h $(ICONRSCGEN_BASE).c $(ICONRSCGEN_BASE).h

$(DESKRSCGEN_BASE).c $(DESKRSCGEN_BASE).h: erd$(EXE) $(DESKRSC_BASE).rsc $(DESKRSC_BASE).def
	./erd$(EXE) -pdesk $(DESKRSC_BASE) $(DESKRSCGEN_BASE)
$(GEMRSCGEN_BASE).c $(GEMRSCGEN_BASE).h: grd$(EXE) $(GEMRSC_BASE).rsc $(GEMRSC_BASE).def
	./grd$(EXE) $(GEMRSC_BASE) $(GEMRSCGEN_BASE)
$(ICONRSCGEN_BASE).c $(ICONRSCGEN_BASE).h: ird$(EXE) $(ICONRSC_BASE).rsc $(ICONRSC_BASE).def
	./ird$(EXE) -picon $(ICONRSC_BASE) $(ICONRSCGEN_BASE)

#
# Special ROM support
#

TOCLEAN += mkrom$(EXE)

NODEP += mkrom$(EXE)
mkrom$(EXE): tools/mkrom.c
	$(NATIVECC) -o $@ $<

# test target to build all tools
.PHONY: tools
NODEP += tools
tools: bug$(EXE) compr$(EXE) erd$(EXE) mkflop$(EXE) mkrom$(EXE) tos-lang-change$(EXE) tounix$(EXE) uncompr$(EXE)

# user tool, not needed in EmuTOS building
TOCLEAN += tos-lang-change$(EXE)
NODEP += tos-lang-change$(EXE)
tos-lang-change$(EXE): tools/tos-lang-change.c
	$(NATIVECC) -o $@ $<

#
# all binaries
#

.PHONY: allbin
NODEP += allbin
allbin:
	@echo "# Building $(ROM_512)"
	$(MAKE) $(ROM_512)
	$(RM) obj/*.o
	$(MAKE) all256
	$(RM) obj/*.o
	$(MAKE) all192

# The sleep command in targets below ensure that all the generated sources
# will have a timestamp older than any object file.
# This matters on filesystems having low timestamp resolution (ext2, ext3).

.PHONY: all256
NODEP += all256
all256:
	@for i in $(COUNTRIES); \
	do \
	  echo; \
	  echo "sleep 1"; \
	  sleep 1; \
	  $(MAKE) 256 UNIQUE=$$i || exit 1; \
	done

.PHONY: all192
NODEP += all192
all192:
	@for i in $(COUNTRIES); \
	do \
	  echo; \
	  echo "sleep 1"; \
	  sleep 1; \
	  $(MAKE) 192 UNIQUE=$$i || exit 1; \
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
emutos.img ramtos.img: $(TRANS_SRC)

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

# A phony target is never up to date.
.PHONY: always-execute-recipe

# The recipe of a target depending on a phony target will always be executed
# in order to determine if the target is up to date or not.
# If the recipe does not touch the target, it is considered up to date.
obj/country: always-execute-recipe
	@echo $(COUNTRY) $(UNIQUE) > last.tmp; \
	if [ -e $@ ]; \
	then \
	  if cmp -s last.tmp $@; \
	  then \
	    rm last.tmp; \
	    exit 0; \
	  fi; \
	fi; \
	echo "echo $(COUNTRY) $(UNIQUE) > $@"; \
	mv last.tmp $@; \
	for i in $(TRANS_SRC); \
	do \
	  j=obj/`basename $$i tr.c`o; \
	  echo "rm -f $$i $$j"; \
	  rm -f $$i $$j; \
	done

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
include/i18nconf.h: obj/country
	@echo '# Generating $@ with CONF_LANG="$(ETOSLANG)" CONF_KEYB=KEYB_$(ETOSKEYB) CONF_CHARSET=CHARSET_$(ETOSCSET)'
	@rm -f $@; touch $@
	@echo '#define CONF_MULTILANG 0' >> $@
	@echo '#define CONF_WITH_NLS 0' >> $@
	@echo '#define CONF_LANG "$(ETOSLANG)"' >> $@
	@echo '#define CONF_KEYB KEYB_$(ETOSKEYB)' >> $@
	@echo '#define CONF_CHARSET CHARSET_$(ETOSCSET)' >> $@
	@echo "#define CONF_IDT ($(ETOSIDT))" >> $@
else
include/i18nconf.h: obj/country
	@echo '# Generating $@ with CONF_MULTILANG=1'
	@rm -f $@; touch $@
	@echo '#define CONF_MULTILANG 1' >> $@
	@echo '#define CONF_WITH_NLS 1' >> $@
endif

#
# ctables.h - the country tables, generated from country.mk, and only
# included in bios/country.c
#

TOCLEAN += bios/ctables.h

bios/ctables.h: country.mk tools/genctables.awk
	awk -f tools/genctables.awk < country.mk > $@

#
# OS header
#

TOCLEAN += bios/header.h

bios/header.h: tools/mkheader.awk obj/country
	awk -f tools/mkheader.awk $(COUNTRY) > $@

#
# build rules - the little black magic here allows for e.g.
# $(bios_copts) to specify additional options for C source files
# in bios/, and $(vdi_sopts) to specify additional options for
# ASM source files in vdi/
#

TOCLEAN += obj/*.o */*.dsm

CFILE_FLAGS = $(strip $(CFLAGS) $($(subst /,_,$(dir $<))copts))
SFILE_FLAGS = $(strip $(CFLAGS) $($(subst /,_,$(dir $<))sopts))

obj/%.o : %.tr.c
	$(CC) $(CFILE_FLAGS) -c $< -o $@

obj/%.o : %.c
	$(CC) $(CFILE_FLAGS) -c $< -o $@

obj/%.o : %.S
	$(CC) $(SFILE_FLAGS) -c $< -o $@

%.dsm : %.c
	$(CC) $(CFILE_FLAGS) -S $< -o $@

#
# version string
#

TOCLEAN += obj/*.c

# This temporary file is always generated
obj/version2.c:
	@echo '/* Generated from Makefile */' > $@
	@echo 'const char version[] = "$(VERSION)";' >> $@

# If the official version file is different than the temporary one, update it
obj/version.c: obj/version2.c
	@if ! cmp -s $@ $< ; then \
	  echo '# Updating $@ with VERSION=$(VERSION)' ; \
	  cp $< $@ ; \
	fi ; \
	rm $<

obj/version.o: obj/version.c
	$(CC) $(CFILE_FLAGS) -c $< -o $@

#
# generic dsm handling
#

TOCLEAN += *.dsm dsm.txt

%.dsm: %.map %.img
	vma=`sed -e '/^\.text/!d;s/[^0]*//;s/ .*//;q' $<`; \
	$(OBJDUMP) --target=binary --architecture=m68k \
	  --adjust-vma=$$vma -D $*.img \
	  | sed -e '/^ *[0-9a-f]*:/!d;s/^   /000/;s/^  /00/;s/:	/: /' > dsm.tmp
	sed -e '/^ *0x/!d;s///;s/  */:  /' $< > map.tmp
	cat dsm.tmp map.tmp | LC_ALL=C sort > $@
	rm -f dsm.tmp map.tmp

dsm.txt: emutos.dsm
	cp $< $@

.PHONY: dsm
dsm: dsm.txt

.PHONY: show
show: dsm.txt
	cat dsm.txt

#
# Hatari symbols file
#

TOCLEAN += *.sym

%.sym: emutos.map tools/map2sym.sh
	$(SHELL) tools/map2sym.sh $< >$@

#
# indent - indents the files except when there are warnings
# checkindent - check for indent warnings, but do not alter files.
#

INDENTFILES = bdos/*.c bios/*.c util/*.c tools/*.c desk/*.c aes/*.c vdi/*.c

.PHONY: checkindent
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

.PHONY: indent
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
# gitready
#

TOCLEAN += tounix$(EXE)

EXPAND_FILES = $(wildcard */*.[chS] */*.awk)
EXPAND_NOFILES = vdi/vdi_tblit_cf.S

.PHONY: expand
NODEP += expand
expand:
	@for i in `grep -l '	' $(filter-out $(EXPAND_NOFILES), $(EXPAND_FILES))` ; do \
		echo expanding $$i; \
		expand <$$i >expand.tmp; \
		mv expand.tmp $$i; \
	done

NODEP += tounix$(EXE)
tounix$(EXE): tools/tounix.c
	$(NATIVECC) -o $@ $<

# LVL - I checked that both on Linux and Cygwin passing more than 10000
# arguments on the command line works fine. On other systems it might be
# necessary to adopt another technique, for example using an find | xargs
# approach like that below:
#
# HERE = $(shell pwd)
# crlf:	tounix$(EXE)
#     find . -name .git -prune -or -not -name '*~' | xargs $(HERE)/tounix$(EXE)

.PHONY: crlf
NODEP += crlf
crlf: tounix$(EXE)
	./$< * bios/* bdos/* cli/* doc/* util/* tools/* po/* include/* aes/* desk/* vdi/*

# Check the sources charset (no automatic fix)
.PHONY: charset
NODEP += charset
charset:
	@echo "# All the files below should use charset=utf-8"
	find . -type f '!' -path '*/.git/*' '!' -path './obj/*' '!' -path './*.img' '!' -path './?rd*' '!' -path './bug*' '!' -path './mkrom*' '!' -name '*.def' '!' -name '*.rsc' '!' -name '*.icn' '!' -name '*.po' -print0 | xargs -0 file -i |grep -v us-ascii

.PHONY: gitready
NODEP += gitready
gitready: expand crlf

#
# local Makefile
#

ifneq (,$(wildcard local.mk))
include local.mk
endif

#
# clean and distclean
# (distclean is called before creating a source archive)
#

.PHONY: clean
NODEP += clean
clean:
	rm -f $(TOCLEAN)

.PHONY: distclean
NODEP += distclean
distclean: clean
	rm -f '.#'* */'.#'*

#
# ColdFire autoconverted sources.
# They are not generated automatically.
# To regenerate them, type "make coldfire-sources".
# You will need the PortAsm/68K for ColdFire tool from MicroAPL.
# See http://www.microapl.co.uk/Porting/ColdFire/pacf_download.html
#

PORTASM = pacf
PORTASMFLAGS = -blanks on -core v4 -hardware_divide -hardware_mac -a gnu -out_syntax standard -nowarning 402,502,900,1111,1150 -noerrfile

GENERATED_COLDFIRE_SOURCES = vdi/vdi_tblit_cf.S

.PHONY: coldfire-sources
NODEP += coldfire-sources
coldfire-sources:
	rm -f $(GENERATED_COLDFIRE_SOURCES)
	$(MAKE) COLDFIRE=1 $(GENERATED_COLDFIRE_SOURCES)

# Intermediate target (intermediate files are automatically removed)
TOCLEAN += vdi/*_preprocessed.*
vdi/%_preprocessed.s: vdi/%.S
	$(CPP) $(CFILE_FLAGS) $< -o $@

NODEP += $(GENERATED_COLDFIRE_SOURCES)
vdi/%_cf.S: vdi/%_preprocessed.s
	cd $(<D) && $(PORTASM) $(PORTASMFLAGS) -o $(@F) $(<F)
	sed -i $@ \
		-e "s:\.section\t.bss,.*:.bss:g" \
		-e "s:\( \|\t\)bsr\(  \|\..\):\1jbsr :g" \
		-e "s:\( \|\t\)bra\(  \|\..\):\1jbra :g" \
		-e "s:\( \|\t\)beq\(  \|\..\):\1jbeq :g" \
		-e "s:\( \|\t\)bne\(  \|\..\):\1jbne :g" \
		-e "s:\( \|\t\)bgt\(  \|\..\):\1jbgt :g" \
		-e "s:\( \|\t\)bge\(  \|\..\):\1jbge :g" \
		-e "s:\( \|\t\)blt\(  \|\..\):\1jblt :g" \
		-e "s:\( \|\t\)ble\(  \|\..\):\1jble :g" \
		-e "s:\( \|\t\)bcc\(  \|\..\):\1jbcc :g" \
		-e "s:\( \|\t\)bcs\(  \|\..\):\1jbcs :g" \
		-e "s:\( \|,\)0(%:\1(%:g"

#
# The targets for building a release are in a separate file
#

include release.mk

#
# file dependencies (makefile.dep)
#

.PHONY: depend
NODEP += depend
depend: makefile.dep

TOCLEAN += makefile.dep
NODEP += makefile.dep
makefile.dep: util/langs.c bios/header.h bios/ctables.h include/i18nconf.h
	( \
	  $(CC) $(MULTILIBFLAGS) $(TOOLCHAIN_CFLAGS) -MM $(INC) -Ibios -Iaes $(DEF) $(CSRC); \
	  $(CC) $(MULTILIBFLAGS) $(TOOLCHAIN_CFLAGS) -MM $(INC) $(DEF) $(SSRC) \
	) | sed -e '/:/s,^,obj/,' >makefile.dep

# Do not include or rebuild makefile.dep for the targets listed in NODEP
# as well as the default target (currently "help").
# Since NODEP is used inside an ifeq condition, it must be fully set before
# being used. Be sure to keep this block at the end of the Makefile.
ifneq (,$(MAKECMDGOALS))
ifeq (,$(filter $(NODEP), $(MAKECMDGOALS)))
-include makefile.dep
endif
endif
