#
# Makefile - the EmuTOS overbloated Makefile
#
# Copyright (C) 2001-2022 The EmuTOS development team.
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
# To modify the list of source code files, update the variables xxx_src below.


#
# General settings
#

MAKEFLAGS = --no-print-directory

#
# NODEP will accumulate the names of the targets which does not need to include
# makefile.dep, to avoid rebuilding that file when not necessary.
# This includes targets not using $(CC) and targets recursively invoking $(MAKE).
#

NODEP =

# "all" must be the *first* rule of this Makefile. Don't move!
# Variable assignments are allowed before this, but *not* rules.
# Note: The first rule is used when make is invoked without argument.
# So "make" is actually a synonym of "make all", actually "make help".
.PHONY: all
NODEP += all
all: help

.PHONY: help
NODEP += help
help: UNIQUE = $(COUNTRY)
help:
	@echo "target  meaning"
	@echo "------  -------"
	@echo "help    this help message"
	@echo "version display the EmuTOS version"
	@echo "192     etos192$(UNIQUE).img, EmuTOS ROM padded to size 192 KB"
	@echo "256     etos256$(UNIQUE).img, EmuTOS ROM padded to size 256 KB"
	@echo "512     etos512$(UNIQUE).img, EmuTOS ROM padded to size 512 KB"
	@echo "1024    etos1024$(UNIQUE).img, EmuTOS ROM padded to size 1024 KB"
	@echo "aranym  $(ROM_ARANYM), optimized for ARAnyM"
	@echo "firebee $(SREC_FIREBEE), to be flashed on the FireBee"
	@echo "firebee-prg emutos.prg, a RAM tos for the FireBee"
	@echo "amiga   $(ROM_AMIGA), EmuTOS ROM for Amiga hardware"
	@echo "amigavampire $(VAMPIRE_ROM_AMIGA), EmuTOS ROM for Amiga optimized for Vampire V2"
	@echo "v4sa    $(V4_ROM_AMIGA), EmuTOS ROM for Amiga Vampire V4 Standalone"
	@echo "amigakd $(AMIGA_KICKDISK), EmuTOS as Amiga 1000 Kickstart disk"
	@echo "amigaflop $(EMUTOS_ADF), EmuTOS RAM as Amiga boot floppy"
	@echo "amigaflopvampire $(EMUTOS_VAMPIRE_ADF), EmuTOS RAM as Amiga boot floppy optimized for Vampire V2"
	@echo "lisaflop $(EMUTOS_DC42), EmuTOS RAM as Apple Lisa boot floppy"
	@echo "m548x-dbug $(SREC_M548X_DBUG), EmuTOS-RAM for dBUG on ColdFire Evaluation Boards"
	@echo "m548x-bas  $(SREC_M548X_BAS), EmuTOS for BaS_gcc on ColdFire Evaluation Boards"
	@echo "m548x-prg  emutos.prg, a RAM tos for ColdFire Evaluation Boards with BaS_gcc"
	@echo "prg     emutos.prg, a RAM tos"
	@echo "prg256  $(EMU256_PRG), a RAM tos for ST/STe systems"
	@echo "flop    $(EMUTOS_ST), a bootable floppy with RAM tos"
	@echo "pak3    $(ROM_PAK3), suitable for PAK/3 systems"
	@echo "cart    $(ROM_CARTRIDGE), EmuTOS as a diagnostic cartridge"
	@echo "clean   remove temporary files"
	@echo "Use '$(MAKE) help-develop' for development-oriented targets"
	@echo "Use '$(MAKE) help-multi' for multi-image targets"

.PHONY: help-develop
NODEP += help-develop
help-develop:
	@echo "target        meaning"
	@echo "------        -------"
	@echo "help-develop  this help message"
	@echo "expand        expand tabs to spaces"
	@echo "crlf          convert all end of lines to LF"
	@echo "charset       check the charset of all the source files"
	@echo "bugready      set up files in preparation for 'bug update'"
	@echo "gitready      same as $(MAKE) expand crlf"
	@echo "dsm           dsm.txt, an edited disassembly of emutos.img"
	@echo "release       build the release archives into $(RELEASE_DIR)"
	@echo "release-clean remove the release archives"

.PHONY: help-multi
NODEP += help-multi
help-multi:
	@echo "target     meaning"
	@echo "------     -------"
	@echo "help-multi this help message"
	@echo "all192     all 192 KB images"
	@echo "all256     all 256 KB images"
	@echo "all512     all 512 KB images"
	@echo "allpak3    all PAK/3 images"
	@echo "allprg     all emutos*.prg"
	@echo "allprg256  all emu256*.prg"
	@echo "allflop    all emutos*.st"
	@echo "allfirebee all FireBee srec files"

#
# EmuTOS version
#

include version.mk

# Display the EmuTOS version
.PHONY: version
NODEP += version
version:
	@echo '$(VERSION)'

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
UNIQUEARG =
ifneq (,$(UNIQUE))
COUNTRY = $(UNIQUE)
UNIQUEARG = -u$(UNIQUE)
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
CORE = *.stackdump
DDOPTS = iflag=binary oflag=binary
else
# ordinary Unix stuff
CORE = core
DDOPTS =
endif

#
# test for localconf.h
#

ifneq (,$(wildcard localconf.h))
LOCALCONF = -DLOCALCONF
LOCALCONFINFO = \n\#\#\# NOTE: Configuration settings from localconf.h were used.\n
else
LOCALCONF =
LOCALCONFINFO =
endif

#
# GEN_SRC will accumulate the all the generated source files.
# Consequences for such files are:
# - they are automatically built before generating makefile.dep
# - they are automatically deleted on make clean
#

GEN_SRC =

#
# TOCLEAN will accumulate over the Makefile the names of files to remove
# when doing make clean; temporary Makefile files are *.tmp
#

