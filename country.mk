#
# country.mk - country-related items
#
# Copyright (C) 2002-2025 The EmuTOS development team
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

COUNTRIES = us de fr cz gr es fi sg ru it uk no se nl pl hu tr ro ca
ifeq (,$(findstring $(COUNTRY),$(COUNTRIES)))
$(error Unknown COUNTRY=$(COUNTRY))
endif

#

ETOSLANG = $(shell awk '$$1 == country { print $$2 }' country=$(COUNTRY) <localise.ctl)

#

FONTOBJ_ST = fnt_st_6x6.o fnt_st_8x8.o fnt_st_8x16.o
FONTOBJ_L2 = fnt_l2_6x6.o fnt_l2_8x8.o fnt_l2_8x16.o
FONTOBJ_GR = fnt_gr_6x6.o fnt_gr_8x8.o fnt_gr_8x16.o
FONTOBJ_RU = fnt_ru_6x6.o fnt_ru_8x8.o fnt_ru_8x16.o
FONTOBJ_TR = fnt_tr_6x6.o fnt_tr_8x8.o fnt_tr_8x16.o
FONTOBJ_ALL = $(FONTOBJ_ST) $(FONTOBJ_L2) $(FONTOBJ_GR) $(FONTOBJ_RU) $(FONTOBJ_TR)
FONTOBJ_COMMON = obj/fnt_off_6x6.o obj/fnt_off_8x8.o
FONTOBJ = $(FONTOBJ_ALL:%=obj/%)

TOCLEAN += obj/*.a

obj/libfont.a: $(FONTOBJ)
	$(AR) $(ARFLAGS) $@ $^
