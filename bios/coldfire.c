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
