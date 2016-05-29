/*
 * coldfire.h - ColdFire specific functions
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef COLDFIRE_H
#define COLDFIRE_H

#ifdef __mcoldfire__

#if CONF_WITH_COLDFIRE_RS232
BOOL coldfire_rs232_can_write(void);
void coldfire_rs232_write_byte(UBYTE b);
BOOL coldfire_rs232_can_read(void);
UBYTE coldfire_rs232_read_byte(void);
#endif

#if CONF_COLDFIRE_TIMER_C
void coldfire_init_system_timer(void);
void coldfire_int_61(void); /* In coldfire2.S */
#endif

#ifdef MACHINE_M548X
const char* m548x_machine_name(void);
# if CONF_WITH_IDE && !CONF_WITH_BAS_MEMORY_MAP
void m548x_init_cpld(void);
# endif
#endif /* MACHINE_M548X */

#ifdef MACHINE_FIREBEE
BOOL firebee_pic_can_write(void);
void firebee_pic_write_byte(UBYTE b);
void firebee_shutdown(void);
#endif /* MACHINE_FIREBEE */

#if CONF_SERIAL_CONSOLE
void coldfire_rs232_enable_interrupt(void);
void coldfire_int_35(void); /* In coldfire2.S */
#endif /* CONF_SERIAL_CONSOLE */

/*
 *  Structure pointed by the '_MCF' cookie's value
 *
 *  Values defined below taken from ColdFire Family Programmer's Reference
 *  Manual (CFPRM). Section 1.10 Hardware Configuration Information.
 *
 *  The struct's member "device_name" is the "device identification number" as
 *  specified in the different families reference manuals, in sections describing
 *  the registers SDID, CIR, DEVICEID or JTAGID (depending on the CF family).
 *  For example: MCF5474, MCF5485, MCF54455, etc ...
 */

/* ColdFire core version */
#define MCF_V1  1
#define MCF_V2  2
#define MCF_V3  3
#define MCF_V4  4
#define MCF_V5  5

/* Instruction-Set Architecture (ISA) revision level */
#define MCF_ISA_A       0
#define MCF_ISA_B       1
#define MCF_ISA_C       2
#define MCF_ISA_A_PLUS  8

/* Debug module revision number */
#define MCF_DEBUG_A             0
#define MCF_DEBUG_B             1
#define MCF_DEBUG_C             2
#define MCF_DEBUG_D             3
#define MCF_DEBUG_E             4
#define MCF_DEBUG_B_PLUS        9
#define MCF_DEBUG_D_PLUS        11
#define MCF_DEBUG_D_PLUS_PST    15

/* Bit mask for units, set when present */
#define MCF_UNITS_MAC           1
#define MCF_UNITS_DIV           2
#define MCF_UNITS_EMAC          4
#define MCF_UNITS_FPU           8
#define MCF_UNITS_MMU           16

/*
 * To use with the struct members: version, core,
 * revision, isa, debug and sysbus_frequency
 * In the bitmask set to 0 when unknown.
 */
#define MCF_VALUE_UNKNOWN     -1

typedef struct {
    BYTE magic[3];            /* Magic number 0x4d4346 (MCF), identifies this struct */
#define COOKIE_MCF_VERSION    1
    UBYTE version;            /* This struct version */
    BYTE device_name[16];     /* Device identification number, null terminated */
    BYTE core;                /* ColdFire core version number */
    BYTE revision;            /* Processor revision number */
    ULONG units;              /* Bit mask. b0: MAC, b1: DIV, b2: EMAC, b3: FPU, b4: MMU */
    BYTE isa;                 /* Instruction-Set Architecture (ISA) revision level */
    BYTE debug;               /* Debug module revision */
    WORD sysbus_frequency;    /* System bus frequency in Mhz */
} MCF_COOKIE;

extern MCF_COOKIE cookie_mcf;

void setvalue_mcf(void);

#if CONF_WITH_FLEXCAN
void coldfire_init_flexcan(void);
void coldfire_flexcan_ikbd_writeb(UBYTE b);
void coldfire_int_57(void); /* In coldfire2.S */
void call_ikbdraw(UBYTE b); /* In coldfire2.S */
#endif /* CONF_WITH_FLEXCAN */

#endif /* __mcoldfire__ */

#endif /* COLDFIRE_H */
