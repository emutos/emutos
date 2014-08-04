/*
 * coldfire.c - ColdFire specific functions
 *
 * Copyright (c) 2013-2014 The EmuTOS development team
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
#include "tosvars.h"

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
