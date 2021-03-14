EmuTOS - floppy versions

These special versions allow EmuTOS to be loaded from a boot floppy
without the need of replacing the system ROM.
This is the simplest way to test EmuTOS on real hardware from a floppy.
The drawback is less available RAM compared to ROM versions.

emutoscz.st - Czech (PAL)
emutosde.st - German (PAL)
emutoses.st - Spanish (PAL)
emutosfi.st - Finnish (PAL)
emutosfr.st - French (PAL)
emutosgr.st - Greek (PAL)
emutoshu.st - Hungarian (PAL)
emutosit.st - Italian (PAL)
emutosnl.st - Dutch (PAL)
emutosno.st - Norwegian (PAL)
emutospl.st - Polish (PAL)
emutosru.st - Russian (PAL)
emutosse.st - Swedish (PAL)
emutossg.st - Swiss German (PAL)
emutostr.st - Turkish (PAL)
emutosuk.st - English (PAL)
emutosus.st - English (NTSC)

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Note that the emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.

Notes:
- these versions are compatible with any Atari hardware (except the FireBee)
- TT and Falcon 030 are supported
- the language of the Norwegian/Swedish versions is English; however the
  keyboard layouts are Norwegian/Swedish

These floppies are provided in the form of raw single-sided floppy images.
They can be used as is on most emulators.

In order to use a floppy image on real hardware, first you need to write it
to a real floppy using some raw image tool.

On Windows, you can use the RawWrite tool:
http://www.chrysocome.net/rawwrite

These special versions have been built using:
make allflop

