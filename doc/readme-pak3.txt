EmuTOS - PAK/3 versions

These ROMs are suitable for the following hardware:
- ST or Mega ST with a PAK68/3 board

Note 1: These ROMs do _not_ require special boot ROMs on the original
motherboard to work; they work correctly with any original TOS ROMs on
the motherboard.

Note 2: Extra hardware is autodetected, but the following TT- and
Falcon-specific hardware is not supported: memory control unit,
video, SCSI, NVRAM, and the second MFP of a TT.

The desktop features are comparable to Atari TOS 2.

Each ROM contains a single language:

etospak3cz.img - Czech (PAL)
etospak3de.img - German (PAL)
etospak3es.img - Spanish (PAL)
etospak3fi.img - Finnish (PAL)
etospak3fr.img - French (PAL)
etospak3gr.img - Greek (PAL)
etospak3hu.img - Hungarian (PAL)
etospak3it.img - Italian (PAL)
etospak3nl.img - Dutch (PAL)
etospak3no.img - Norwegian (PAL)
etospak3pl.img - Polish (PAL)
etospak3ru.img - Russian (PAL)
etospak3se.img - Swedish (PAL)
etospak3sg.img - Swiss German (PAL)
etospak3us.img - English (NTSC)
etospak3uk.img - English (PAL)

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Notes on possible points of confusion
1. The emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.
2. Selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.

These ROM images have been built using:
make allpak3

