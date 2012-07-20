EmuTOS - Amiga version

This EmuTOS version runs on Amiga hardware.

emutos-amiga.rom - English ROM
emutos-kickdisk.adf - English Amiga 1000 Kickstart disk

This is a Kickstart ROM replacement.
EmuTOS will boot and run just as on Atari computers.
This is not an Atari emulator. It is a simple an clean
operating system, which allows running clean Atari programs.
No Atari hardware is emulated, and the full Amiga hardware is available.
The Atari OS calls have been implemented using the Amiga hardware.

To be clear:
- Atari programs using the OS only will work fine
- Atari programs using the hardware (games, demos...) will not work

Note that EmuTOS is totally unrelated to AmigaOS.
No AmigaOS component is required, or even usable.

Currently, only the WinUAE emulator is actively supported,
but it should also work on other emulators or real hardware.

Supported Amiga hardware features:
- Any CPU from 68000 to 68060
- Chip RAM, Zorro II/III Fast RAM, Slow RAM, A3000/A4000 motherboard RAM
- Monochrome, interlaced 640x400 video mode
- Keyboard
- Mouse
- A600/A1200 IDE interface

Supported Atari features:
- ST-RAM
- ST-High video mode
- Keyboard
- Mouse
- IDE hard disk (with EmuTOS internal driver)

Unsupported Atari features:
- Color video modes
- Floppy disks
- Sound

This ROM image has been built using:
make amiga

