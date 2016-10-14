/*
 * coldfire.c - ColdFire specific functions
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef __mcoldfire__
#error This file must only be compiled on ColdFire targets
#endif

/* #define ENABLE_KDEBUG */
#define DEBUG_FLEXCAN 0

#include "config.h"
#include "portab.h"
#include "coldfire.h"
#include "coldpriv.h"
#include "tosvars.h"
#include "ikbd.h"
#include "string.h"
#include "kprint.h"
#include "delay.h"
#include "asm.h"

#if DEBUG_FLEXCAN
static void flexcan_dump_registers(void);
#endif

void coldfire_early_init(void)
{
#if defined(MACHINE_M548X) && CONF_WITH_IDE && !CONF_WITH_BAS_MEMORY_MAP
    m548x_init_cpld();
#endif
}

#if CONF_WITH_COLDFIRE_RS232

#define RS232_UART_PORT 0 /* PSC channel used as terminal */

BOOL coldfire_rs232_can_write(void)
{
    /* Check if space is available in the FIFO */
    return MCF_UART_USR(RS232_UART_PORT) & MCF_UART_USR_TXRDY;
}

void coldfire_rs232_write_byte(UBYTE b)
{
    while (!coldfire_rs232_can_write())
    {
        /* Wait */
    }

    /* Send the byte */
    MCF_UART_UTB(RS232_UART_PORT) = (UBYTE)b;
}

BOOL coldfire_rs232_can_read(void)
{
    /* Wait until a byte is received */
    return MCF_UART_USR(RS232_UART_PORT) & MCF_UART_USR_RXRDY;
}

UBYTE coldfire_rs232_read_byte(void)
{
    /* Wait until character has been received */
    while (!coldfire_rs232_can_read())
    {
        /* Wait */
    }

    /* Read the received byte */
    return MCF_UART_URB(RS232_UART_PORT);
}

#endif /* CONF_WITH_COLDFIRE_RS232 */

#if CONF_COLDFIRE_TIMER_C

void coldfire_init_system_timer(void)
{
    /* Disable the timer before configuration */
    MCF_GPT1_GMS = MCF_GPT_GMS_TMS(0UL);

    /* Set the interrupt handler */
    INTERRUPT_VECTOR(61) = coldfire_int_61;

    /* Interrupt priority.
     * Never assign the same Level and Priority to several interrupts,
     * otherwise an exception 127 will occur on RTE! */
    MCF_INTC_ICR61 = MCF_INTC_ICR_IL(0x6UL) | /* Level */
                     MCF_INTC_ICR_IP(0x0UL);  /* Priority within the level */

    /* Enable the reception of the interrupt */
    MCF_INTC_IMRH &= ~MCF_INTC_IMRH_INT_MASK61;

    /* Set the frequency to 200 Hz (SDCLK / PRE / CNT) */
#ifdef SDCLK_FREQUENCY_MHZ
    MCF_GPT1_GCIR = MCF_GPT_GCIR_PRE(SDCLK_FREQUENCY_MHZ) |
                    MCF_GPT_GCIR_CNT(5000UL);
#else
# error Unknown SDCLK for this machine
#endif

    /* Enable the timer */
    MCF_GPT1_GMS = MCF_GPT_GMS_CE       | /* Enable */
                   MCF_GPT_GMS_SC       | /* Continuous mode */
                   MCF_GPT_GMS_IEN      | /* Interrupt enable */
                   MCF_GPT_GMS_TMS(4UL);  /* Internal timer */
}

#endif /* CONF_COLDFIRE_TIMER_C */

#ifdef MACHINE_M548X

const char* m548x_machine_name(void)
{
    /* Guess the board type from the CPU model */
    switch (MCF_SIU_JTAGID & MCF_SIU_JTAGID_PROCESSOR)
    {
        case MCF_SIU_JTAGID_MCF5484:
            return "M5484LITE";

        case MCF_SIU_JTAGID_MCF5485:
            return "M5485EVB";

        default:
            return "M548????";
    }
}

#if CONF_WITH_IDE && !CONF_WITH_BAS_MEMORY_MAP

