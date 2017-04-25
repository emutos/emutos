EmuTOS - Amiga ROM version

This EmuTOS version is a ROM for Amiga hardware.

emutos-amiga.rom - English ROM
emutos-kickdisk.adf - English Amiga 1000 Kickstart disk
emutos-vampire.rom - English ROM optimized for Vampire V2 accelerators

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

This is a Kickstart ROM replacement.
EmuTOS runs on Amiga hardware, and behaves just like on Atari computers.
This is not an Atari emulator. It is a simple and clean
operating system, which allows running clean Atari programs.
No Atari hardware is emulated, and the full Amiga hardware is available.
The Atari OS calls have been implemented using the Amiga hardware.

To be clear:
- Atari programs using only the OS will work fine
- Atari programs using the hardware (games, demos...) will not work

Note that EmuTOS is totally unrelated to AmigaOS.
No AmigaOS component is required, or even usable.

EmuTOS for Amiga has been successfully tested on:
- WinUAE emulator, with any hardware and CPU combination
- Amiga 1000, using emutos-kickdisk.adf instead of the Kickstart floppy
- Amiga 1200 with Blizzard 1260 accelerator board, using BlizKick
- Amiga 600, as ROM replacement
- Amiga 500 with Vampire V2 accelerator board and experimental MapROM feature

The (*) in the following lists indicates support disabled by default.
See below for the details.

Supported Amiga hardware features:
- Any CPU from 68000 to 68060, including Apollo 68080
- Chip RAM
- Monochrome, interlaced 640x400 video mode
- Keyboard
- Mouse
- A600/A1200 IDE interface
- Battery backed up clock (MSM6242B and RF5C01A)
- Zorro II/III Fast RAM, Slow RAM, A3000/A4000 motherboard RAM (*)
- Vampire V2 FastRAM
- Floppy drives

Supported Atari features:
- ST-RAM
- ST-High video mode
- Keyboard
- Mouse
- IDE hard disk (with EmuTOS internal driver)
- XBIOS hardware clock
- Alt-RAM
- ST floppy disks (only 9 sectors, 720 KB, read-only)

Unsupported Atari features:
- Color video modes
- Sound

This ROM image has been built using:
make amiga

The Vampire V2 ROM image has been built using:
make amigavampire

(*) This additional hardware support is provided by sources imported
from the AROS project. Unfortunately, due to AROS and GPL licenses
incompatibility, this support is disabled in the official EmuTOS
binaries.

You can enable full AROS support by rebuilding EmuTOS from sources using:
make amiga AROS=1

Personal usage of the resulting binary will be fully allowed.
However, due to the licensing issue, redistribution of such binary
is strictly forbidden.

Note: Alt-RAM support is available without AROS support with the EmuTOS floppy
for Amiga. See the emutos-amiga-floppy-*.zip archive.

