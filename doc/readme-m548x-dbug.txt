EmuTOS - M548x dBUG version

This EmuTOS version runs on ColdFire V4e Evaluation Boards (EVB).
No graphical output, only text mode ColdFire TOS programs through RS-232.

emutos-m548x-dbug.s19 - RAM executable (English)

Supported hardware:

- M5485EVB (a.k.a. Zoom ColdFire EVB)
http://www.nxp.com/products/microcontrollers-and-processors/more-processors/coldfire-plus-coldfire-mcus-mpus/coldfire-mpus/v4-mpu/mcf5485-evaluation-board:M5485EVB

- M5484LITE (a.k.a. Zoom ColdFire LITEKIT)
http://www.nxp.com/pages/mcf5484-lite-evaluation-kit:M5484LITE

Prerequisites:
On your EVB, you need Freescale's dBUG command-line tool. By default, at startup
the EVB boots straight to the dBUG prompt. If you lost the dBUG tool, you can
download it there and flash it on your EVB:
http://cache.nxp.com/files/32bit/software/app_software/M547X-8XEVB-DBBIN.zip

Requirements:
- An RS-232 connection between the EVB and a controlling computer.
- On the computer, an ANSI terminal emulator connected to the RS-232 port.
- An Ethernet connection between the EVB and your network.
- A TFTP server running somewhere on your network (can be the same computer).
- Optional: a CompactFlash card with a FAT16 partition.

To use the CompactFlash card, you need to have programmed the CPLD
(programmable component) on your board. You need to contact Logic PD
(the board manufacturer) to get the appropriate software package.
https://support.logicpd.com/

How to run EmuTOS:

1) Ensure that your serial connection works fine. In the terminal, you should
see the dBUG prompt.

2) Ensure that your TCP/IP connection is properly configured, on both sides.

3) Put emutos-m548x-dbug.s19 at the root of your TFTP server.

4) In the terminal, type the following command to download the binary:
dn emutos-m548x-dbug.s19

5) Then type the following command to run EmuTOS:
go

6) You will see the EmuTOS welcome screen.
Press C to enter the early console.

7) If you wait too much, the desktop will run in the background.
Press Control+Z to return to EmuCON.

8) Then you can run EmuCON commands, and ColdFire text mode TOS programs from
the CompactFlash card.

Restrictions:

- No graphical display, no keyboard, no mouse.
Only text I/O through an RS/232 terminal.

- Support for ColdFire TOS programs only. 680x0 programs will not work.

- No Atari hardware emulated. Clean programs using only the OS will work,
the ones trying direct access to the Atari hardware will not work.

This ROM image has been built using:
make m548x-dbug

