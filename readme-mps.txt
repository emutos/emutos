This is a hacked EmuTOS with some goodies to make it cooler.
It is specifically targetted for the stock Atari ST/STe.

Changelog:
2021-Mar-09 VB: 
	* EmuCON now recognizes the PATH environnement variable that is provided to the AES (if any).
	* Sound is played at boot time using the PSG. The sound is different for cold and warm boot.
	  For cold boot it's C5 C4 G4. For warm boot it C5 C5 C5.
	  