void m548x_init_cpld(void)
{
    MCF_FBCS4_CSAR = MCF_FBCS_CSAR_BA(FIRE_ENGINE_CS4_BASE);
    MCF_FBCS4_CSCR = FIRE_ENGINE_CS4_ACCESS;
    MCF_FBCS4_CSMR = MCF_FBCS_CSMR_BAM_256M + MCF_FBCS_CSMR_V;

    MCF_FBCS5_CSAR = MCF_FBCS_CSAR_BA(FIRE_ENGINE_CS5_BASE);
    MCF_FBCS5_CSCR = FIRE_ENGINE_CS5_ACCESS;
    MCF_FBCS5_CSMR = MCF_FBCS_CSMR_BAM_256M + MCF_FBCS_CSMR_V;

    *(volatile UWORD*)(FIRE_ENGINE_CS5_BASE + 0x1000000) |= 0x8000;
}

#endif /* CONF_WITH_IDE && !CONF_WITH_BAS_MEMORY_MAP */

#endif /* MACHINE_M548X */

#ifdef MACHINE_FIREBEE

BOOL firebee_pic_can_write(void)
{
    /* Check if space is available in the FIFO */
    return MCF_UART_USR3 & MCF_UART_USR_TXRDY;
}

void firebee_pic_write_byte(UBYTE b)
{
    while (!firebee_pic_can_write())
    {
        /* Wait */
    }

    /* Send the byte */
    MCF_UART_UTB3 = b;
}

void firebee_shutdown(void)
{
    firebee_pic_write_byte(0x0c);
    firebee_pic_write_byte('O');
    firebee_pic_write_byte('F');
    firebee_pic_write_byte('F');
}

#endif /* MACHINE_FIREBEE */

#if CONF_SERIAL_CONSOLE

void coldfire_rs232_enable_interrupt(void)
{
    /* We assume that the RS-232 port has already been configured.
     * Here, we just enable the interrupt on data reception. */
#if RS232_UART_PORT != 0
# error We only support RS232_UART_PORT == 0
#endif
    INTERRUPT_VECTOR(35) = coldfire_int_35;

    /* Interrupt priority.
     * Never assign the same Level and Priority to several interrupts,
     * otherwise an exception 127 will occur on RTE! */
    MCF_INTC_ICR35 = MCF_INTC_ICR_IL(0x6UL) | /* Level */
                     MCF_INTC_ICR_IP(0x1UL);  /* Priority within the level */

    /* Enable the interrupt when data is available */
    MCF_PSC0_PSCIMR = MCF_PSC_PSCIMR_RXRDY_FU;

    /* Allow the reception of the interrupt */
    MCF_INTC_IMRH &= ~MCF_INTC_IMRH_INT_MASK35;
}

/* Called from assembler routine coldfire_int_35 */
void coldfire_rs232_interrupt_handler(void)
{
    UBYTE ascii;

    /* While there are pending bytes */
    while (MCF_UART_USR(RS232_UART_PORT) & MCF_UART_USR_RXRDY)
    {
        /* Read the ASCII character */
        ascii = MCF_UART_URB(RS232_UART_PORT);

        /* And append a new IOREC value into the IKBD buffer */
        push_ascii_ikbdiorec(ascii);

#if DEBUG_FLEXCAN
        /* Dump FlexCAN registers when Return is typed on the serial console */
        if (ascii == '\r')
            flexcan_dump_registers();
#endif
    }
}

#endif /* CONF_SERIAL_CONSOLE */

MCF_COOKIE cookie_mcf;

