#
# the country tables.
#
# this file is included in the Makefile to build unique-country ROMs.
# also in the future it might serve as source to fill the table 
# currently in bios/country.c
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

COUNTRIES = us de fr cz

# 

ETOSLANG = $(i18n_$(COUNTRY)_lang)
ETOSKEYB = $(i18n_$(COUNTRY)_keyb)
ETOSCSET = $(i18n_$(COUNTRY)_cset)

#

FONTOBJ_ST = fnt_st_6x6.o fnt_st_8x8.o fnt_st_8x16.o 
FONTOBJ_L2 = fnt_l2_6x6.o fnt_l2_8x8.o fnt_l2_8x16.o
FONTOBJ_ALL = $(FONTOBJ_ST) $(FONTOBJ_L2)

ifneq (,$(UNIQUE))
FONTOBJ = $(FONTOBJ_$(ETOSCSET):%=obj/%)
else
FONTOBJ = $(FONTOBJ_ALL:%=obj/%)
endif

