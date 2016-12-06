EmuTOS - Amiga floppy version

This EmuTOS version is a boot floppy for Amiga.

emutos.adf - English Amiga boot floppy

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

The (*) in the following lists indicates support disabled by default.
See below for the details.

Supported Amiga hardware features:
- Any CPU from 68000 to 68060, including Apollo Core 68080
- Chip RAM
- Monochrome, interlaced 640x400 video mode
- Keyboard
- Mouse
- A600/A1200 IDE interface
- Battery backed up clock (MSM6242B and RF5C01A)
- Floppy drives (*)

Supported Atari features:
- ST-RAM
- ST-High video mode
- Keyboard
- Mouse
- IDE hard disk (with EmuTOS internal driver)
- XBIOS hardware clock
- ST floppy disks (only 9 sectors, 720 KB, read-only) (*)

Unsupported Amiga hardware features:
- FastRAM (supported by EmuTOS ROM only) (*)

Unsupported Atari features:
- Color video modes
- Sound

This floppy image has been built using:
make amigaflop

(*) This additional hardware support is provided by sources imported
from the AROS project. Unfortunately, due to AROS and GPL licenses
incompatibility, this support is disabled in the official EmuTOS
binaries.

You can enable full AROS support by rebuilding EmuTOS from sources using:
make amigaflop AROS=1

Personal usage of the resulting binary will be fully allowed.
However, due to the licensing issue, redistribution of such binary
is strictly forbidden.

