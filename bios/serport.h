/*
 * serport.h - header for serport.c
 *
 * Copyright (c) 2012 EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _SERPORT_H
#define _SERPORT_H

#include "portab.h"

/*
 * baud rate codes
 */
#define B19200 0
#define B9600  1
#define B4800  2 
#define B3600  3 
#define B2400  4
#define B2000  5
#define B1800  6
#define B1200  7
#define B600   8
#define B300   9
#define B200   10
#define B150   11
#define B134   12
#define B110   13
#define B75    14
#define B50    15

#define MIN_BAUDRATE_CODE   B19200
#define MAX_BAUDRATE_CODE   B50

/*
 * flow control codes
 */
#define FLOW_CTRL_NONE  0    /* no flow control */
#define FLOW_CTRL_SOFT  1    /* software flow control (XON/XOFF) */
#define FLOW_CTRL_HARD  2    /* hardware flow control (RTS/CTS) */
#define FLOW_CTRL_BOTH  3    /* XON/XOFF and RTS/CTS */

#define MIN_FLOW_CTRL   FLOW_CTRL_NONE
#define MAX_FLOW_CTRL   FLOW_CTRL_BOTH

/*
 * function prototypes
 */
void init_serport(void);
ULONG rsconf(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);

#endif
