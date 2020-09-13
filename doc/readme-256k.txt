EmuTOS - 256 KB versions

These ROMs are suitable for the following hardware:
- STe
- Mega STe
- emulators of the above

Note: Extra hardware is autodetected, but the following TT- and
Falcon-specific hardware is not supported: memory control unit,
video, SCSI, NVRAM, and the second MFP of a TT.

The desktop features are comparable to Atari TOS 2.

Each ROM contains a single language:

etos256cz.img - Czech (PAL)
etos256de.img - German (PAL)
etos256es.img - Spanish (PAL)
etos256fi.img - Finnish (PAL)
etos256fr.img - French (PAL)
etos256gr.img - Greek (PAL)
etos256hu.img - Hungarian (PAL)
etos256it.img - Italian (PAL)
etos256nl.img - Dutch (PAL)
etos256no.img - Norwegian (PAL)
etos256pl.img - Polish (PAL)
etos256ru.img - Russian (PAL)
etos256se.img - Swedish (PAL)
etos256sg.img - Swiss German (PAL)
etos256us.img - English (NTSC)
etos256uk.img - English (PAL)

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
make all256

