#
# localise.ctl - control file for 'localise'
#
# Copyright (C) 2021 The EmuTOS development team
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.
#

#
# each line corresponds to one country code; the sequence of
# data is as follows:
#   <country> <language> <keyboard> <font> <IDT defines> <group>
#
# notes:
# . fields are separated by white space
# . <group> is used to split multi-language ROMs (* => all groups)
#
us us us st IDT_12H|IDT_MMDDYY|'/'  *

de de de st IDT_24H|IDT_DDMMYY|'.'  A
fr fr fr st IDT_24H|IDT_DDMMYY|'/'  A
uk us uk st IDT_12H|IDT_DDMMYY|'/'  A
es es es st IDT_24H|IDT_DDMMYY|'/'  A
it it it st IDT_24H|IDT_DDMMYY|'/'  A
se us se st IDT_24H|IDT_DDMMYY|'/'  A
sg de sg st IDT_24H|IDT_DDMMYY|'.'  A
fi fi se st IDT_24H|IDT_DDMMYY|'.'  A
no us no st IDT_24H|IDT_DDMMYY|'/'  A
nl nl nl st IDT_24H|IDT_DDMMYY|'-'  A

cz cs cz l2 IDT_24H|IDT_DDMMYY|'.'  B
hu hu hu l2 IDT_24H|IDT_DDMMYY|'/'  B
pl pl pl l2 IDT_24H|IDT_DDMMYY|'/'  B
ru ru ru ru IDT_24H|IDT_DDMMYY|'/'  B
gr gr gr gr IDT_24H|IDT_DDMMYY|'/'  B
