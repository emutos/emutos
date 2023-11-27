EmuTOS - M548x BaS_gcc version

This EmuTOS version runs on ColdFire V4e Evaluation Boards (EVB).
No graphical output, only text mode ColdFire TOS programs through RS-232.

emutos-m548x-bas.s19 - RAM executable (English)

Supported hardware:

- M5485EVB and M5475EVB (a.k.a. Zoom ColdFire EVB)
https://www.nxp.com/products/no-longer-manufactured/mcf5485-evaluation-board:M5485EVB
https://www.nxp.com/products/no-longer-manufactured/mcf5475-evaluation-board:M5475EVB

- M5484LITE and M5474LITE (a.k.a. Zoom ColdFire LITEKIT)
https://www.nxp.com/products/no-longer-manufactured/mcf5484-lite-evaluation-kit:M5484LITE
https://www.nxp.com/products/no-longer-manufactured/mcf5474-lite-evaluation-kit:M5474LITE

Prerequisites:
Your EVB needs to boot to BaS_gcc; this is a pre-OS similar to the one found on
the FireBee. You can download it at this URL, but you will have to build it yourself:
https://github.com/firebee-org/BaS_gcc
Then you can flash the file m5484lite/bas.s19. Beware, after that, your EVB will
boot straight to BaS_gcc, and dBUG will not be available anymore.
Of course, you can always reflash dBUG afterwards.

Requirements:
- An RS-232 connection between the EVB and a controlling computer.
- On the computer, an ANSI terminal emulator connected to the RS-232 port.

Optional supported hardware:
- a CompactFlash card with a FAT16 partition.
- a micro SD card with a FAT16 partition via an Arduino-type adapter.
- PS/2 keyboard connected through an Eiffel/CAN adapter.

The use of CompactFlash and SD cards is described in m54xx-cards.txt.

How to flash EmuTOS:
You just have to flash emutos-m548x-bas.s19 with your favorite flash tool.
Beware: The start address specified in this file is set with respect to the
BaS address space, which is different from dBUG's. If you use Freescale's
CF Flasher on Windows, you need to go to the Target Config dialog and set the
Flash Base Address to 0xE0000000 before flashing.

How to run EmuTOS:

1) Switch on the EVB.
You will see a few BaS_gcc debug messages in the terminal.

2) Then you will see the EmuTOS welcome screen.
Press Escape to enter the early console.

3) If you wait too much, the desktop will run in the background.
Press Control+Z to return to EmuCON.

4) Then you can run EmuCON commands, and external text mode programs.

Restrictions:

- No graphical display.
Only text I/O through an RS/232 terminal or PS/2 keyboard

- Support for ColdFire TOS programs only. 680x0 programs will not work.

- No Atari hardware emulated. Clean programs using only the OS will work;
any that attempt direct access to the Atari hardware will not work.

This ROM image has been built using:
make m548x-bas

