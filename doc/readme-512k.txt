EmuTOS - 512 KB versions

These ROMs are suitable for the following hardware:
- TT
- Falcon
- emulators of the above

Note: Extra hardware is autodetected.

The desktop features are comparable to Atari TOS 3 or 4.

Each ROM contains a single language for display:

etos512ca.img - Catalan (PAL)
etos512cz.img - Czech (PAL)
etos512de.img - German (PAL)
etos512es.img - Spanish (PAL)
etos512fi.img - Finnish (PAL)
etos512fr.img - French (PAL)
etos512gr.img - Greek (PAL)
etos512hu.img - Hungarian (PAL)
etos512it.img - Italian (PAL)
etos512nl.img - Dutch (PAL)
etos512no.img - Norwegian (PAL)
etos512pl.img - Polish (PAL)
etos512ro.img - Romanian (PAL)
etos512ru.img - Russian (PAL)
etos512se.img - Swedish (PAL)
etos512sg.img - Swiss German (PAL)
etos512tr.img - Turkish (PAL)
etos512us.img - English (NTSC)
etos512uk.img - English (PAL)

However, note that these ROMs will use the standard values from NVRAM
for keyboard, date/time etc.

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Notes on possible points of confusion
1. The emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.
2. Selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.
3. The 'Shutdown' menu item is active when EmuTOS is run under an
emulator supporting NatFeats (under Hatari, you need to enable this
with the "--natfeats on" option).

These ROM images have been built using:
make all512

