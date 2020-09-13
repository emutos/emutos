EmuTOS - 192 KB versions

These ROMs are suitable for the following hardware:
- ST / STf
- Mega ST
- emulators of the above

Unlike other ROM versions, they do NOT autodetect extra hardware, and might not
work on machines with additional hardware. For example, they don't work under
the Hatari emulator's Falcon emulation due to missing VIDEL support.
Also, they only work with plain 68000 CPU.

Each ROM contains a single language:

etos192cz.img - Czech (PAL)
etos192de.img - German (PAL)
etos192es.img - Spanish (PAL)
etos192fi.img - Finnish (PAL)
etos192fr.img - French (PAL)
etos192gr.img - Greek (PAL)
etos192hu.img - Hungarian (PAL)
etos192it.img - Italian (PAL)
etos192nl.img - Dutch (PAL)
etos192no.img - Norwegian (PAL)
etos192pl.img - Polish (PAL)
etos192ru.img - Russian (PAL)
etos192se.img - Swedish (PAL)
etos192sg.img - Swiss German (PAL)
etos192us.img - English (NTSC)
etos192uk.img - English (PAL)

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Note that the emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.

Note that selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.

Due to size limitations, the 192 KB ROMs contain:
- no EmuCON
- limited desktop features (comparable to Atari TOS 1)
- no builtin text file viewer and print function

These ROM images have been built using:
make all192

