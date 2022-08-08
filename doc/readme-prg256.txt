EmuTOS - PRG256 versions

These special versions allow EmuTOS to be loaded from the filesystem
(floppy or hard disk) without needing to replace the system ROM.
This is the simplest way to test EmuTOS on real hardware.
The drawback is less available RAM compared to ROM versions.

These versions are suitable for the following hardware:
- STe
- Mega STe
- emulators of the above

Note: Extra hardware is autodetected, but the following TT- and
Falcon-specific hardware is not supported: memory control unit,
video, DSP, SCSI, NVRAM, and the second MFP of a TT.

The desktop features are comparable to Atari TOS 2.

emu256cz.prg - Czech (PAL)
emu256de.prg - German (PAL)
emu256es.prg - Spanish (PAL)
emu256fi.prg - Finnish (PAL)
emu256fr.prg - French (PAL)
emu256gr.prg - Greek (PAL)
emu256hu.prg - Hungarian (PAL)
emu256it.prg - Italian (PAL)
emu256nl.prg - Dutch (PAL)
emu256no.prg - Norwegian (PAL)
emu256pl.prg - Polish (PAL)
emu256ru.prg - Russian (PAL)
emu256se.prg - Swedish (PAL)
emu256sg.prg - Swiss German (PAL)
emu256tr.prg - Turkish (PAL)
emu256uk.prg - English (PAL)
emu256us.prg - English (NTSC)

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Note that the emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.

Notes:
- if you plan to run one of these programs from the AUTO folder, you MUST
  rename it so the first 6 letters of the name are EMUTOS; otherwise EmuTOS
  will continually reload itself each time it processes the AUTO folder
- the language of the Norwegian/Swedish versions is English; however the
  keyboard layouts are Norwegian/Swedish
- when using these versions with a Magnum ST/STE alt-RAM card, the Magnum
  driver for TOS must not have been loaded before running emu256*.prg
- there is no multilanguage version since NVRAM is not supported by ST(e)
  hardware

These special versions have been built using:
make allprg256

