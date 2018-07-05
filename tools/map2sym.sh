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
# Special case font objects containing just const font struct
# symbols as having data type (helps Hatari profiler a bit).
#
# Print object addresses only if they contain other symbols,
# or their address differs from other addresses, to avoid
# duplicate & misleading object addresses.
awk '
BEGIN {
    # system variables type at startup
    objtype = "T";
    objaddr = 0x0;
}
function set_object (addr, type, name) {
    addr = strtonum(addr);
    if (addr && objaddr && objaddr != addr) {
        printf "0x%08x %s %s\n", objaddr, objtype, objname;
    }
    if (name) {
        # remove EmuTOS object file path
        sub("obj/", "", name);
        objname = name;
        objaddr = addr;
    }
    # font objects contain just font data structs
    if (index(name, "fnt_") == 1) {
        objtype = "D";
    } else {
        objtype = type;
    }
}
# special sections
/^ *\.lowstram/  { objtype = "D"; }
/^ *\.laststram/ { objtype = "D"; }
/^ *\.stack/     { objtype = "B"; }
/^ COMMON/       { objtype = "T"; }
# normal TEXT, BSS & DATA sections
/^ *\.text/ { set_object($2, "T", $4); }
/^ *\.data/ { set_object($2, "D", $4); }
/^ *\.bss/  { set_object($2, "B", $4); }
# symbols in any of the sections
/^ +0x/     {
    if (objtype) {
        if (objaddr) {
            printf "0x%08x %s %s\n", objaddr, objtype, objname;
            objaddr = 0x0;
        }
        if ($2 != "." && $2 != "ASSERT") {
            printf "0x%08x %s %s\n", strtonum($1), objtype, $2;
        }
    }
}
# clean out library paths for objects coming from static libraries
' $1 | sed -e 's/ [^ ]\+(//' -e 's/)//' | sort -n
