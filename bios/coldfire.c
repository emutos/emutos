/*
 * coldfire.c - ColdFire specific functions
 *
 * Copyright (c) 2013 The EmuTOS development team
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

#include "config.h"
#include "portab.h"
#include "coldfire.h"
#include "coldpriv.h"

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

    /* Interrupt priority */
    MCF_INTC_ICR61 = MCF_INTC_ICR_IL(0x6UL) | /* Level */
                     MCF_INTC_ICR_IP(0x0UL);  /* Priority within the level */

    /* Enable the reception of the interrupt */
    MCF_INTC_IMRH &= ~MCF_INTC_IMRH_INT_MASK61;

    /* Set the frequency to 200 Hz (SDCLK / PRE / CNT) */
#ifdef MACHINE_FIREBEE
    /* SDCLK = 132.00 MHz */
    MCF_GPT1_GCIR = MCF_GPT_GCIR_PRE(132UL) |
                    MCF_GPT_GCIR_CNT(5000UL);
#elif defined (MACHINE_M548X)
    /* SDCLK = 100.00 MHz */
    MCF_GPT1_GCIR = MCF_GPT_GCIR_PRE(100UL) |
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
