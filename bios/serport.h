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
#include "iorec.h"

#if CONF_WITH_SCC || CONF_WITH_TT_MFP
#define BCONMAP_AVAILABLE 1
#else
#define BCONMAP_AVAILABLE 0
#endif

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
 * Bconmap() stuff
 */
typedef struct {
    IOREC in;
    IOREC out;
    char baudrate;
    char flowctrl;  	/* TODO, flow control */
} EXT_IOREC;

typedef struct {        /* one per mappable device */
    LONG (*Bconstat)();
    LONG (*Bconin)();
    LONG (*Bcostat)();
    LONG (*Bconout)(WORD,WORD);
    ULONG (*Rsconf)(WORD,WORD,WORD,WORD,WORD,WORD);
    EXT_IOREC *Iorec;   /* points to IOREC and extended IOREC */
} MAPTAB;

typedef struct {
    MAPTAB *maptab;
    WORD maptabsize;
    WORD mapped_device; /* device currently mapped to device 1 */
} BCONMAP;

#define BCONMAP_START_HANDLE    6

/*
 * external references
 */
extern BCONMAP bconmap_root;
extern ULONG (*rsconfptr)(WORD,WORD,WORD,WORD,WORD,WORD);
extern EXT_IOREC *rs232iorecptr;

/*
 * function prototypes
 */
void init_serport(void);
LONG bconstat1(void);
LONG bconin1(void);
LONG bcostat1(void);
LONG bconout1(WORD,WORD);
ULONG rsconf1(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);
LONG bconmap(WORD);

#if BCONMAP_AVAILABLE
LONG bconstatA(void);
LONG bconinA(void);
LONG bcostatA(void);
LONG bconoutA(WORD,WORD);
LONG bconstatB(void);
LONG bconinB(void);
LONG bcostatB(void);
LONG bconoutB(WORD,WORD);
LONG bconstatTT(void);
LONG bconinTT(void);
LONG bcostatTT(void);
LONG bconoutTT(WORD,WORD);
ULONG rsconfA(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);
ULONG rsconfB(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);
ULONG rsconfTT(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);
ULONG rsconf_dummy(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr);
#endif

#endif  /* _SERPORT_H */
