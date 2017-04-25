EmuTOS - Amiga floppy version

This EmuTOS version is a boot floppy for Amiga.

emutos.adf - English Amiga boot floppy
emutos-vampire.adf - Same floppy, optimized for Vampire V2 accelerators

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

EmuTOS runs on Amiga hardware, and behaves just like on Atari computers.
This is not an Atari emulator. It is a simple and clean
operating system, which allows running clean Atari programs.
No Atari hardware is emulated, and the full Amiga hardware is available.
The Atari OS calls have been implemented using the Amiga hardware.

To be clear:
- Atari programs using only the OS will work fine
- Atari programs using the hardware (games, demos...) will not work

AmigaOS is only used to load EmuTOS into RAM. Then EmuTOS completely takes
over the machine, and AmigaOS is no more available until reboot.

Unlike the Amiga EmuTOS ROM, these floppies are full-featured by default.
They are the easiest way to test EmuTOS on real hardware.
Floppy emulators such as HxC and Gotek are supported, and even recommended.

Supported Amiga hardware features:
- Any CPU from 68000 to 68060, including Apollo 68080
- Chip RAM
- FastRAM
- Monochrome, interlaced 640x400 video mode
- Keyboard
- Mouse
- A600/A1200 IDE interface
- Battery backed up clock (MSM6242B and RF5C01A)
- Floppy drives

Supported Atari features:
- ST-RAM
- ST-High video mode
- Keyboard
- Mouse
- IDE hard disk (with EmuTOS internal driver)
- XBIOS hardware clock
- ST floppy disks (only 9 sectors, 720 KB, read-only)

Unsupported Atari features:
- Color video modes
- Sound

Special features of emutos-vampire.adf:
- Optimized for 68040 (best option for Apollo 68080)
- EmuTOS itself runs in FastRAM, for best performance

These floppy images have been built using:
make amigaflop
make amigaflopvampire

