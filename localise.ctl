#
# localise.ctl - control file for 'localise'
#
# Copyright (C) 2021-2024 The EmuTOS development team
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

#
# each line corresponds to one country code; the sequence of
# data is as follows:
#   <country> <language> <keyboard> <font> <IDT defines>
#
# notes:
# . fields are separated by white space
#
us us us st IDT_12H|IDT_MMDDYY|'/'

de de de st IDT_24H|IDT_DDMMYY|'.'
fr fr fr st IDT_24H|IDT_DDMMYY|'/'
uk us uk st IDT_12H|IDT_DDMMYY|'/'
es es es st IDT_24H|IDT_DDMMYY|'/'
it it it st IDT_24H|IDT_DDMMYY|'/'
se us se st IDT_24H|IDT_DDMMYY|'/'
sg de sg st IDT_24H|IDT_DDMMYY|'.'
fi fi se st IDT_24H|IDT_DDMMYY|'.'
no us no st IDT_24H|IDT_DDMMYY|'/'
nl nl nl st IDT_24H|IDT_DDMMYY|'-'

cz cs cz l2 IDT_24H|IDT_DDMMYY|'.'
hu hu hu l2 IDT_24H|IDT_DDMMYY|'/'
pl pl pl l2 IDT_24H|IDT_DDMMYY|'/'
ru ru ru ru IDT_24H|IDT_DDMMYY|'/'
gr gr gr gr IDT_24H|IDT_DDMMYY|'/'
tr tr tr tr IDT_24H|IDT_DDMMYY|'.'
ro ro ro l2 IDT_24H|IDT_DDMMYY|'.'
ca ca es st IDT_24H|IDT_DDMMYY|'/'
