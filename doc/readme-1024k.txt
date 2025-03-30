EmuTOS - 1024 KB version

This ROM image is not suitable for original Atari hardware, due to
hardware restrictions.  It is suitable for the following emulators:
- Hatari

This ROM image is the optimal one for Hatari, with any combination of
emulated hardware.  But note that, if you want to use it on an emulated
TT or Falcon, it requires a "recent" version of Hatari, specifically:
- a release newer than v2.3.1 (if using an official Hatari release)
- built from commit 21669ea or newer (if building from the repository)

Some legacy programs may not work with a 1024k image; for these one may
need to use a smaller EmuTOS image.

etos1024k.img - Multilanguage
etos1024k.sym - Symbol address information for Hatari debugger and profiler

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Additional information on debugging EmuTOS and its software compatibility
is included with Hatari:
    https://www.hatari-emu.org/doc/emutos.txt

The default language is English.  Other supported languages are:
- Catalan
- Czech
- Dutch
- Finnish
- French
- German
- Greek
- Hungarian
- Italian
- Norwegian
- Polish
- Romanian
- Russian
- Spanish
- Swedish
- Swiss German
- Turkish
They can be selected by setting the NVRAM appropriately.

Notes on possible points of confusion
1. The emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.
2. Selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.
3. The 'Shutdown' menu item is active when EmuTOS is run under an
emulator supporting NatFeats (under Hatari, you need to enable this
with the "--natfeats on" option).

This ROM image has been built using:
make 1024

