#
# country.mk - the country tables
#
# Copyright (c) 2002-2005 EmuTOS development team.
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

# this file is included in the Makefile to build unique-country ROMs.
#
# indormation in lines below serve also as source to fill the tables 
# needed by bios/country.c (tables generated in bios/ctables.h). For
# each country there should be a four line paragraph giving the language,
# keyboard code, charset and IDT.
#

i18n_us_lang = us
i18n_us_keyb = US
i18n_us_cset = ST
i18n_us_idt  = IDT_12H | IDT_MMDDYY | '/'

i18n_de_lang = de
i18n_de_keyb = DE
i18n_de_cset = ST
i18n_de_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_fr_lang = fr
i18n_fr_keyb = FR
i18n_fr_cset = ST
i18n_fr_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_cz_lang = cs
i18n_cz_keyb = CZ
i18n_cz_cset = L2
i18n_cz_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_gr_lang = gr
i18n_gr_keyb = GR
i18n_gr_cset = GR
i18n_gr_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_es_lang = es
i18n_es_keyb = US
i18n_es_cset = L9
i18n_es_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_fi_lang = fi
i18n_fi_keyb = SE
i18n_fi_cset = ST
i18n_fi_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_sg_lang = de
i18n_sg_keyb = SG
i18n_sg_cset = ST
i18n_sg_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_ru_lang = ru
i18n_ru_keyb = RU
i18n_ru_cset = RU
i18n_ru_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_it_lang = it
i18n_it_keyb = IT
i18n_it_cset = ST
i18n_it_idt  = IDT_24H | IDT_DDMMYY | '/'

i18n_uk_lang = us
i18n_uk_keyb = UK
i18n_uk_cset = ST
i18n_uk_idt  = IDT_12H | IDT_DDMMYY | '/'

COUNTRIES = us de fr cz gr es fi sg ru it uk
ifeq (,$(findstring $(COUNTRY),$(COUNTRIES)))
$(error Unknown COUNTRY=$(COUNTRY))
endif

# 

ETOSLANG = $(i18n_$(COUNTRY)_lang)
ETOSKEYB = $(i18n_$(COUNTRY)_keyb)
ETOSCSET = $(i18n_$(COUNTRY)_cset)

#

FONTOBJ_ST = fnt_st_6x6.o fnt_st_8x8.o fnt_st_8x16.o 
FONTOBJ_L2 = fnt_l2_6x6.o fnt_l2_8x8.o fnt_l2_8x16.o
FONTOBJ_L9 = fnt_l9_6x6.o fnt_l9_8x8.o fnt_l9_8x16.o
FONTOBJ_GR = fnt_gr_6x6.o fnt_gr_8x8.o fnt_gr_8x16.o
FONTOBJ_RU = fnt_ru_6x6.o fnt_ru_8x8.o fnt_ru_8x16.o
FONTOBJ_ALL = $(FONTOBJ_ST) $(FONTOBJ_L2) $(FONTOBJ_L9) $(FONTOBJ_GR) $(FONTOBJ_RU)

ifneq (,$(UNIQUE))
FONTOBJ = $(FONTOBJ_$(ETOSCSET):%=obj/%)
else
FONTOBJ = $(FONTOBJ_ALL:%=obj/%)
endif

