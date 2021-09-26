EmuTOS - AlphaSmart Dana version.

This EmuTOS version is a RAM-loadable version for the AlphaSmart Dana portable
laptop thing.

emutos-dana.rom - loadable kernel

The following optional files are also supplied:
tools/danacal.c - touchscreen calibration tool
tools/danarun/danarun.prc - PalmOS boot loader

EmuTOS runs on AlphaSmart hardware, and behaves just like on AlphaSmart
computers.  This is not an Atari emulator. It is a simple and clean operating
system, which allows running clean Atari programs.  No Atari hardware is
emulated.  The Atari OS calls have been implemented using the Amiga hardware.

To be clear:
- Atari programs using only the OS will work fine
- Atari programs using the hardware (games, demos...) will not work

PalmOS is only used to load EmuTOS into RAM. Then EmuTOS completely takes
over the machine, and PalmOS is no longer available until reboot.

This is a very early port and is only just working. Your mileage may vary,
things in the rear view mirror may be closer than they appear, do not stare
into laser with last remaining eye and do not taunt the happy fun ball.

Important note: the 160-pixel high AlphaSmart screen is smaller than Atari low
resolution mode, and the 560-pixel wide AlphaSmart screen is smaller than
80-column mode. I've found quite a lot of Atari software refuses to run because
it thinks the screen is too small. This may be fixable with the appropriate
shims but I don't know how.

Supported features:
- 68328 CPU
- 4MB of RAM (more is available but not yet usable; this is a bug)
- monochrome video at 560x160
- keyboard
- touchscreen (requiring calibration, and rather unreliable)
- one of the two SD card slots

Unsupported AlphaSmart features:
- anything to do with the battery --- in fact, no verification has been done that we're not writing garbage to the charger control chip, so I'd suggest running it without any battery installed
- anything to do with power management
- anything to do with USB
- the second SD card slot
- sound

These floppy images have been built using:
make dana