void setvalue_mcf(void)
{
    cookie_mcf.magic[0] = 0x4d; /* 'M' */
    cookie_mcf.magic[1] = 0x43; /* 'C' */
    cookie_mcf.magic[2] = 0x46; /* 'F' */
    cookie_mcf.version = COOKIE_MCF_VERSION;
    cookie_mcf.core = MCF_V4;
    cookie_mcf.revision = MCF_VALUE_UNKNOWN;
    cookie_mcf.units = MCF_UNITS_DIV | MCF_UNITS_EMAC | MCF_UNITS_FPU | MCF_UNITS_MMU;
    cookie_mcf.isa = MCF_ISA_B;
    cookie_mcf.debug = MCF_DEBUG_D;

#ifdef MACHINE_FIREBEE
    strcpy(cookie_mcf.device_name, "MCF5474");
    cookie_mcf.sysbus_frequency = 132;
#else
    switch (MCF_SIU_JTAGID & MCF_SIU_JTAGID_PROCESSOR)
    {
        case MCF_SIU_JTAGID_MCF5484:
            strcpy(cookie_mcf.device_name, "MCF5484");
            /* If a MCF5484 we guess it is a LITE board */
            cookie_mcf.sysbus_frequency = 100;
            break;
        case MCF_SIU_JTAGID_MCF5485:
            strcpy(cookie_mcf.device_name, "MCF5485");
            /* If a MCF5485 we guess it is a EVB board */
            cookie_mcf.sysbus_frequency = 133;
            break;
        default:
            strcpy(cookie_mcf.device_name, "UNKNOWN");
            cookie_mcf.sysbus_frequency = MCF_VALUE_UNKNOWN;
            break;
    }
#endif
}

#if CONF_WITH_FLEXCAN

#if DEBUG_FLEXCAN
static void flexcan_dump_registers(void)
{
    ULONG errstat1 = MCF_CAN_ERRSTAT1;

    KDEBUG(("CANMCR1 = 0x%08lx\n", MCF_CAN_CANMCR1));
    KDEBUG(("CANCTRL1 = 0x%08lx\n", MCF_CAN_CANCTRL1));
    KDEBUG(("TIMER1 = 0x%08lx\n", MCF_CAN_TIMER1));
    KDEBUG(("RXGMASK1 = 0x%08lx\n", MCF_CAN_RXGMASK1));
    KDEBUG(("RX14MASK1 = 0x%08lx\n", MCF_CAN_RX14MASK1));
    KDEBUG(("RX15MASK1 = 0x%08lx\n", MCF_CAN_RX15MASK1));
    KDEBUG(("ERRCNT1 = 0x%08lx\n", MCF_CAN_ERRCNT1));
    KDEBUG(("ERRSTAT1 = 0x%08lx\n", errstat1));
    KDEBUG(("IMASK1 = 0x%04x\n", MCF_CAN_IMASK1));
    KDEBUG(("IFLAG1 = 0x%04x\n", MCF_CAN_IFLAG1));

    if (errstat1 & MCF_CAN_ERRSTAT_ERRINT)
    {
        /* FIXME: When plugged, the Eiffel device continuously sends error
         * frames on the bus. This looks like an Eiffel firmware bug. */
        KDEBUG(("Warning: ERRINT=1 BITERR=%d%d ACKERR=%d CRCERR=%d FRMERR=%d STFERR=%d\n",
            !!(errstat1 & 0x00008000), /* BITERR high */
            !!(errstat1 & 0x00004000), /* BITERR low */
            !!(errstat1 & MCF_CAN_ERRSTAT_ACKERR),
            !!(errstat1 & MCF_CAN_ERRSTAT_CRCERR),
            !!(errstat1 & MCF_CAN_ERRSTAT_FRMERR),
            !!(errstat1 & MCF_CAN_ERRSTAT_STFERR)
        ));
    }
}
#endif /* DEBUG_FLEXCAN */

