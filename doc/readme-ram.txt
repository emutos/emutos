EmuTOS - RAM version

This special version allows EmuTOS to be loaded from the filesystem (floppy or
hard disk) without the need of replacing the system ROM.
This is the simplest way to test EmuTOS on real hardware.

boot.prg   - loader
ramtos.img - EmuTOS image

Notes:
- this version is compatible with any hardware (except the FireBee)
- special care has been taken in this release for Falcon 030 support

The default language is English.
Other supported languages are:
- Czech
- German
- Spanish
- Finnish
- French
- Greek
- Italian
- Russian
- Swiss German
They can be used by setting the NVRAM appropriately.

This special version has been built using:
make ram

