EmuTOS - FireBee version

These ROMs are suitable for FireBee hardware.  See:
http://firebee.org/

The desktop features are comparable to Atari TOS 4.

Each ROM contains a single language for display:

emutosfbcz.s19 - Czech (PAL)
emutosfbde.s19 - German (PAL)
emutosfbes.s19 - Spanish (PAL)
emutosfbfi.s19 - Finnish (PAL)
emutosfbfr.s19 - French (PAL)
emutosfbgr.s19 - Greek (PAL)
emutosfbhu.s19 - Hungarian (PAL)
emutosfbit.s19 - Italian (PAL)
emutosfbnl.s19 - Dutch (PAL)
emutosfbno.s19 - Norwegian (PAL)
emutosfbpl.s19 - Polish (PAL)
emutosfbru.s19 - Russian (PAL)
emutosfbse.s19 - Swedish (PAL)
emutosfbsg.s19 - Swiss German (PAL)
emutosfbtr.s19 - Turkish (PAL)
emutosfbus.s19 - English (NTSC)
emutosfbuk.s19 - English (PAL)

However, note that these ROMs will use the standard values from NVRAM
for keyboard, date/time etc.

The following optional files are also supplied:
emuicon.rsc - contains additional icons for the desktop
emuicon.def - definition file for the above

Notes on possible points of confusion
1. The emuicon.rsc file format differs from deskicon.rsc used by later
versions of the Atari TOS desktop.
2. Selecting Norwegian/Swedish currently sets the language to English,
but the keyboard layout to Norwegian/Swedish.

This ROM image can be flashed into the FireBee flash memory using any supported
flash tool, including:

1) Freescale's CF Flasher
https://www.nxp.com/files-static/soft_dev_tools/software/programmers/CFFLASHER.zip
This tool runs on MS Windows and requires additional hardware.

2) Didier Méquignon's FLASH060.PRG
https://didierm.pagesperso-orange.fr/firebee.htm
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

These ROM images have been built using:
make allfirebee

