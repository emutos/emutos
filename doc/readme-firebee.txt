EmuTOS - FireBee version

This ROM is suitable for FireBee hardware.  See:
http://firebee.org/

emutosfb.s19 - Multilanguage

The default language is English.
Other supported languages are:
- Czech
- German
- Spanish
- Finnish
- French
- Greek
- Italian
- Norwegian
- Russian
- Swedish
- Swiss German
They can be used by setting the NVRAM appropriately.

Note that selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.

This ROM image can be flashed into the FireBee flash memory using any supported
flash tool, including:

1) Freescale's CF Flasher
http://cache.nxp.com/files/soft_dev_tools/software/programmers/CFFLASHER.zip
This tool runs on MS Windows and requires additional hardware.

2) Didier Méquignon's FLASH060.PRG
http://didierm.pagesperso-orange.fr/firebee.htm
Get it from the tos060.lzh archive, in the tos060/flash.too folder.
On your FireBee, start FireTOS, run FLASH060.PRG and flash emutosfb.s19.
You can also use FLASH_CF.PRG from EmuTOS itself.

In both cases, the correct EmuTOS flash parameters are prefilled,
you don't need to change anything.

To start EmuTOS on the FireBee, you have 2 options.

1) Put DIP switch #5 down and DIP switch #6 up. This is the preferred
method; EmuTOS will be started directly from the BaS in fully native mode.

2) Start FireTOS. At the very beginning, a boot menu appears. Select 2 - EMUTOS.
This will run EmuTOS over FreeRTOS and enable extra features, such as support
for USB keyboard and mouse.

The FireBee ROM features:
- Pure ColdFire OS: ColdFire TOS programs only, 680x0 programs unsupported
- IDE support for both internal CompactFlash and external IDE connector
- MMC/SD-Card support, including hot swap
- Longer welcome screen delay to allow monitors to recover from sleep modes
- NVRAM boot resolution is currently ignored, 640x480x16 is always used

This ROM image has been built using:
make firebee

