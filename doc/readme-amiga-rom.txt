EmuTOS - Amiga ROM version

This EmuTOS version is a ROM for Amiga hardware.

emutos-amiga.rom - English ROM
emutos-kickdisk.adf - English Amiga 1000 Kickstart disk
emutos-vampire.rom - English ROM optimized for Vampire V2 accelerators
emutos-vampire-v4sa.rom - English ROM optimized for Vampire V4 Standalone

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Note that the emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.

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
- Vampire V4 Standalone

Supported Amiga hardware features:
- Any CPU from 68000 to 68060, including Apollo 68080
- Chip RAM
- All kinds of Fast RAM
- Monochrome video modes up to 640x512 interlaced
- Keyboard
- Mouse
- A600/A1200 IDE interface (including Vampire)
- Battery backed up clock (MSM6242B and RF5C01A)
- Floppy drives
- Vampire V2/V4 SD Card

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

The Vampire V4 Standalone ROM image has been built using:
make v4sa

