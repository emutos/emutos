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

How to use:
- You'll need two SD cards; one is formatted for Atari TOS and the other for
  PalmOS. (It ought to be possible to combine the two, as both operating
  systems use standard FAT filesystems, but they're both quite picky about
  which formats they support and having two cards is easier.)
- Build EmuTOS and danarun.prc.
- Copy emutos-dana.rom -> \kernel.img on the PalmOS card.
- Copy tools/danarun/danarun.prc -> \palm\launcher\danarun.prc on the PalmOS
  card.
- Insert the PalmOS card in the _left_ slot (the one furthest from the power
  cable).
- Insert the Atari TOS card in the _right_ slot (the one closest to the power
  cable).
- Ensure that any files you want to keep on the Dana are backed up, because
  running EmuTOS will wipe the memory.
- Run DanaRun on the Dana.
- EmuTOS should start.

Calibrating the touch screen:
- You will want to build tools/danacal.c and copy the .prg file to the Atari
  TOS card.
- Run it, either from the ^Z EmuCON prompt or using the keyboard mouse
  emulation.
- Follow the instructions.
- The touchscreen should now work, sort of. If it seems noticeably off,
  recalibrate.
- Be prepared to be underwhelmed (it works rather badly; this is a bug).

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

