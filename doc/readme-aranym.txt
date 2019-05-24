EmuTOS - ARAnyM version

This ROM is optimized for ARAnyM:
https://aranym.github.io/

emutos-aranym.img - Multilanguage

The following optional files are also supplied:
emucurs.rsc - modifiable mouse cursors for the AES/desktop
emucurs.def - definition file for the above
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Note that the emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.

The default language is English.
Other languages can be used by setting the NVRAM appropriately.

Alternatively, you can add the -k xx option on the ARAnyM command line
to force a specific language, where xx is:
cz - Czech
de - German
es - Spanish
fi - Finnish
fr - French
gr - Greek
it - Italian
no - Norwegian
ru - Russian (currently unsupported by ARAnyM)
se - Swedish
cd - Swiss German
us - English (US)
uk - English (UK)

Note that selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.

The ARAnyM ROM features:
- optimization for 68040 CPU
- builtin MMU support for FreeMiNT (see below)
- no ACSI support
- full NatFeat support (also enabled in the standard 512 KB version)

Builtin MMU support for FreeMiNT
As of release 0.9.11, the ARAnyM ROM always initialises the PMMU to
support enabling memory protection under FreeMiNT. If you have been using
set_mmu.prg to have FreeMiNT running with memory protection enabled, you
can safely disable it. It will do no harm to continue to use set_mmu.prg,
but it is no longer required. Because the ARAnyM ROM now always builds
PMMU tables, memory usage will increase somewhat (compared to earlier
releases) if you continue to use set_mmu.prg, or if you do not use memory
protection under FreeMiNT, or if you do not use FreeMiNT at all.

This ROM image has been built using:
make aranym