TOCLEAN = *~ */*~ $(CORE) *.tmp obj/*.h $(GEN_SRC)

#
# Don't update makefile.dep when the user asks to generate source files
# from the command line. Does this really happen?
#

NODEP += %.c %.h %.pot

#
# compilation flags
#

# Override with 1 to use the ELF toolchain instead of the MiNT one
ELF = 0

ifeq (1,$(ELF))
# Standard ELF toolchain
TOOLCHAIN_PREFIX = m68k-elf-
TOOLCHAIN_CFLAGS = -fleading-underscore -Wa,--register-prefix-optional -fno-reorder-functions --param=min-pagesize=0 -DELF_TOOLCHAIN
else
# MiNT toolchain
TOOLCHAIN_PREFIX = m68k-atari-mint-
TOOLCHAIN_CFLAGS =
endif

# indent flags
INDENT = indent -kr

# Archiver
AR = $(TOOLCHAIN_PREFIX)ar
ARFLAGS = rc

# Linker with relocation information and binary output (image)
LD = $(CC) $(MULTILIBFLAGS) -nostartfiles -nostdlib
LIBS = -lgcc
LDFLAGS = -Wl,-T,obj/emutospp.ld
PCREL_LDFLAGS = -Wl,--oformat=binary,-Ttext=0,--entry=0

# C compiler
CC = $(TOOLCHAIN_PREFIX)gcc
CPP = $(CC) -E
CPUFLAGS = -m68000
MULTILIBFLAGS = $(CPUFLAGS) -mshort
INC = -Iinclude
OTHERFLAGS = -fomit-frame-pointer -fno-common

# Optimization flags (affects ROM size and execution speed)
STANDARD_OPTFLAGS = -O2
SMALL_OPTFLAGS = -Os
BUILD_TOOLS_OPTFLAGS = -O
OPTFLAGS = $(STANDARD_OPTFLAGS)

WARNFLAGS =
WARNFLAGS += -Wall
WARNFLAGS += -Werror=undef
WARNFLAGS += -Werror=missing-prototypes
WARNFLAGS += -Werror=strict-prototypes
WARNFLAGS += -Werror=implicit-function-declaration
WARNFLAGS += -Werror=format
WARNFLAGS += -Werror=redundant-decls
WARNFLAGS += -Werror=format-extra-args
#WARNFLAGS += -Werror=unused-function
#WARNFLAGS += -Wshadow
#WARNFLAGS += -Werror

GCCVERSION := $(shell $(CC) -dumpversion | cut -d. -f1)
# add warning flags not supported by GCC v2
ifneq (,$(GCCVERSION))
ifneq (2,$(GCCVERSION))
WARNFLAGS += -Werror=old-style-definition
WARNFLAGS += -Werror=type-limits
endif
endif

DEFINES = $(LOCALCONF) -DWITH_AES=$(WITH_AES) -DWITH_CLI=$(WITH_CLI) $(DEF)
CFLAGS_COMPILE = $(TOOLCHAIN_CFLAGS) $(OPTFLAGS) $(OTHERFLAGS) $(WARNFLAGS)
CFLAGS = $(MULTILIBFLAGS) $(CFLAGS_COMPILE) $(INC) $(DEFINES)

CPPFLAGS = $(CFLAGS)

ifeq (1,$(LTO))
# Link Time Optimization
LTOFLAGS = -flto=auto
CFLAGS += $(LTOFLAGS)
LDFLAGS += $(LTOFLAGS) $(CFLAGS_COMPILE)
AR = $(TOOLCHAIN_PREFIX)gcc-ar
endif

# The objdump utility (disassembler)
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump

# The objcopy utility
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy

# the native C compiler, for tools
NATIVECC = gcc -ansi -pedantic $(WARNFLAGS) -Wextra $(BUILD_TOOLS_OPTFLAGS)

#
# source code in bios/
#

# The source below must be the first to be linked
bios_src = startup.S

# These sources will be placed in ST-RAM by the linked script
bios_src += lowstram.c

# Other BIOS sources can be put in any order
bios_src +=  memory.S processor.S vectors.S aciavecs.S bios.c xbios.c acsi.c \
             biosmem.c blkdev.c chardev.c clock.c conout.c country.c \
             disk.c dma.c dmasound.c floppy.c font.c ide.c ikbd.c \
             kprint.c kprintasm.S linea.S lineainit.c lineavars.S machine.c \
             mfp.c midi.c mouse.c natfeat.S natfeats.c nvram.c panicasm.S \
             parport.c screen.c serport.c sound.c videl.c vt52.c xhdi.c \
             pmmu030.c 68040_pmmu.S \
             amiga.c amiga2.S spi_vamp.c \
             lisa.c lisa2.S \
             delay.c delayasm.S sd.c memory2.c bootparams.c scsi.c nova.c \
             dsp.c dsp2.S \
             scsidriv.c

ifeq (1,$(COLDFIRE))
  bios_src += coldfire.c coldfire2.S spi_cf.c
endif

#
# source code in bdos/
#

bdos_src = bdosmain.c console.c fsbuf.c fsdir.c fsdrive.c fsfat.c fsglob.c \
           fshand.c fsio.c fsmain.c fsopnclo.c iumem.c kpgmld.c osmem.c \
           proc.c rwa.S time.c umem.c initinfo.c bootstrap.c logo.c \
		   program_loader.c prg_program_loader.c pgz_program_loader.c

#
# source code in util/
#

util_src = cookie.c doprintf.c intmath.c langs.c memmove.S memset.S miscasm.S \
           nls.c nlsasm.S setjmp.S string.c lisautil.S miscutil.c

# The functions in the following modules are used by the AES and EmuDesk
ifeq ($(WITH_AES),1)
util_src += gemdos.c optimize.c optimopt.S rectfunc.c stringasm.S
endif

#
# source code in vdi/
#

vdi_src = vdi_asm.S vdi_bezier.c vdi_col.c vdi_control.c vdi_esc.c \
          vdi_fill.c vdi_gdp.c vdi_input.c vdi_line.c vdi_main.c \
          vdi_marker.c vdi_misc.c vdi_mouse.c vdi_raster.c vdi_text.c \
          vdi_textblit.c

ifeq (1,$(COLDFIRE))
vdi_src += vdi_tblit_cf.S
else
vdi_src += vdi_blit.S vdi_tblit.S
endif

# The source below must be the last VDI one
vdi_src += endvdi.S

#
# source code in aes/
#

aes_src = gemasm.S gemstart.S gemdosif.S gemaplib.c gemasync.c gemctrl.c \
          gemdisp.c gemevlib.c gemflag.c gemfmalt.c gemfmlib.c \
          gemfslib.c gemgraf.c gemgrlib.c gemgsxif.c geminit.c geminput.c \
          gemmnext.c gemmnlib.c gemobed.c gemobjop.c gemoblib.c gempd.c gemqueue.c \
          gemrslib.c gemsclib.c gemshlib.c gemsuper.c gemwmlib.c gemwrect.c \
          gsx2.c gem_rsc.c mforms.c aescfg.c shellutl.c

#
# source code in desk/
#

desk_src = deskstart.S deskmain.c gembind.c deskact.c deskapp.c deskdir.c \
           deskfpd.c deskfun.c deskglob.c deskinf.c deskins.c deskobj.c \
           deskpro.c deskrez.c deskrsrc.c desksupp.c deskwin.c \
           desk_rsc.c icons.c

#
# source code in cli/ for EmuTOS console EmuCON
#

cli_src = cmdasm.S cmdmain.c cmdedit.c cmdexec.c cmdint.c cmdparse.c cmdutil.c

#
# source code to put at the end of the ROM
#

end_src = bios/endrom.c

#
# Makefile functions
#

# Shell command to get the address of a symbol
FUNCTION_SHELL_GET_SYMBOL_ADDRESS = printf 0x%08x $$(awk '/^ *0x[^ ]* *$(1)( |$$)/{print $$1}' $(2))

# Function to get the address of a symbol into a Makefile variable
# $(1) = symbol name
# $(2) = map file name
MAKE_SYMADDR = $(shell $(call FUNCTION_SHELL_GET_SYMBOL_ADDRESS,$(1),$(2)))

# Function to get the address of a symbol into a shell variable
# This is useful to make an action in the first line of a recipe,
# then to get the result on the second line.
# Makefile variables in recipe can't be used for that, because they are
# evaluated before executing all recipe lines (but after building prerequisites)
# $(1) = symbol name
# $(2) = map file name
SHELL_SYMADDR = $$($(call FUNCTION_SHELL_GET_SYMBOL_ADDRESS,$(1),$(2)))

# VMA: Virtual Memory Address
# This is the address where the ROM is mapped at run time.
VMA = $(call MAKE_SYMADDR,__text,emutos.map)

# LMA: Load Memory Address
# This is the physical address where the ROM is stored.
# On some machines (i.e. FireBee), the ROM is stored at some address (LMA),
# then mapped to another address (VMA) at run time.
# On most machines, LMA and VMA are equal.
# LMA is important on systems where the ROM can be dynamically updated.
LMA = $(VMA)

# Entry point address to boot the ROM.
# This is used when the pseudo-ROM is loaded into RAM by some kind of debugger,
# i.e. dBUG on ColdFire Evaluation Boards.
# Specifying the entry point is generally useless, as such debugger uses
# the start of the ROM as default entry point, and TOS-like ROMs already have
# a branch to _main there.
ENTRY = $(call MAKE_SYMADDR,_main,emutos.map)

# The following reference values have been gathered from major TOS versions
MEMBOT_TOS100 = 0x0000a100
MEMBOT_TOS102 = 0x0000ca00
MEMBOT_TOS104 = 0x0000a84e
MEMBOT_TOS162 = 0x0000a892
MEMBOT_TOS206 = 0x0000ccb2
MEMBOT_TOS306 = 0x0000e6fc
MEMBOT_TOS404 = 0x0000f99c

# If set, compare performance to one of the TOS versions above.
REF_OS=

#
# Directory selection depending on the features
#

# Core directories are essential for basic OS operation
core_dirs = bios bdos util

# Optional directories may be disabled for reduced features
optional_dirs = vdi

ifeq ($(WITH_AES),1)
 optional_dirs += aes desk
endif

ifeq ($(WITH_CLI),1)
 optional_dirs += cli
endif

dirs = $(core_dirs) $(optional_dirs)

vpath %.c $(dirs)
vpath %.S $(dirs)

#
# country-specific settings
#

include country.mk

#
# everything should work fine below.
#

SRC = $(foreach d,$(dirs),$(addprefix $(d)/,$($(d)_src))) $(end_src)

CORE_OBJ = $(foreach d,$(core_dirs),$(patsubst %.c,obj/%.o,$(patsubst %.S,obj/%.o,$($(d)_src)))) $(FONTOBJ_COMMON) obj/libfont.a obj/version.o
OPTIONAL_OBJ = $(foreach d,$(optional_dirs),$(patsubst %.c,obj/%.o,$(patsubst %.S,obj/%.o,$($(d)_src))))
END_OBJ = $(patsubst %,obj/%.o,$(basename $(notdir $(end_src))))
OBJECTS = $(CORE_OBJ) $(OPTIONAL_OBJ) $(END_OBJ)

#
# Preprocess the linker script, to allow #include, #define, #if, etc.
#

TOCLEAN += obj/*.ld

obj/emutospp.ld: emutos.ld include/config.h tosvars.ld
	$(CPP) $(CPPFLAGS) -P -x c $< -o $@

#
# the maps must be built at the same time as the images, to enable
# one generic target to deal with all edited disassembly.
#

TOCLEAN += *.img *.map

emutos.img: $(OBJECTS) obj/emutospp.ld
	$(LD) $(CORE_OBJ) $(LIBS) $(OPTIONAL_OBJ) $(LIBS) $(END_OBJ) $(LDFLAGS) -Wl,-Map=emutos.map -o emutos.img
	@if [ $$(($$(awk '/^\.data /{print $$3}' emutos.map))) -gt 0 ]; then \
	  echo "### Warning: The DATA segment is not empty."; \
	  echo "### Please examine emutos.map and use \"const\" where appropriate."; \
	fi
	@echo "# TEXT=$(call SHELL_SYMADDR,__text,emutos.map)"\
" STKBOT=$(call SHELL_SYMADDR,_stkbot,emutos.map)"\
" LOWSTRAM=$(call SHELL_SYMADDR,__low_stram_start,emutos.map)"\
" BSS=$(call SHELL_SYMADDR,__bss,emutos.map)"\
" MEMBOT=$(call SHELL_SYMADDR,__end_os_stram,emutos.map)"
ifneq (,$(REF_OS))
	@MEMBOT=$(call SHELL_SYMADDR,__end_os_stram,emutos.map);\
	echo "# RAM used: $$(($$MEMBOT)) bytes ($$(($$MEMBOT - $(MEMBOT_$(REF_OS)))) bytes more than $(REF_OS))"
else
	@MEMBOT=$(call SHELL_SYMADDR,__end_os_stram,emutos.map);\
	echo "# RAM used: $$(($$MEMBOT)) bytes"
endif

#
# Padded Image
#

ROM_PADDED = $(if $(UNIQUE),etos$(ROMSIZE)$(UNIQUE).img,etos$(ROMSIZE)k.img)

$(ROM_PADDED): emutos.img mkrom
	./mkrom pad $(ROMSIZE)k $< $@

#
# 192kB Image
#

.PHONY: 192
NODEP += 192
192: UNIQUE = $(COUNTRY)
192: OPTFLAGS = $(SMALL_OPTFLAGS)
192: override DEF += -DTARGET_192
192: WITH_CLI = 0
192: ROMSIZE = 192
192:
	$(MAKE) DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' WITH_CLI=$(WITH_CLI) UNIQUE=$(UNIQUE) ROMSIZE=$(ROMSIZE) ROM_PADDED=$(ROM_PADDED) $(ROM_PADDED) REF_OS=TOS104
	@printf "$(LOCALCONFINFO)"

#
# 256kB Image
#

.PHONY: 256
NODEP += 256
256: UNIQUE = $(COUNTRY)
256: OPTFLAGS = $(SMALL_OPTFLAGS)
256: override DEF += -DTARGET_256
256: ROMSIZE = 256
256:
	$(MAKE) DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' UNIQUE=$(UNIQUE) ROMSIZE=$(ROMSIZE) ROM_PADDED=$(ROM_PADDED) $(ROM_PADDED) REF_OS=TOS206
	@printf "$(LOCALCONFINFO)"

#
# 512kB Image (for TT or Falcon; also usable for ST/STe under Hatari)
#

.PHONY: 512
NODEP += 512
512: UNIQUE = $(COUNTRY)
512: override DEF += -DTARGET_512
512: ROMSIZE = 512
512:
	$(MAKE) DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' UNIQUE=$(UNIQUE) MULTIKEYBD='-k' ROMSIZE=$(ROMSIZE) ROM_PADDED=$(ROM_PADDED) $(ROM_PADDED) REF_OS=TOS404
	@printf "$(LOCALCONFINFO)"

#
# 512kB PAK/3 Image (based on 256kB Image)
#

ROM_PAK3 = etospak3$(UNIQUE).img

.PHONY: pak3
NODEP += pak3
pak3: UNIQUE = $(COUNTRY)
pak3: OPTFLAGS = $(SMALL_OPTFLAGS)
pak3: override DEF += -DTARGET_256
pak3:
	$(MAKE) DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' UNIQUE=$(UNIQUE) ROM_PAK3=$(ROM_PAK3) $(ROM_PAK3) REF_OS=TOS206
	@printf "$(LOCALCONFINFO)"

$(ROM_PAK3): ROMSIZE = 256
$(ROM_PAK3): emutos.img mkrom
	./mkrom pak3 $< $(ROM_PAK3)

#
# 1024kB Image (for Hatari (and potentially other emulators))
#

.PHONY: 1024
NODEP += 1024
1024: override DEF += -DTARGET_1024
1024: ROMSIZE = 1024
1024: SYMFILE = $(addsuffix .sym,$(basename $(ROM_PADDED)))
1024:
	$(MAKE) DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' UNIQUE=$(UNIQUE) MULTIKEYBD='-k' ROMSIZE=$(ROMSIZE) ROM_PADDED=$(ROM_PADDED) $(ROM_PADDED) REF_OS=TOS404 SYMFILE=$(SYMFILE) $(SYMFILE)
	@printf "$(LOCALCONFINFO)"

#
# ARAnyM Image
#

ROM_ARANYM = emutos-aranym.img

.PHONY: aranym
NODEP += aranym
aranym: override DEF += -DMACHINE_ARANYM
aranym: OPTFLAGS = $(SMALL_OPTFLAGS)
aranym: CPUFLAGS = -m68040
aranym: ROMSIZE = 512
aranym: ROM_PADDED = $(ROM_ARANYM)
aranym:
	@echo "# Building ARAnyM EmuTOS into $(ROM_PADDED)"
	$(MAKE) CPUFLAGS='$(CPUFLAGS)' OPTFLAGS='$(OPTFLAGS)' DEF='$(DEF)' ROMSIZE=$(ROMSIZE) ROM_PADDED=$(ROM_PADDED) $(ROM_PADDED) REF_OS=TOS404
	@printf "$(LOCALCONFINFO)"

#
# Diagnostic Cartridge Image
#

TOCLEAN += *.stc
ROM_CARTRIDGE = etoscart.img

.PHONY: cart
NODEP += cart
cart: OPTFLAGS = $(SMALL_OPTFLAGS)
cart: override DEF += -DTARGET_CART
cart: WITH_AES = 0
cart: ROMSIZE = 128
cart: ROM_PADDED = $(ROM_CARTRIDGE)
cart:
	@echo "# Building Diagnostic Cartridge EmuTOS into $(ROM_PADDED)"
	$(MAKE) OPTFLAGS='$(OPTFLAGS)' DEF='$(DEF)' UNIQUE=$(COUNTRY) WITH_AES=$(WITH_AES) ROMSIZE=$(ROMSIZE) ROM_PADDED=$(ROM_PADDED) $(ROM_PADDED) REF_OS=TOS104
	./mkrom stc emutos.img emutos.stc
	@printf "$(LOCALCONFINFO)"

#
# Amiga Image
#

TOCLEAN += *.rom

ROM_AMIGA = emutos-amiga.rom
AMIGA_DEFS =

.PHONY: amiga
NODEP += amiga
amiga: UNIQUE = $(COUNTRY)
amiga: OPTFLAGS = $(SMALL_OPTFLAGS)
amiga: override DEF += -DTARGET_AMIGA_ROM $(AMIGA_DEFS)
amiga:
	@echo "# Building Amiga EmuTOS into $(ROM_AMIGA)"
	$(MAKE) CPUFLAGS='$(CPUFLAGS)' DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' UNIQUE=$(UNIQUE) ROM_AMIGA=$(ROM_AMIGA) $(ROM_AMIGA) REF_OS=TOS206
	@printf "$(LOCALCONFINFO)"

$(ROM_AMIGA): emutos.img mkrom
	./mkrom amiga $< $(ROM_AMIGA)

# Special Amiga ROM optimized for Vampire V2

VAMPIRE_CPUFLAGS = -m68040
VAMPIRE_COMMON_DEF = -DCONF_WITH_VAMPIRE_SPI=1 -DCONF_WITH_SDMMC=1
VAMPIRE_DEF = -DSTATIC_ALT_RAM_ADDRESS=0x08000000 -DSTATIC_ALT_RAM_SIZE=126UL*1024*1024
VAMPIRE_ROM_AMIGA = emutos-vampire.rom

.PHONY: amigavampire
NODEP += amigavampire
amigavampire: CPUFLAGS = $(VAMPIRE_CPUFLAGS)
amigavampire: override DEF += $(VAMPIRE_COMMON_DEF) $(VAMPIRE_DEF)
amigavampire: ROM_AMIGA = $(VAMPIRE_ROM_AMIGA)
amigavampire: amiga

# Special Amiga ROM optimized for Vampire V4 Standalone
V4_DEF = -DSTATIC_ALT_RAM_ADDRESS=0x01000000 -DSTATIC_ALT_RAM_SIZE=496UL*1024*1024
V4_ROM_AMIGA = emutos-vampire-v4sa.rom

.PHONY: v4sa
NODEP += v4sa
v4sa: CPUFLAGS = $(VAMPIRE_CPUFLAGS)
v4sa: override DEF += $(VAMPIRE_COMMON_DEF) $(V4_DEF)
v4sa: ROM_AMIGA = $(V4_ROM_AMIGA)
v4sa: amiga

#
# Amiga Kickstart disk image for Amiga 1000
#

TOCLEAN += *.adf

AMIGA_KICKDISK = emutos-kickdisk.adf

.PHONY: amigakd
NODEP += amigakd
amigakd: amiga
	./mkrom amiga-kickdisk $(ROM_AMIGA) $(AMIGA_KICKDISK)

#
# ColdFire images
#

TOCLEAN += *.s19
SRECFILE = emutos.s19

# Length of an S-Record data field on a single line, in bytes (optional).
# Increasing the default value may speed up the transfer,
# specially through a slow serial port when data is displayed on the screen.
SREC_LEN =
SREC_LEN_OPTION = $(if $(SREC_LEN),--srec-len=$(SREC_LEN))

$(SRECFILE): emutos.img
	$(OBJCOPY) -I binary -O srec $(SREC_LEN_OPTION) --change-addresses $(LMA) --change-start $(ENTRY) $< $(SRECFILE)

CPUFLAGS_FIREBEE = -mcpu=5474
SREC_FIREBEE = etosfb$(UNIQUE).s19

.PHONY: firebee
NODEP += firebee
firebee: UNIQUE = $(COUNTRY)
firebee: OPTFLAGS = $(STANDARD_OPTFLAGS)
firebee: override DEF += -DMACHINE_FIREBEE
firebee: CPUFLAGS = $(CPUFLAGS_FIREBEE)
firebee:
	@echo "# Building FireBee EmuTOS into $(SREC_FIREBEE)"
	$(MAKE) COLDFIRE=1 CPUFLAGS='$(CPUFLAGS)' DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' UNIQUE=$(UNIQUE) LMA=0xe0600000 SRECFILE=$(SREC_FIREBEE) $(SREC_FIREBEE) REF_OS=TOS404
	@printf "$(LOCALCONFINFO)"

.PHONY: firebee-prg
NODEP += firebee-prg
firebee-prg: OPTFLAGS = $(STANDARD_OPTFLAGS)
firebee-prg: override DEF += -DMACHINE_FIREBEE
firebee-prg: CPUFLAGS = $(CPUFLAGS_FIREBEE)
firebee-prg:
	@echo "# Building FireBee $(EMUTOS_PRG)"
	$(MAKE) COLDFIRE=1 CPUFLAGS='$(CPUFLAGS)' DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' prg

CPUFLAGS_M548X = -mcpu=5475

.PHONY: m548x-prg
NODEP += m548x-prg
m548x-prg: OPTFLAGS = $(STANDARD_OPTFLAGS)
m548x-prg: override DEF += -DMACHINE_M548X -DCONF_WITH_BAS_MEMORY_MAP=1
m548x-prg: CPUFLAGS = $(CPUFLAGS_M548X)
m548x-prg:
	@echo "# Building m548x $(EMUTOS_PRG)"
	$(MAKE) COLDFIRE=1 CPUFLAGS='$(CPUFLAGS)' DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' prg

SREC_M548X_DBUG = emutos-m548x-dbug.s19
.PHONY: m548x-dbug
NODEP += m548x-dbug
m548x-dbug: UNIQUE = $(COUNTRY)
m548x-dbug: override DEF += -DMACHINE_M548X
m548x-dbug: CPUFLAGS = $(CPUFLAGS_M548X)
m548x-dbug:
	@echo "# Building M548x dBUG EmuTOS in $(SREC_M548X_DBUG)"
	$(MAKE) COLDFIRE=1 CPUFLAGS='$(CPUFLAGS)' DEF='$(DEF)' UNIQUE=$(UNIQUE) SRECFILE=$(SREC_M548X_DBUG) $(SREC_M548X_DBUG) REF_OS=TOS404
	@printf "$(LOCALCONFINFO)"

SREC_M548X_BAS = emutos-m548x-bas.s19
.PHONY: m548x-bas
NODEP += m548x-bas
m548x-bas: UNIQUE = $(COUNTRY)
m548x-bas: override DEF += -DMACHINE_M548X -DCONF_WITH_BAS_MEMORY_MAP=1
m548x-bas: CPUFLAGS = $(CPUFLAGS_M548X)
m548x-bas:
	@echo "# Building M548x BaS_gcc EmuTOS in $(SREC_M548X_BAS)"
	$(MAKE) COLDFIRE=1 CPUFLAGS='$(CPUFLAGS)' DEF='$(DEF)' UNIQUE=$(UNIQUE) LMA=0xe0100000 SRECFILE=$(SREC_M548X_BAS) $(SREC_M548X_BAS) REF_OS=TOS404
	@printf "$(LOCALCONFINFO)"

#
# Special variants of EmuTOS running in RAM instead of ROM.
# In this case, emutos.img needs to be loaded into RAM by some loader.
#

obj/ramtos.h: emutos.img
	@echo '# Generating $@'
	@printf \
'/* Generated from emutos.map */\n'\
'#define ADR_TEXT $(call MAKE_SYMADDR,__text,emutos.map)\n'\
'#define ADR_ALTRAM_REGIONS $(call MAKE_SYMADDR,_altram_regions,emutos.map)\n'\
>$@