/*
 * Initialize the FlexCAN interface.
 * An Eiffel 2.0 adapter may be present on the bus, to simulate Atari IKBD
 * devices (keyboard, mouse, joystick) from PS/2 keyboard and mouse.
 * On ColdFire evaluation boards, the physical CAN connector is wired
 * to the FlexCAN 1 module. FlexCAN 0 is not used.
 * Message buffer 0 is configured to transfer messages.
 * Message buffer 15 is configured to receive messages.
 */
 void coldfire_init_flexcan(void)
{
    int i;

    KDEBUG(("coldfire_init_flexcan()\n"));

    /* Enable the CAN1 clock in the PLL.
     * Even if all clocks are enabled by default at power-on, BaS_gcc might
     * have disabled the unused ones to reduce power consumption. */
    MCF_CLOCK_SPCR |= MCF_CLOCK_SPCR_CAN1EN;

    /* Reset FlexCAN 1. This matters on warm boot, to clear errors, etc. */
    MCF_CAN_CANMCR1 = MCF_CAN_CANMCR_SOFTRST;
    while (MCF_CAN_CANMCR1 & MCF_CAN_CANMCR_SOFTRST); /* Wait */

    /* On ColdFire, the CANRX1/CANTX1 signals are mapped to several pins. See
     * MCF5485RM.pdf, table 15-2. For normal operation, TIN2/TOUT2 pins of the
     * Timer Module must be configured for FlexCAN 1, while other ones must be
     * configured for GPIO to prevent unexpected behaviour. */

    /* Configure IRQ6/IRQ5 pins for GPIO instead of FlexCAN 1 */
    MCF_PAD_PAR_FECI2CIRQ |= MCF_PAD_PAR_FECI2CIRQ_PAR_IRQ5 |
                             MCF_PAD_PAR_FECI2CIRQ_PAR_IRQ6;

    /* Configure DSPICS3/DSPICS2 pins for GPIO instead of FlexCAN 1 */
    MCF_PAD_PAR_DSPI &= ~(MCF_PAD_PAR_DSPI_PAR_CS3_DSPICS3 |
                          MCF_PAD_PAR_DSPI_PAR_CS2_DSPICS2);

    /* Configure TIN3/TOUT3 pins for GPIO, and TIN2/TOUT2 for FlexCAN 1 */
    MCF_PAD_PAR_TIMER = MCF_PAD_PAR_TIMER_PAR_TIN3_TIN3 |
                        MCF_PAD_PAR_TIMER_PAR_TOUT3;

    /* Set bit rate to 250 kHz */
    MCF_CAN_CANCTRL1 = MCF_CAN_CANCTRL_PRESDIV(0x18) |
                       MCF_CAN_CANCTRL_PROPSEG(2) |
                       MCF_CAN_CANCTRL_PSEG1(7) |
                       MCF_CAN_CANCTRL_PSEG2(3) |
                       MCF_CAN_CANCTRL_SAMP;

    /* Set Rx Mask Registers to "all identifier bits must match" */
    MCF_CAN_RXGMASK1 = MCF_CAN_RXGMASK_MI(0x1fffffff); /* Buffers 0-13 */
    MCF_CAN_RX14MASK1 = MCF_CAN_RX14MASK_MI(0x1fffffff); /* Buffer 14 */
    MCF_CAN_RX15MASK1 = MCF_CAN_RX15MASK_MI(0x1fffffff); /* Buffer 15 */

    /* Disable all message buffers */
    for (i = 0; i < 16; i++)
        MCF_CAN_MBUF_CTRL(1, i) = MCF_CAN_MBUF_CTRL_CODE(MBOX_RXCODE_NOT_ACTIVE);

    /* Configure Message Buffer 15 for reception */
    MCF_CAN_MBUF_ID(1, 15) = MCF_CAN_MBUF_ID_STD(0x181); /* Identifier of frames sent by Eiffel */
    MCF_CAN_MBUF_CTRL(1, 15) = MCF_CAN_MBUF_CTRL_CODE(MBOX_RXCODE_EMPTY);

    /* Setup the interrupt vector */
    INTERRUPT_VECTOR(57) = coldfire_int_57;

    /* Interrupt priority.
     * Never assign the same Level and Priority to several interrupts,
     * otherwise an exception 127 will occur on RTE! */
    MCF_INTC_ICR57 = MCF_INTC_ICR_IL(0x6UL) | /* Level */
                     MCF_INTC_ICR_IP(0x4UL);  /* Priority within the level */

    /* Enable interrupt for Message Buffer 15 */
    MCF_CAN_IMASK1 = MCF_CAN_IMASK_BUF15M;

    /* Start FlexCAN 1 */
    MCF_CAN_CANMCR1 = MCF_CAN_CANMCR_FRZ |
                      MCF_CAN_CANMCR_SUPV |
                      MCF_CAN_CANMCR_MAXMB(15);

    /* Allow reception of the interrupt */
    MCF_INTC_IMRH &= ~MCF_INTC_IMRH_INT_MASK57;

#if DEBUG_FLEXCAN
    /* Dump FlexCAN registers before startup */
    flexcan_dump_registers();
#endif
}

