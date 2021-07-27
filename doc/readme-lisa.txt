EmuTOS - Apple Lisa floppy version

This EmuTOS version is a boot floppy for Apple Lisa.

emutos.dc42 - English Apple Lisa boot floppy

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Note that the emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.

EmuTOS runs on Apple Lisa hardware, and behaves just like on Atari computers.
This is not an Atari emulator. It is a simple and clean
operating system, which allows running clean Atari programs.
No Atari hardware is emulated, and the full Lisa hardware is available.
The Atari OS calls have been implemented using the Lisa hardware.

To be clear:
- Atari programs using only the OS will work fine
- Atari programs using the hardware (games, demos...) will not work

The Lisa Boot ROM is only used to load EmuTOS into RAM. Then EmuTOS completely
takes over the machine, until reboot.

This floppy image can be used as is with the LisaEm emulator:
https://lisa.sunder.net/
You will also need a proper Boot ROM.

You can also write this image to a real 3"1/2 floppy to use it on real Lisa 2
hardware. Note that you will need an old Macintosh with a floppy drive
compatible with double density GCR floppies (800 KB) in order to write the
image. You will also need Apple Disk Copy 4.2 or compatible software.

Supported Lisa hardware features:
- 68000 CPU (5.09375 MHz)
- RAM
- Monochrome 720x364 video mode
- Keyboard
- Mouse
- Hardware clock (read-only)
- Power off

Supported Atari features:
- ST-RAM
- Monochrome video mode, 720x364 instead of 640x400
- Keyboard
- Mouse
- XBIOS hardware clock (read-only)

Unsupported Atari features:
- Floppy disk (in progress)
- Hard drive
- Color video modes
- Sound

This ROM image has been built using:
make lisaflop