#
# emutos.prg
#

EMUTOS_PRG = emutos$(UNIQUE).prg
TOCLEAN += emutos*.prg

.PHONY: prg
prg: $(EMUTOS_PRG)
	@printf "$(LOCALCONFINFO)"

obj/boot.o: obj/ramtos.h
# incbin dependencies are not automatically detected
obj/ramtos.o: emutos.img

$(EMUTOS_PRG): override DEF += -DTARGET_PRG
$(EMUTOS_PRG): OPTFLAGS = $(SMALL_OPTFLAGS)
$(EMUTOS_PRG): obj/minicrt.o obj/boot.o obj/bootram.o obj/ramtos.o
	$(LD) $+ -lgcc -o $@ -s

#
# emu256.prg
#

EMU256_PRG = emu256$(UNIQUE).prg
TOCLEAN += emu256*.prg

.PHONY: prg256
NODEP += prg256
prg256: override DEF += -DTARGET_256
prg256: UNIQUE = $(COUNTRY)
prg256: EMUTOS_PRG = $(EMU256_PRG)
prg256:
	@echo "# Building $(EMUTOS_PRG)"
	$(MAKE) DEF='$(DEF)' UNIQUE=$(UNIQUE) EMUTOS_PRG=$(EMUTOS_PRG) prg