/* Called from assembler routine coldfire_int_57 */
void coldfire_flexcan_message_buffer_interrupt(void)
{
#if DEBUG_FLEXCAN
    KDEBUG(("*** FlexCAN message buffer interrupt!\n"));
#endif

    /* Handle interrupts from message buffer 15.
     * Read the received message and forward data bytes to IKBD handler. */
    if (MCF_CAN_IFLAG1 & MCF_CAN_IMASK_BUF15M)
    {
        UWORD status;
        int length;
        int i;

#if DEBUG_FLEXCAN
        flexcan_dump_registers();
#endif

        /* Lock the message buffer.
         * This is achieved by reading the control/status word. */
        status = MCF_CAN_MBUF_CTRL(1, 15);
        length = status & 0x000f;

        /* Check for overrun */
        if ((status & 0x0f00) == MCF_CAN_MBUF_CTRL_CODE(MBOX_RXCODE_OVERRUN))
            KDEBUG(("FlexCAN Overrun!\n"));

#if DEBUG_FLEXCAN
        KDEBUG(("status = 0x%04x\n", status));
        KDEBUG(("timestamp = 0x%04x\n", MCF_CAN_MBUF_TMSTP(1, 15)));
        KDEBUG(("id = 0x%08lx\n", MCF_CAN_MBUF_ID(1, 15)));
        KDEBUG(("stdid = 0x%lx\n", (MCF_CAN_MBUF_ID(1, 15) & 0x1ffc0000) >> 18));
        KDEBUG(("length = %d\n", length));
#endif

        /* Read the frame data */
        for (i = 0; i < length; i++)
        {
            vuint8* data = &MCF_CAN_MBUF_BYTE0(1, 15);
            UBYTE b = data[i];
#if DEBUG_FLEXCAN
            KDEBUG(("received ikbd byte = 0x%02x\n", b));
#endif
            call_ikbdraw(b);
        }

        /* Unlock the message buffer.
         * This is achieved by reading the free-running timer. */
        UNUSED(MCF_CAN_TIMER1);

        /* Clear the interrupt */
        MCF_CAN_IFLAG1 = MCF_CAN_IFLAG_BUF15I;
    }
}

/* Send a byte to Eiffel */
void coldfire_flexcan_ikbd_writeb(UBYTE b)
{
    ULONG i;

    /* Set up Message Buffer 0 for transmission */
    MCF_CAN_MBUF_CTRL(1, 0) = MCF_CAN_MBUF_CTRL_CODE(MBOX_TXCODE_NOT_READY);
    MCF_CAN_MBUF_ID(1, 0) = MCF_CAN_MBUF_ID_STD(0x201); /* Identifier of frames to send to Eiffel */
    MCF_CAN_MBUF_BYTE0(1, 0) = b;
    MCF_CAN_MBUF_CTRL(1, 0) = MCF_CAN_MBUF_CTRL_CODE(MBOX_TXCODE_TRANSMIT) |
                              MCF_CAN_MBUF_CTRL_LENGTH(1);

    /* If no Eiffel is plugged, the transmission will never complete.
     * So we need to handle a timeout here.
     * Even a rough approximation will be enough. */
    for (i = 0; i < loopcount_1_msec * 10; i++)
    {
        /* Check for interrupt */
        if (MCF_CAN_IFLAG1 & MCF_CAN_IFLAG_BUF0I)
        {
            /* Clear the interrupt flag */
            MCF_CAN_IFLAG1 = MCF_CAN_IFLAG_BUF0I;

            /* Success */
            return;
        }
    }

    /* Timeout */
}

#endif /* CONF_WITH_FLEXCAN */
