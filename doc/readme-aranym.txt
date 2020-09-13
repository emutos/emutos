EmuTOS - ARAnyM version

This ROM is optimized for ARAnyM:
https://aranym.github.io/

emutos-aranym.img - Multilanguage

The following optional files are also supplied:
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
hu - Hungarian
it - Italian
nl - Dutch
no - Norwegian
pl - Polish
ru - Russian
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

Builtin MMU support for FreeMiNT:
To support enabling memory protection under FreeMiNT, the 68040 PMMU
must be initialised. Under old versions of EmuTOS, this was done by the
standalone program set_mmu.prg. Since EmuTOS release 0.9.11, the ARAnyM
ROM initialises the PMMU itself. If you have been using set_mmu.prg, you
can safely disable it, although it won't cause any problems if you leave
it enabled.
As of EmuTOS release 0.9.12, to cater for those who do not need PMMU
support, EmuTOS queries ARAnyM to see if enabling the PMMU is necessary.
Support for this query is currently only available in development
releases of ARAnyM; if the query fails, the PMMU tables are built.

This ROM image has been built using:
make aranym