obj/boot.o: obj/ramtos.h
# incbin dependencies are not automatically detected
obj/ramtos.o: emutos.img

#
# flop
#

EMUTOS_ST = emutos$(UNIQUE).st
TOCLEAN += emutos*.st mkflop

.PHONY: flop
NODEP += flop
flop: UNIQUE = $(COUNTRY)
flop:
	$(MAKE) UNIQUE=$(UNIQUE) $(EMUTOS_ST)
	@printf "$(LOCALCONFINFO)"

$(EMUTOS_ST): override DEF += -DTARGET_FLOPPY
$(EMUTOS_ST): OPTFLAGS = $(SMALL_OPTFLAGS)
$(EMUTOS_ST): mkflop bootsect.img emutos.img
	./mkflop bootsect.img emutos.img $@

bootsect.img : obj/bootsect.o obj/bootram.o
	$(LD) $+ $(PCREL_LDFLAGS) -o $@

obj/bootsect.o: obj/ramtos.h

NODEP += mkflop
mkflop : tools/mkflop.c
	$(NATIVECC) $< -o $@

#
# amigaflop
#

EMUTOS_ADF = emutos.adf

.PHONY: amigaflop
NODEP += amigaflop
amigaflop: UNIQUE = $(COUNTRY)
amigaflop: OPTFLAGS = $(SMALL_OPTFLAGS)
amigaflop: override DEF += -DTARGET_AMIGA_FLOPPY $(AMIGA_DEFS)
amigaflop:
	$(MAKE) CPUFLAGS='$(CPUFLAGS)' DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' UNIQUE=$(UNIQUE) EMUTOS_ADF=$(EMUTOS_ADF) $(EMUTOS_ADF)
	@printf "$(LOCALCONFINFO)"

