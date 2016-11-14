EmuTOS - Amiga version

This EmuTOS version runs on Amiga hardware.

emutos-amiga.rom - English ROM
emutos-kickdisk.adf - English Amiga 1000 Kickstart disk

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
- Amiga 500 with Vampire 500 V2 accelerator board

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
- Zorro II/III Fast RAM, Slow RAM, A3000/A4000 motherboard RAM (*)
- Floppy drives (*)

Supported Atari features:
- ST-RAM
- ST-High video mode
- Keyboard
- Mouse
- IDE hard disk (with EmuTOS internal driver)
- XBIOS hardware clock
- Alt-RAM (*)
- ST floppy disks (only 9 sectors, 720 KB, read-only) (*)

Unsupported Atari features:
- Color video modes
- Sound

This ROM image has been built using:
make amiga

(*) This additional hardware support is provided by sources imported
from the AROS project. Unfortunately, due to AROS and GPL licenses
incompatibility, this support is disabled in the official EmuTOS
binaries.

You can enable full AROS support by rebuilding EmuTOS from sources using:
make amiga AROS=1

Personal usage of the resulting binary will be fully allowed.
However, due to the licensing issue, redistribution of such binary
is strictly forbidden.

