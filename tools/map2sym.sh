#!/bin/sh

usage ()
{
	name=${0##*/}
	echo
	echo "usage: $name <map file>"
	echo
	echo "convert (e.g. EmuTOS) linker map file to 'nm' format"
	echo "understood by the Hatari debugger 'symbols' command."
	echo
	echo "For example:"
	echo "  $name emutos.map > etos512.sym"
	echo
	echo "ERROR: $1!"
	echo
	exit 1
}
if [ $# -ne 1 ]; then
	usage "incorrect number of arguments"
fi

if [ \! -f $1 ]; then
	usage "given '$1' address map file not found"
fi

# Hatari allows symbols marked as code to be used as disassembly
# addresses, so if certain segment types may contain or be used
# both for code and (e.g. const) data types, mark them code (T)
# rather than data (D).  Object files and common symbols here
# are such.
#
# Const data gets assigned to code section by compiler.
# Special case common prefixes for them and set their type
# as data.
awk '
BEGIN {
	type = "";
	/* OS variables & font arrays */
	split("_os_ _fnt_", prefixes);
}
function type4str (type, str) {
	if (type != "T") {
		return type;
	}
	for (prefix in prefixes) {
		/* starts with indexed prefix? */
		if (index(str, prefixes[prefix]) == 1) {
			return "D";
		}
	}
	return type;
}
function print_object (addr, name) {
	if (name) {
		/* remove object file path */
		sub("obj/", "", name);
		print addr, "T", name;
	}
}
/^\.text/  { type = "T"; print_object($2, $4); }
/^\.data/  { type = "D"; print_object($2, $4); }
/^\.bss/   { type = "B"; print_object($2, $4); }
/^ COMMON/ { type = "T"; print_object($2, $4); }
/^ +0x/    {
	if (type) {
		print $1, type4str(type, $2), $2;
	}
}
' $1 | sort -n