EMUTOS_VAMPIRE_ADF = emutos-vampire.adf

.PHONY: amigaflopvampire
NODEP += amigaflopvampire
amigaflopvampire: override DEF += $(VAMPIRE_COMMON_DEF) -DSTATIC_ALT_RAM_ADDRESS=0x08000000 $(AMIGA_DEFS)
amigaflopvampire: CPUFLAGS = $(VAMPIRE_CPUFLAGS)
amigaflopvampire: EMUTOS_ADF = $(EMUTOS_VAMPIRE_ADF)
amigaflopvampire: amigaflop

# Convenient target to test amigaflopvampire on WinUAE
.PHONY: amigaflopwinuae
NODEP += amigaflopwinuae
amigaflopwinuae: override DEF += -DSTATIC_ALT_RAM_ADDRESS=0x40000000 $(AMIGA_DEFS)
amigaflopwinuae: CPUFLAGS = $(VAMPIRE_CPUFLAGS)
amigaflopwinuae: amigaflop

$(EMUTOS_ADF): amigaboot.img emutos.img mkrom
	./mkrom amiga-floppy amigaboot.img emutos.img $@

amigaboot.img: obj/amigaboot.o obj/bootram.o
	$(LD) $+ $(PCREL_LDFLAGS) -o $@

obj/amigaboot.o: obj/ramtos.h

