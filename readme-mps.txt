This is a hacked EmuTOS with some goodies to make it cooler.
It is specifically targetted for the stock Atari ST/STe.

Changelog:
2021-Oct06 VB:
	* Refactor BIOS-BDOS dependencies so BDOS doesn't directly call BIOS. The separation is
	  not very clean and the BDOS requires some things for which the BIOS has no API. These
	  this are in include/biosext.h
	* The logo displayed during boot is now compressed by a tool logo_compressor so if we
	  create a bigger logo for bigger resolutions, it doesn't uses crazy space.

2021-Mar-09 VB: 
	* EmuCON recognizes the PATH environnement variable that is provided to the AES (if any).
	* Sound is played at boot time using the PSG. The sound is different for cold and warm boot.
	  For cold boot it's C5 C4 G4. For warm boot it C5 C5 C5.
	* Dark theme for boot.
	* Blue-ish background for desktop.
	
2020-Mar-10 VB:
	* Support ACCPATH environment variable in the AES, indicating the folder where to search
	  for accessories. Must not include trailing backslash.
	  
2020-Mar-13 VB:
	* Reduce size by doing the following:
	  * On STf (MPS_STF=1), don't include blitter code.
	  * On STe (MPS_STE=1), always use blitter and don't include software blitter emulation.
	  This is controlled by the MPS_BLITTER_ALWAYS_ON compile switch.

2020-Mar-21 VB:
	* Reduce size by not compiling support for non-present hardware:
	  * On STf (MPS_STF=1), don't include support for DMA sound (set existing CONF_WITH_DMASOUND to 0).
	  * On STe (MPS_STE=1), don't include support for Falcon sound (MPS_STE_SOUND_ONLY).


Wishlist:
	* BUG: Fix Dark theme: EmuCOn is inreadable if launched from boot
	* Make ST-MEDIUM the default resolution on colour screens
	* Allow EmuCON to run batch files (simple list of commands to execute in sequence)
	* Allow support for <bootdrive>:\AUTO.BAT that will use emucon to run programs at startup
	  (so it is possible to run programs with command line parameters at startup)
	* Preserve windows when switching resolutions (create dummy EMUDESK.INF in memory
	  and use it when changing resolutions rather than loading it from disk)
	* Add option to install a RAM-disc of configurable size
	* Share viewer file viewer between EmuCON and EmuDesk
	* Write better file viewer that can be controlled with mouse and allow to go back up in the file
	* Write minimal text editor and embed (will have to be very small!)
	* Create interface for devices (like FreeMiNT's XDD interface)
