EmuTOS - 512 KB version

This ROM is suitable for the following hardware:
- TT
- Falcon
- emulators of the above

Note: Extra hardware is autodetected.

This ROM is the optimal one for Hatari, with any hardware combination.

Some legacy programs don't work with 512k TOS images, for those one may
need to use a smaller EmuTOS image.

etos512k.img - Multilanguage
etos512k.sym - Symbol address information for Hatari debugger and profiler

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Additional information on debugging EmuTOS and its software compatibility
is included with Hatari:
	https://hatari.tuxfamily.org/doc/emutos.txt

The default language is English.  Other supported languages are:
- Czech
- German
- Spanish
- Finnish
- French
- Greek
- Hungarian
- Italian
- Dutch
- Norwegian
- Polish
- Russian
- Swedish
- Swiss German
They can be used by setting the NVRAM appropriately.  If you wish to use
this version of the ROM on a machine without NVRAM, the default language
can be changed by using the tos-lang-change tool included with the EmuTOS
sources.

Notes on possible points of confusion
1. The emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.
2. Selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.
3. The 'Shutdown' menu item is active when EmuTOS is run under an
emulator supporting NatFeats (under Hatari, you need to enable this
with the "--natfeats on" option).

This ROM image has been built using:
make 512