#
# lisaflop
#

TOCLEAN += *.dc42

EMUTOS_DC42 = emutos.dc42
LISA_DEFS =

.PHONY: lisaflop
NODEP += lisaflop
lisaflop: UNIQUE = $(COUNTRY)
lisaflop: OPTFLAGS = $(SMALL_OPTFLAGS)
lisaflop: override DEF += -DTARGET_LISA_FLOPPY $(LISA_DEFS)
lisaflop:
	$(MAKE) CPUFLAGS='$(CPUFLAGS)' DEF='$(DEF)' OPTFLAGS='$(OPTFLAGS)' UNIQUE=$(UNIQUE) EMUTOS_DC42=$(EMUTOS_DC42) $(EMUTOS_DC42)
	@printf "$(LOCALCONFINFO)"

$(EMUTOS_DC42): lisaboot.img emutos.img mkrom
	./mkrom lisa-boot-floppy lisaboot.img emutos.img $@

lisaboot.img: obj/lisaboot.o obj/lisautil.o obj/bootram.o
	$(LD) $+ $(PCREL_LDFLAGS) -o $@

obj/lisaboot.o: obj/ramtos.h

#
# localisation support: create bios/ctables.h include/i18nconf.h
#
# we would like to group targets, since localise generates both targets
# at the same time.  however, this is not supported until gcc make 4.2.
# so that we can use old versions of make, we just run localise twice
# instead, creating a .tmp file each time as a byproduct.
#

GEN_SRC += bios/ctables.h include/i18nconf.h
TOCLEAN += localise include/i18nconf.h bios/ctables.h

NODEP += localise
localise: tools/localise.c
	$(NATIVECC) $< -o $@

bios/ctables.h: obj/country localise.ctl localise
	./localise $(UNIQUEARG) $(MULTIKEYBD) localise.ctl bios/ctables.h localise1.tmp

include/i18nconf.h: obj/country localise.ctl localise
	./localise $(UNIQUEARG) $(MULTIKEYBD) localise.ctl localise2.tmp include/i18nconf.h

#
# NLS support
#

POFILES = $(wildcard po/*.po)

GEN_SRC += util/langs.c
TOCLEAN += bug po/messages.pot

NODEP += bug
bug: tools/bug.c
	$(NATIVECC) $< -o $@

# even if UNIQUE is specified, we ensure langs.c exists to avoid dependency problems
util/langs.c: $(POFILES) po/LINGUAS bug po/messages.pot
	./bug make

po/messages.pot: bug po/POTFILES.in $(shell grep -v '^#' po/POTFILES.in)
	./bug xgettext

#
# Resource support
#

TOCLEAN += erd grd ird mrd draft temp.rsc temp.def

NODEP += erd grd ird mrd draft
erd: tools/erd.c
	$(NATIVECC) $< -o $@
grd: tools/erd.c
	$(NATIVECC) -DGEM_RSC $< -o grd
ird: tools/erd.c
	$(NATIVECC) -DICON_RSC $< -o ird
mrd: tools/erd.c
	$(NATIVECC) -DMFORM_RSC $< -o mrd
draft: tools/draft.c tools/draftexc.c
	$(NATIVECC) $(DEFINES) $^ -o $@

DESKRSC_BASE = desk/desktop
DESKRSCGEN_BASE = desk/desk_rsc
GEMRSC_BASE = aes/gem
GEMRSCGEN_BASE = aes/gem_rsc
ICONRSC_BASE = desk/icon
ICONRSCGEN_BASE = desk/icons
MFORMRSC_BASE = aes/mform
MFORMRSCGEN_BASE = aes/mforms
GEN_SRC += $(DESKRSCGEN_BASE).c $(DESKRSCGEN_BASE).h $(GEMRSCGEN_BASE).c $(GEMRSCGEN_BASE).h
GEN_SRC += $(ICONRSCGEN_BASE).c $(ICONRSCGEN_BASE).h $(MFORMRSCGEN_BASE).c $(MFORMRSCGEN_BASE).h

# Hack below! '%' is used instead of '.' to support tools with multiple outputs.
# For example, the erd tool produces 2 files: a .c and a .h
# Without special care, when using parallel execution (make -j), the recipe
# may run twice (or more) in parallel. This is useless and could be harmful.
# Problem is that _standard rules_ with several targets just specify that the
# same recipe must be applied for all targets. This *doesn't* mean that recipe
# will generate all the targets at the same time.
# This issue is hidden when parallel execution is not used.
# On the other hand, _pattern rules_ (the ones with %) explicitly specify
# that all the targets will be built at the same time by the recipe.
# In order to work, all the targets must share a common substring replaced
# by the % wildcard. A simple trick is to use the '%' wilcard to replace the '.'
# character. This is enough to match actual filenames and enable the special
# behaviour of pattern rules.
# Trick source: https://stackoverflow.com/a/3077254
# For official documentation of pattern rules, see the bison example there:
# https://www.gnu.org/software/make/manual/html_node/Pattern-Examples.html
# Note: GNU Make 4.3 provides a cleaner solution with "grouped explicit targets"
# using the &: syntax in rules. But this is beyond our make prerequisites.
#
# Note: we also use '%' in the prerequisites to avoid matching '%.tr.c'
# https://www.gnu.org/software/make/manual/html_node/Pattern-Match.html
# "A pattern rule can be used to build a given file only if there is a target
# pattern that matches the file name, and all prerequisites in that rule either
# exist or can be built."

$(DESKRSCGEN_BASE)%c $(DESKRSCGEN_BASE)%h: draft erd $(DESKRSC_BASE)%rsc $(DESKRSC_BASE)%def
	./draft $(DESKRSC_BASE) temp
	./erd -pdesk temp $(DESKRSCGEN_BASE)
$(GEMRSCGEN_BASE)%c $(GEMRSCGEN_BASE)%h: grd $(GEMRSC_BASE)%rsc $(GEMRSC_BASE)%def
	./grd $(GEMRSC_BASE) $(GEMRSCGEN_BASE)
$(ICONRSCGEN_BASE)%c $(ICONRSCGEN_BASE)%h: ird $(ICONRSC_BASE)%rsc $(ICONRSC_BASE)%def
	./ird -picon $(ICONRSC_BASE) $(ICONRSCGEN_BASE)
$(MFORMRSCGEN_BASE)%c $(MFORMRSCGEN_BASE)%h: mrd $(MFORMRSC_BASE)%rsc $(MFORMRSC_BASE)%def
	./mrd -pmform $(MFORMRSC_BASE) $(MFORMRSCGEN_BASE)

#
# Logo support
#

TOCLEAN += logo_compressor
NODEP += logo_compressor
LOGO_BASE = bdos/logo
GEN_SRC += $(LOGO_BASE).c $(LOGO_BASE).h
logo_compressor: tools/logo_compressor.c
	$(NATIVECC) $< -o $@
$(LOGO_BASE)%c $(LOGO_BASE)%h: logo_compressor
	./logo_compressor $(LOGO_BASE).c $(LOGO_BASE).h

#
# Special ROM support
#

TOCLEAN += mkrom

NODEP += mkrom
mkrom: tools/mkrom.c
	$(NATIVECC) $< -o $@

# test target to build all tools that can be built by the Makefile
.PHONY: tools
NODEP += tools
tools: bug draft erd grd ird localise mkflop mkrom mrd boot-delay tos-lang-change

# user tools, not needed in EmuTOS building
TOCLEAN += tos-lang-change boot-delay
NODEP += tos-lang-change boot-delay
tos-lang-change: tools/tos-lang-change.c
	$(NATIVECC) $< -o $@

boot-delay: tools/boot-delay.c
	$(NATIVECC) $< -o $@

# The sleep command in targets below ensure that all the generated sources
# will have a timestamp older than any object file.
# This matters on filesystems having low timestamp resolution (ext2, ext3).

.PHONY: allpak3
NODEP += allpak3
allpak3:
	@for i in $(COUNTRIES); \
	do \
	  echo; \
	  echo "sleep 1"; \
	  sleep 1; \
	  $(MAKE) pak3 UNIQUE=$$i || exit 1; \
	done

.PHONY: all512
NODEP += all512
all512:
	@for i in $(COUNTRIES); \
	do \
	  echo; \
	  echo "sleep 1"; \
	  sleep 1; \
	  $(MAKE) 512 UNIQUE=$$i || exit 1; \
	done

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

.PHONY: allprg
NODEP += allprg
allprg:
	$(MAKE) prg
	@for i in $(COUNTRIES); \
	do \
	  echo; \
	  echo "sleep 1"; \
	  sleep 1; \
	  $(MAKE) prg UNIQUE=$$i || exit 1; \
	done

.PHONY: allprg256
NODEP += allprg256
allprg256:
	@for i in $(COUNTRIES); \
	do \
	  echo; \
	  echo "sleep 1"; \
	  sleep 1; \
	  $(MAKE) prg256 UNIQUE=$$i || exit 1; \
	done

.PHONY: allflop
NODEP += allflop
allflop:
	@for i in $(COUNTRIES); \
	do \
	  echo; \
	  echo "sleep 1"; \
	  sleep 1; \
	  $(MAKE) flop UNIQUE=$$i || exit 1; \
	done

.PHONY: allfirebee
NODEP += allfirebee
allfirebee:
	@for i in $(COUNTRIES); \
	do \
	  echo; \
	  echo "sleep 1"; \
	  sleep 1; \
	  $(MAKE) firebee UNIQUE=$$i || exit 1; \
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
emutos.img: $(TRANS_SRC)

%.tr.c : %.c po/$(ETOSLANG).po bug po/LINGUAS obj/country
	./bug translate $(ETOSLANG) $<
endif
endif

#
# obj/country contains the current values of $(COUNTRY) and $(UNIQUE).
# whenever it changes, whatever necessary steps are taken so that the
# correct files get re-compiled.
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
	  j=obj/$$(basename $$i tr.c)o; \
	  echo "rm -f $$i $$j"; \
	  rm -f $$i $$j; \
	done

#
# OS header
#

GEN_SRC += obj/header.h

obj/header.h: tools/mkheader.awk obj/country version.mk
	awk -f tools/mkheader.awk $(COUNTRY) $(MAJOR_VERSION) $(MINOR_VERSION) $(FIX_VERSION) $(UNOFFICIAL) > $@

#
# build rules
#

TOCLEAN += obj/*.o

CFILE_FLAGS = $(strip $(CFLAGS))
SFILE_FLAGS = $(strip $(CFLAGS))

ifeq (1,$(LTO))
# Files in the NOLTO list below will not be compiled using LTO.
# This is necessary to allow the linker script to put the resulting .o files
# into specific sections, as LTO changes the object file name.
NOLTO = bios/lowstram.c
CFILE_FLAGS = $(if $(filter $(NOLTO),$<),$(filter-out $(LTOFLAGS),$(CFLAGS)),$(CFLAGS))
# Compiling assembler sources with LTO is useless.
SFILE_FLAGS = $(filter-out $(LTOFLAGS),$(CFLAGS))
endif

obj/%.o : %.tr.c
	$(CC) $(CFILE_FLAGS) -c $< -o $@

obj/%.o : %.c
	$(CC) $(CFILE_FLAGS) -c $< -o $@

obj/%.o : %.S
	$(CC) $(SFILE_FLAGS) -c $< -o $@

#
# version string
#

GEN_SRC += obj/version.c

# This temporary file is always generated
obj/version2.c:
	@echo '/* Generated from Makefile */' > $@
	@echo '#define MAJOR_VERSION $(MAJOR_VERSION)' >> $@
	@echo '#define MINOR_VERSION $(MINOR_VERSION)' >> $@
	@echo '#define FIX_VERSION $(FIX_VERSION)' >> $@
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
# Disassembly
#

DSM_OUTPUT = dsm.txt
DSM_TMP_CODE = obj/dsm.tmp
DSM_TMP_LABELS = obj/map.tmp
TOCLEAN += $(DSM_OUTPUT)

.PHONY: check_target_exists
check_target_exists:
	@test -f emutos.img -a -f emutos.map \
	  || (echo "Please make a target before disassembling." >&2 ; false)

.PHONY: dsm
NODEP += dsm
dsm: check_target_exists
	$(OBJDUMP) --target=binary --architecture=m68k --adjust-vma=$(VMA) -D emutos.img \
	  | sed -e '/^ *[0-9a-f]*:/!d;s/^    /0000/;s/^   /000/;s/^  /00/;s/^ /0/;s/:	/: /' > $(DSM_TMP_CODE)
	sed -e '/^ *0x/!d;s///;s/  */:  /;s/^00000000//;/^00000001:  ASSERT /d;/ \. = /d;s/ = .*//' emutos.map > $(DSM_TMP_LABELS)
	cat $(DSM_TMP_CODE) $(DSM_TMP_LABELS) | LC_ALL=C sort > $(DSM_OUTPUT)
	rm $(DSM_TMP_CODE) $(DSM_TMP_LABELS)
	@echo "# $(DSM_OUTPUT) contains the disassembly of emutos.img"

#
# Hatari symbols file
#

# By default, no symbols file is generated.
SYMFILE =
TOCLEAN += *.sym

%.sym: emutos.img tools/map2sym.sh
	$(SHELL) tools/map2sym.sh emutos.map >$@

#
# checkindent - check for indent warnings, but do not alter files
#

INDENTFILES = bdos/*.c bios/*.c util/*.c tools/*.c desk/*.c aes/*.c vdi/*.c

.PHONY: checkindent
checkindent:
	@err=0 ; \
	for i in $(INDENTFILES) ; do \
		$(INDENT) <$$i 2>err.tmp >/dev/null; \
		if test -s err.tmp ; then \
			err=$$(expr $$err + 1); \
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


#
# gitready
#

EXPAND_FILES = $(wildcard */*.[chS] */*.awk */*.sh)
EXPAND_NOFILES = vdi/vdi_tblit_cf.S

.PHONY: expand
NODEP += expand
expand:
	@for i in $$(grep -l '	' $(filter-out $(EXPAND_NOFILES), $(EXPAND_FILES))) ; do \
		echo expanding $$i; \
		expand <$$i >expand.tmp; \
		mv expand.tmp $$i; \
	done

.PHONY: crlf
NODEP += crlf
crlf:
	find -type f '!' -path './.git/*' '!' -name '*.rsc' '!' -name '*.def' | xargs dos2unix

# Check the sources charset (no automatic fix)
.PHONY: charset
NODEP += charset
charset:
	@echo "# All the files below should use charset=utf-8"
	find . -type f '!' -path '*/.git/*' '!' -path './obj/*' '!' -path './*.img' '!' -path './?rd*' '!' -path './draft*' '!' -path './bug*' '!' -path './mkrom*' '!' -name '*.def' '!' -name '*.rsc' '!' -name '*.icn' '!' -name '*.po' -print0 | xargs -0 file -i |grep -v us-ascii

.PHONY: gitready
NODEP += gitready
gitready: expand crlf

#
# set up files in preparation for 'bug update'
#
.PHONY: bugready
NODEP += bugready
bugready: bug erd grd
	./erd -pdesk $(DESKRSC_BASE) $(DESKRSCGEN_BASE)
	./grd $(GEMRSC_BASE) $(GEMRSCGEN_BASE)
	./bug xgettext

#
# Standalone EmuCON
#

TOCLEAN += cli/version.c cli/*.o cli/*.tos

#
# local Makefile
#

ifneq (,$(wildcard local.mk))
include local.mk
endif

#
# clean
#

.PHONY: clean
NODEP += clean
clean:
	rm -f $(TOCLEAN)
	@for dir in $(wildcard tests/*/Makefile) ; do \
		$(MAKE) -C $$(dirname $$dir) clean ; \
	done

#
# ColdFire autoconverted sources.
# They are not generated automatically.
# To regenerate them, type "make coldfire-sources".
# You will need the PortAsm/68K for ColdFire tool from MicroAPL.
# See https://microapl.com/Porting/ColdFire/pacf_download.html
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
	dos2unix $@
	sed -i $@ \
		-e "s:\.section\t.bss,.*:.bss:g" \
		-e "s:\( \|\t\)bsr\(  \|\..\):\1jbsr :g" \
		-e "s:\( \|\t\)bra\(  \|\..\):\1jra  :g" \
		-e "s:\( \|\t\)beq\(  \|\..\):\1jeq  :g" \
		-e "s:\( \|\t\)bne\(  \|\..\):\1jne  :g" \
		-e "s:\( \|\t\)bgt\(  \|\..\):\1jgt  :g" \
		-e "s:\( \|\t\)bge\(  \|\..\):\1jge  :g" \
		-e "s:\( \|\t\)blt\(  \|\..\):\1jlt  :g" \
		-e "s:\( \|\t\)ble\(  \|\..\):\1jle  :g" \
		-e "s:\( \|\t\)bcc\(  \|\..\):\1jcc  :g" \
		-e "s:\( \|\t\)bcs\(  \|\..\):\1jcs  :g" \
		-e "s:\( \|\t\)bpl\(  \|\..\):\1jpl  :g" \
		-e "s:\( \|\t\)bmi\(  \|\..\):\1jmi  :g" \
		-e "s:\( \|\t\)bhi\(  \|\..\):\1jhi  :g" \
		-e "s:\( \|\t\)blo\(  \|\..\):\1jlo  :g" \
		-e "s:\( \|\t\)bhs\(  \|\..\):\1jhs  :g" \
		-e "s:\( \|\t\)bls\(  \|\..\):\1jls  :g" \
		-e "s:\( \|,\)0(%:\1(%:g"

#
# The targets for building a release are in a separate file
#

include release.mk

#
# Tests
#

.PHONY: test
NODEP += test
test:
	@for dir in $(wildcard tests/*/Makefile) ; do \
		$(MAKE) -C $$(dirname $$dir) test CC=$(CC) || exit 1; \
	done

#
# file dependencies (makefile.dep)
#

ALL_UTIL_SRC = $(wildcard util/*.[cS])
DEP_SRC = $(sort $(SRC) $(ALL_UTIL_SRC))

TOCLEAN += makefile.dep
NODEP += makefile.dep
# Theoretically, makefile.dep should depend on $(DEP_SRC). But in that case,
# makefile.dep would be rebuilt every time a single source is modified, even
# for trivial changes. This would be useless in most cases. As a pragmatic
# workaround, makefile.dep only depends on generated sources, which ensures
# they are always created first.
makefile.dep: $(GEN_SRC)
	$(CC) $(CFLAGS) -MM -DGENERATING_DEPENDENCIES $(DEP_SRC) | sed -e '/:/s,^,obj/,' >makefile.dep

# Do not include or rebuild makefile.dep for the targets listed in NODEP
# as well as the default target (currently "help").
# Be sure to keep this block at the end of the Makefile,
# after NODEP is fully populated.
GOALS_REQUIRING_DEPENDENCIES := $(filter-out $(NODEP),$(MAKECMDGOALS))
ifneq ($(GOALS_REQUIRING_DEPENDENCIES),)
# The leading dash below means: don't warn if the included file doesn't exist.
# That situation can't actually happen because we have a rule just above to
# generate makefile.dep when needed. And that rule is automatically called by
# make before inclusion.
# *But* there is a bug in make <4.2:
# Even if make knows it's going to generate makefile.dep,
# it issues a bogus warning before the generation:
# Makefile:...: makefile.dep: No such file or directory
# This has been fixed in make 4.2: no more warning in this case.
# But while older make versions are still around, we keep that leading dash.
-include makefile.dep
endif
