/*
 * coldpriv.h - ColdFire private macros and definitions
 *
 * The contents of this file come from dBUG includes.
 * Originally written by Freescale Semiconductor, Inc.
 * No copyright or license was included in the original sources.
 */

#ifndef __mcoldfire__
#error This file must only be compiled on ColdFire targets
#endif

#ifndef COLDPRIV_H
#define COLDPRIV_H

/* Compatibility types */
typedef volatile unsigned char vuint8;
typedef volatile unsigned short vuint16;
typedef volatile unsigned long vuint32;

#if CONF_WITH_BAS_MEMORY_MAP
# define __MBAR ((vuint8*)0xff000000)
# define __RAMBAR0 ((vuint8*)0xff100000)
# define __VBR __RAMBAR0
#else
/* dBUG memory map */
# define __MBAR ((vuint8*)0x10000000)
# define __VBR ((vuint8*)0x00000000)
#endif

#define EXCEPTION_VECTOR(n) (*(void* volatile *)(__VBR + (n)*4))
#define INTERRUPT_VECTOR(n) EXCEPTION_VECTOR(64 + (n))

/*********************************************************************
*
* Programmable Serial Controller (UART Compatible Definitions) (UART)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_UART_UMR0        (*(vuint8 *)(void*)(&__MBAR[0x008600]))
#define MCF_UART_USR0        (*(vuint8 *)(void*)(&__MBAR[0x008604]))
#define MCF_UART_UCSR0       (*(vuint8 *)(void*)(&__MBAR[0x008604]))
#define MCF_UART_UCR0        (*(vuint8 *)(void*)(&__MBAR[0x008608]))
#define MCF_UART_URB0        (*(vuint8 *)(void*)(&__MBAR[0x00860C]))
#define MCF_UART_UTB0        (*(vuint8 *)(void*)(&__MBAR[0x00860C]))
#define MCF_UART_UIPCR0      (*(vuint8 *)(void*)(&__MBAR[0x008610]))
#define MCF_UART_UACR0       (*(vuint8 *)(void*)(&__MBAR[0x008610]))
#define MCF_UART_UISR0       (*(vuint8 *)(void*)(&__MBAR[0x008614]))
#define MCF_UART_UIMR0       (*(vuint8 *)(void*)(&__MBAR[0x008614]))
#define MCF_UART_UBG10       (*(vuint8 *)(void*)(&__MBAR[0x008618]))
#define MCF_UART_UBG20       (*(vuint8 *)(void*)(&__MBAR[0x00861C]))
#define MCF_UART_UIP0        (*(vuint8 *)(void*)(&__MBAR[0x008634]))
#define MCF_UART_UOP10       (*(vuint8 *)(void*)(&__MBAR[0x008638]))
#define MCF_UART_UOP00       (*(vuint8 *)(void*)(&__MBAR[0x00863C]))
#define MCF_UART_UMR1        (*(vuint8 *)(void*)(&__MBAR[0x008700]))
#define MCF_UART_USR1        (*(vuint8 *)(void*)(&__MBAR[0x008704]))
#define MCF_UART_UCSR1       (*(vuint8 *)(void*)(&__MBAR[0x008704]))
#define MCF_UART_UCR1        (*(vuint8 *)(void*)(&__MBAR[0x008708]))
#define MCF_UART_URB1        (*(vuint8 *)(void*)(&__MBAR[0x00870C]))
#define MCF_UART_UTB1        (*(vuint8 *)(void*)(&__MBAR[0x00870C]))
#define MCF_UART_UIPCR1      (*(vuint8 *)(void*)(&__MBAR[0x008710]))
#define MCF_UART_UACR1       (*(vuint8 *)(void*)(&__MBAR[0x008710]))
#define MCF_UART_UISR1       (*(vuint8 *)(void*)(&__MBAR[0x008714]))
#define MCF_UART_UIMR1       (*(vuint8 *)(void*)(&__MBAR[0x008714]))
#define MCF_UART_UBG11       (*(vuint8 *)(void*)(&__MBAR[0x008718]))
#define MCF_UART_UBG21       (*(vuint8 *)(void*)(&__MBAR[0x00871C]))
#define MCF_UART_UIP1        (*(vuint8 *)(void*)(&__MBAR[0x008734]))
#define MCF_UART_UOP11       (*(vuint8 *)(void*)(&__MBAR[0x008738]))
#define MCF_UART_UOP01       (*(vuint8 *)(void*)(&__MBAR[0x00873C]))
#define MCF_UART_UMR2        (*(vuint8 *)(void*)(&__MBAR[0x008800]))
#define MCF_UART_USR2        (*(vuint8 *)(void*)(&__MBAR[0x008804]))
#define MCF_UART_UCSR2       (*(vuint8 *)(void*)(&__MBAR[0x008804]))
#define MCF_UART_UCR2        (*(vuint8 *)(void*)(&__MBAR[0x008808]))
#define MCF_UART_URB2        (*(vuint8 *)(void*)(&__MBAR[0x00880C]))
#define MCF_UART_UTB2        (*(vuint8 *)(void*)(&__MBAR[0x00880C]))
#define MCF_UART_UIPCR2      (*(vuint8 *)(void*)(&__MBAR[0x008810]))
#define MCF_UART_UACR2       (*(vuint8 *)(void*)(&__MBAR[0x008810]))
#define MCF_UART_UISR2       (*(vuint8 *)(void*)(&__MBAR[0x008814]))
#define MCF_UART_UIMR2       (*(vuint8 *)(void*)(&__MBAR[0x008814]))
#define MCF_UART_UBG12       (*(vuint8 *)(void*)(&__MBAR[0x008818]))
#define MCF_UART_UBG22       (*(vuint8 *)(void*)(&__MBAR[0x00881C]))
#define MCF_UART_UIP2        (*(vuint8 *)(void*)(&__MBAR[0x008834]))
#define MCF_UART_UOP12       (*(vuint8 *)(void*)(&__MBAR[0x008838]))
#define MCF_UART_UOP02       (*(vuint8 *)(void*)(&__MBAR[0x00883C]))
#define MCF_UART_UMR3        (*(vuint8 *)(void*)(&__MBAR[0x008900]))
#define MCF_UART_USR3        (*(vuint8 *)(void*)(&__MBAR[0x008904]))
#define MCF_UART_UCSR3       (*(vuint8 *)(void*)(&__MBAR[0x008904]))
#define MCF_UART_UCR3        (*(vuint8 *)(void*)(&__MBAR[0x008908]))
#define MCF_UART_URB3        (*(vuint8 *)(void*)(&__MBAR[0x00890C]))
#define MCF_UART_UTB3        (*(vuint8 *)(void*)(&__MBAR[0x00890C]))
#define MCF_UART_UIPCR3      (*(vuint8 *)(void*)(&__MBAR[0x008910]))
#define MCF_UART_UACR3       (*(vuint8 *)(void*)(&__MBAR[0x008910]))
#define MCF_UART_UISR3       (*(vuint8 *)(void*)(&__MBAR[0x008914]))
#define MCF_UART_UIMR3       (*(vuint8 *)(void*)(&__MBAR[0x008914]))
#define MCF_UART_UBG13       (*(vuint8 *)(void*)(&__MBAR[0x008918]))
#define MCF_UART_UBG23       (*(vuint8 *)(void*)(&__MBAR[0x00891C]))
#define MCF_UART_UIP3        (*(vuint8 *)(void*)(&__MBAR[0x008934]))
#define MCF_UART_UOP13       (*(vuint8 *)(void*)(&__MBAR[0x008938]))
#define MCF_UART_UOP03       (*(vuint8 *)(void*)(&__MBAR[0x00893C]))
#define MCF_UART_UMR(x)      (*(vuint8 *)(void*)(&__MBAR[0x008600+((x)*0x100)]))
#define MCF_UART_USR(x)      (*(vuint8 *)(void*)(&__MBAR[0x008604+((x)*0x100)]))
#define MCF_UART_UCSR(x)     (*(vuint8 *)(void*)(&__MBAR[0x008604+((x)*0x100)]))
#define MCF_UART_UCR(x)      (*(vuint8 *)(void*)(&__MBAR[0x008608+((x)*0x100)]))
#define MCF_UART_URB(x)      (*(vuint8 *)(void*)(&__MBAR[0x00860C+((x)*0x100)]))
#define MCF_UART_UTB(x)      (*(vuint8 *)(void*)(&__MBAR[0x00860C+((x)*0x100)]))
#define MCF_UART_UIPCR(x)    (*(vuint8 *)(void*)(&__MBAR[0x008610+((x)*0x100)]))
#define MCF_UART_UACR(x)     (*(vuint8 *)(void*)(&__MBAR[0x008610+((x)*0x100)]))
#define MCF_UART_UISR(x)     (*(vuint8 *)(void*)(&__MBAR[0x008614+((x)*0x100)]))
#define MCF_UART_UIMR(x)     (*(vuint8 *)(void*)(&__MBAR[0x008614+((x)*0x100)]))
#define MCF_UART_UBG1(x)     (*(vuint8 *)(void*)(&__MBAR[0x008618+((x)*0x100)]))
#define MCF_UART_UBG2(x)     (*(vuint8 *)(void*)(&__MBAR[0x00861C+((x)*0x100)]))
#define MCF_UART_UIP(x)      (*(vuint8 *)(void*)(&__MBAR[0x008634+((x)*0x100)]))
#define MCF_UART_UOP1(x)     (*(vuint8 *)(void*)(&__MBAR[0x008638+((x)*0x100)]))
#define MCF_UART_UOP0(x)     (*(vuint8 *)(void*)(&__MBAR[0x00863C+((x)*0x100)]))

/* Bit definitions and macros for MCF_UART_UMR */
#define MCF_UART_UMR_BC(x)              (((x)&0x03)<<0)
#define MCF_UART_UMR_PT                 (0x04)
#define MCF_UART_UMR_PM(x)              (((x)&0x03)<<3)
#define MCF_UART_UMR_ERR                (0x20)
#define MCF_UART_UMR_RXIRQ              (0x40)
#define MCF_UART_UMR_RXRTS              (0x80)
#define MCF_UART_UMR_SB(x)              (((x)&0x0F)<<0)
#define MCF_UART_UMR_TXCTS              (0x10)
#define MCF_UART_UMR_TXRTS              (0x20)
#define MCF_UART_UMR_CM(x)              (((x)&0x03)<<6)
#define MCF_UART_UMR_PM_MULTI_ADDR      (0x1C)
#define MCF_UART_UMR_PM_MULTI_DATA      (0x18)
#define MCF_UART_UMR_PM_NONE            (0x10)
#define MCF_UART_UMR_PM_FORCE_HI        (0x0C)
#define MCF_UART_UMR_PM_FORCE_LO        (0x08)
#define MCF_UART_UMR_PM_ODD             (0x04)
#define MCF_UART_UMR_PM_EVEN            (0x00)
#define MCF_UART_UMR_BC_5               (0x00)
#define MCF_UART_UMR_BC_6               (0x01)
#define MCF_UART_UMR_BC_7               (0x02)
#define MCF_UART_UMR_BC_8               (0x03)
#define MCF_UART_UMR_CM_NORMAL          (0x00)
#define MCF_UART_UMR_CM_ECHO            (0x40)
#define MCF_UART_UMR_CM_LOCAL_LOOP      (0x80)
#define MCF_UART_UMR_CM_REMOTE_LOOP     (0xC0)
#define MCF_UART_UMR_SB_STOP_BITS_1     (0x07)
#define MCF_UART_UMR_SB_STOP_BITS_15    (0x08)
#define MCF_UART_UMR_SB_STOP_BITS_2     (0x0F)

/* Bit definitions and macros for MCF_UART_USR */
#define MCF_UART_USR_RXRDY              (0x01)
#define MCF_UART_USR_FFULL              (0x02)
#define MCF_UART_USR_TXRDY              (0x04)
#define MCF_UART_USR_TXEMP              (0x08)
#define MCF_UART_USR_OE                 (0x10)
#define MCF_UART_USR_PE                 (0x20)
#define MCF_UART_USR_FE                 (0x40)
#define MCF_UART_USR_RB                 (0x80)

/* Bit definitions and macros for MCF_UART_UCSR */
#define MCF_UART_UCSR_TCS(x)            (((x)&0x0F)<<0)
#define MCF_UART_UCSR_RCS(x)            (((x)&0x0F)<<4)
#define MCF_UART_UCSR_RCS_SYS_CLK       (0xD0)
#define MCF_UART_UCSR_RCS_CTM16         (0xE0)
#define MCF_UART_UCSR_RCS_CTM           (0xF0)
#define MCF_UART_UCSR_TCS_SYS_CLK       (0x0D)
#define MCF_UART_UCSR_TCS_CTM16         (0x0E)
#define MCF_UART_UCSR_TCS_CTM           (0x0F)

/* Bit definitions and macros for MCF_UART_UCR */
#define MCF_UART_UCR_RXC(x)             (((x)&0x03)<<0)
#define MCF_UART_UCR_TXC(x)             (((x)&0x03)<<2)
#define MCF_UART_UCR_MISC(x)            (((x)&0x07)<<4)
#define MCF_UART_UCR_NONE               (0x00)
#define MCF_UART_UCR_STOP_BREAK         (0x70)
#define MCF_UART_UCR_START_BREAK        (0x60)
#define MCF_UART_UCR_BKCHGINT           (0x50)
#define MCF_UART_UCR_RESET_ERROR        (0x40)
#define MCF_UART_UCR_RESET_TX           (0x30)
#define MCF_UART_UCR_RESET_RX           (0x20)
#define MCF_UART_UCR_RESET_MR           (0x10)
#define MCF_UART_UCR_TX_DISABLED        (0x08)
#define MCF_UART_UCR_TX_ENABLED         (0x04)
#define MCF_UART_UCR_RX_DISABLED        (0x02)
#define MCF_UART_UCR_RX_ENABLED         (0x01)

/* Bit definitions and macros for MCF_UART_UIPCR */
#define MCF_UART_UIPCR_CTS              (0x01)
#define MCF_UART_UIPCR_COS              (0x10)

/* Bit definitions and macros for MCF_UART_UACR */
#define MCF_UART_UACR_IEC               (0x01)

/* Bit definitions and macros for MCF_UART_UISR */
#define MCF_UART_UISR_TXRDY             (0x01)
#define MCF_UART_UISR_RXRDY_FU          (0x02)
#define MCF_UART_UISR_DB                (0x04)
#define MCF_UART_UISR_RXFTO             (0x08)
#define MCF_UART_UISR_TXFIFO            (0x10)
#define MCF_UART_UISR_RXFIFO            (0x20)
#define MCF_UART_UISR_COS               (0x80)

/* Bit definitions and macros for MCF_UART_UIMR */
#define MCF_UART_UIMR_TXRDY             (0x01)
#define MCF_UART_UIMR_RXRDY_FU          (0x02)
#define MCF_UART_UIMR_DB                (0x04)
#define MCF_UART_UIMR_COS               (0x80)

/* Bit definitions and macros for MCF_UART_UIP */
#define MCF_UART_UIP_CTS                (0x01)

/* Bit definitions and macros for MCF_UART_UOP1 */
#define MCF_UART_UOP1_RTS               (0x01)

/* Bit definitions and macros for MCF_UART_UOP0 */
#define MCF_UART_UOP0_RTS               (0x01)

/*********************************************************************
*
* Common GPIO
*
*********************************************************************/

/* Register read/write macros */
#define MCF_PAD_PAR_FBCTL                    (*(vuint16*)(&__MBAR[0xA40]))
#define MCF_PAD_PAR_FBCS                     (*(vuint8 *)(&__MBAR[0xA42]))
#define MCF_PAD_PAR_DMA                      (*(vuint8 *)(&__MBAR[0xA43]))
#define MCF_PAD_PAR_FECI2CIRQ                (*(vuint16*)(&__MBAR[0xA44]))
#define MCF_PAD_PAR_PCIBG                    (*(vuint16*)(&__MBAR[0xA48]))
#define MCF_PAD_PAR_PCIBR                    (*(vuint16*)(&__MBAR[0xA4A]))
#define MCF_PAD_PAR_PSC3                     (*(vuint8 *)(&__MBAR[0xA4C]))
#define MCF_PAD_PAR_PSC2                     (*(vuint8 *)(&__MBAR[0xA4D]))
#define MCF_PAD_PAR_PSC1                     (*(vuint8 *)(&__MBAR[0xA4E]))
#define MCF_PAD_PAR_PSC0                     (*(vuint8 *)(&__MBAR[0xA4F]))
#define MCF_PAD_PAR_DSPI                     (*(vuint16*)(&__MBAR[0xA50]))
#define MCF_PAD_PAR_TIMER                    (*(vuint8 *)(&__MBAR[0xA52]))


/* Bit definitions and macros for MCF_PAD_PAR_FBCTL */
#define MCF_PAD_PAR_FBCTL_PAR_ALE(x)         (((x)&0x3)<<0)
#define MCF_PAD_PAR_FBCTL_PAR_ALE_GPIO       (0)
#define MCF_PAD_PAR_FBCTL_PAR_ALE_TBST       (0x2)
#define MCF_PAD_PAR_FBCTL_PAR_ALE_ALE        (0x3)
#define MCF_PAD_PAR_FBCTL_PAR_TA             (0x4)
#define MCF_PAD_PAR_FBCTL_PAR_RWB(x)         (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_FBCTL_PAR_RWB_GPIO       (0)
#define MCF_PAD_PAR_FBCTL_PAR_RWB_TBST       (0x20)
#define MCF_PAD_PAR_FBCTL_PAR_RWB_RW         (0x30)
#define MCF_PAD_PAR_FBCTL_PAR_OE             (0x40)
#define MCF_PAD_PAR_FBCTL_PAR_BWE0           (0x100)
#define MCF_PAD_PAR_FBCTL_PAR_BWE1           (0x400)
#define MCF_PAD_PAR_FBCTL_PAR_BWE2           (0x1000)
#define MCF_PAD_PAR_FBCTL_PAR_BWE3           (0x4000)

/* Bit definitions and macros for MCF_PAD_PAR_FBCS */
#define MCF_PAD_PAR_FBCS_PAR_CS1             (0x2)
#define MCF_PAD_PAR_FBCS_PAR_CS2             (0x4)
#define MCF_PAD_PAR_FBCS_PAR_CS3             (0x8)
#define MCF_PAD_PAR_FBCS_PAR_CS4             (0x10)
#define MCF_PAD_PAR_FBCS_PAR_CS5             (0x20)

/* Bit definitions and macros for MCF_PAD_PAR_DMA */
#define MCF_PAD_PAR_DMA_PAR_DREQ0(x)         (((x)&0x3)<<0)
#define MCF_PAD_PAR_DMA_PAR_DREQ0_GPIO       (0)
#define MCF_PAD_PAR_DMA_PAR_DREQ0_TIN0       (0x2)
#define MCF_PAD_PAR_DMA_PAR_DREQ0_DREQ0      (0x3)
#define MCF_PAD_PAR_DMA_PAR_DREQ1(x)         (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_DMA_PAR_DREQ1_GPIO       (0)
#define MCF_PAD_PAR_DMA_PAR_DREQ1_IRQ1       (0x4)
#define MCF_PAD_PAR_DMA_PAR_DREQ1_TIN1       (0x8)
#define MCF_PAD_PAR_DMA_PAR_DREQ1_DREQ1      (0xC)
#define MCF_PAD_PAR_DMA_PAR_DACK0(x)         (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_DMA_PAR_DACK0_GPIO       (0)
#define MCF_PAD_PAR_DMA_PAR_DACK0_TOUT0      (0x20)
#define MCF_PAD_PAR_DMA_PAR_DACK0_DACK0      (0x30)
#define MCF_PAD_PAR_DMA_PAR_DACK1(x)         (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_DMA_PAR_DACK1_GPIO       (0)
#define MCF_PAD_PAR_DMA_PAR_DACK1_TOUT1      (0x80)
#define MCF_PAD_PAR_DMA_PAR_DACK1_DACK1      (0xC0)

/* Bit definitions and macros for MCF_PAD_PAR_FECI2CIRQ */
#define MCF_PAD_PAR_FECI2CIRQ_PAR_IRQ5       (0x1)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_IRQ6       (0x2)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_SCL        (0x4)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_SDA        (0x8)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E1MDC(x)   (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E1MDC_SCL  (0x80)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E1MDC_E1MDC (0xC0)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E1MDIO(x)  (((x)&0x3)<<0x8)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E1MDIO_SDA (0x200)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E1MDIO_E1MDIO (0x300)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E1MII      (0x400)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E17        (0x800)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E0MDC      (0x1000)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E0MDIO     (0x2000)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E0MII      (0x4000)
#define MCF_PAD_PAR_FECI2CIRQ_PAR_E07        (0x8000)

/* Bit definitions and macros for MCF_PAD_PAR_PCIBG */
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG0(x)      (((x)&0x3)<<0)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG0_GPIO    (0)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG0_TOUT0   (0x2)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG0_PCIBG0  (0x3)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG1(x)      (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG1_GPIO    (0)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG1_TOUT1   (0x8)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG1_PCIBG1  (0xC)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG2(x)      (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG2_GPIO    (0)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG2_TOUT2   (0x20)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG2_PCIBG2  (0x30)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG3(x)      (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG3_GPIO    (0)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG3_TOUT3   (0x80)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG3_PCIBG3  (0xC0)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG4(x)      (((x)&0x3)<<0x8)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG4_GPIO    (0)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG4_TBST    (0x200)
#define MCF_PAD_PAR_PCIBG_PAR_PCIBG4_PCIBG4  (0x300)

/* Bit definitions and macros for MCF_PAD_PAR_PCIBR */
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR0(x)      (((x)&0x3)<<0)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR0_GPIO    (0)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR0_TIN0    (0x2)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR0_PCIBR0  (0x3)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR1(x)      (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR1_GPIO    (0)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR1_TIN1    (0x8)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR1_PCIBR1  (0xC)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR2(x)      (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR2_GPIO    (0)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR2_TIN2    (0x20)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR2_PCIBR2  (0x30)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR3(x)      (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR3_GPIO    (0)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR3_TIN3    (0x80)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR3_PCIBR3  (0xC0)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR4(x)      (((x)&0x3)<<0x8)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR4_GPIO    (0)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR4_IRQ4    (0x200)
#define MCF_PAD_PAR_PCIBR_PAR_PCIBR4_PCIBR4  (0x300)

/* Bit definitions and macros for MCF_PAD_PAR_PSC3 */
#define MCF_PAD_PAR_PSC3_PAR_TXD3            (0x4)
#define MCF_PAD_PAR_PSC3_PAR_RXD3            (0x8)
#define MCF_PAD_PAR_PSC3_PAR_RTS3(x)         (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_PSC3_PAR_RTS3_GPIO       (0)
#define MCF_PAD_PAR_PSC3_PAR_RTS3_FSYNC      (0x20)
#define MCF_PAD_PAR_PSC3_PAR_RTS3_RTS        (0x30)
#define MCF_PAD_PAR_PSC3_PAR_CTS3(x)         (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_PSC3_PAR_CTS3_GPIO       (0)
#define MCF_PAD_PAR_PSC3_PAR_CTS3_BCLK       (0x80)
#define MCF_PAD_PAR_PSC3_PAR_CTS3_CTS        (0xC0)

/* Bit definitions and macros for MCF_PAD_PAR_PSC2 */
#define MCF_PAD_PAR_PSC2_PAR_TXD2            (0x4)
#define MCF_PAD_PAR_PSC2_PAR_RXD2            (0x8)
#define MCF_PAD_PAR_PSC2_PAR_RTS2(x)         (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_PSC2_PAR_RTS2_GPIO       (0)
#define MCF_PAD_PAR_PSC2_PAR_RTS2_FSYNC      (0x20)
#define MCF_PAD_PAR_PSC2_PAR_RTS2_RTS        (0x30)
#define MCF_PAD_PAR_PSC2_PAR_CTS2(x)         (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_PSC2_PAR_CTS2_GPIO       (0)
#define MCF_PAD_PAR_PSC2_PAR_CTS2_BCLK       (0x80)
#define MCF_PAD_PAR_PSC2_PAR_CTS2_CTS        (0xC0)

/* Bit definitions and macros for MCF_PAD_PAR_PSC1 */
#define MCF_PAD_PAR_PSC1_PAR_TXD1            (0x4)
#define MCF_PAD_PAR_PSC1_PAR_RXD1            (0x8)
#define MCF_PAD_PAR_PSC1_PAR_RTS1(x)         (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_PSC1_PAR_RTS1_GPIO       (0)
#define MCF_PAD_PAR_PSC1_PAR_RTS1_FSYNC      (0x20)
#define MCF_PAD_PAR_PSC1_PAR_RTS1_RTS        (0x30)
#define MCF_PAD_PAR_PSC1_PAR_CTS1(x)         (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_PSC1_PAR_CTS1_GPIO       (0)
#define MCF_PAD_PAR_PSC1_PAR_CTS1_BCLK       (0x80)
#define MCF_PAD_PAR_PSC1_PAR_CTS1_CTS        (0xC0)

/* Bit definitions and macros for MCF_PAD_PAR_PSC0 */
#define MCF_PAD_PAR_PSC0_PAR_TXD0            (0x4)
#define MCF_PAD_PAR_PSC0_PAR_RXD0            (0x8)
#define MCF_PAD_PAR_PSC0_PAR_RTS0(x)         (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_PSC0_PAR_RTS0_GPIO       (0)
#define MCF_PAD_PAR_PSC0_PAR_RTS0_FSYNC      (0x20)
#define MCF_PAD_PAR_PSC0_PAR_RTS0_RTS        (0x30)
#define MCF_PAD_PAR_PSC0_PAR_CTS0(x)         (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_PSC0_PAR_CTS0_GPIO       (0)
#define MCF_PAD_PAR_PSC0_PAR_CTS0_BCLK       (0x80)
#define MCF_PAD_PAR_PSC0_PAR_CTS0_CTS        (0xC0)

/* Bit definitions and macros for MCF_PAD_PAR_DSPI */
#define MCF_PAD_PAR_DSPI_PAR_SOUT(x)         (((x)&0x3)<<0)
#define MCF_PAD_PAR_DSPI_PAR_SOUT_GPIO       (0)
#define MCF_PAD_PAR_DSPI_PAR_SOUT_TXD        (0x2)
#define MCF_PAD_PAR_DSPI_PAR_SOUT_SOUT       (0x3)
#define MCF_PAD_PAR_DSPI_PAR_SIN(x)          (((x)&0x3)<<0x2)
#define MCF_PAD_PAR_DSPI_PAR_SIN_GPIO        (0)
#define MCF_PAD_PAR_DSPI_PAR_SIN_RXD         (0x8)
#define MCF_PAD_PAR_DSPI_PAR_SIN_SIN         (0xC)
#define MCF_PAD_PAR_DSPI_PAR_SCK(x)          (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_DSPI_PAR_SCK_GPIO        (0)
#define MCF_PAD_PAR_DSPI_PAR_SCK_BCLK        (0x10)
#define MCF_PAD_PAR_DSPI_PAR_SCK_CTS         (0x20)
#define MCF_PAD_PAR_DSPI_PAR_SCK_SCK         (0x30)
#define MCF_PAD_PAR_DSPI_PAR_CS0(x)          (((x)&0x3)<<0x6)
#define MCF_PAD_PAR_DSPI_PAR_CS0_GPIO        (0)
#define MCF_PAD_PAR_DSPI_PAR_CS0_FSYNC       (0x40)
#define MCF_PAD_PAR_DSPI_PAR_CS0_RTS         (0x80)
#define MCF_PAD_PAR_DSPI_PAR_CS0_DSPICS0     (0xC0)
#define MCF_PAD_PAR_DSPI_PAR_CS2(x)          (((x)&0x3)<<0x8)
#define MCF_PAD_PAR_DSPI_PAR_CS2_GPIO        (0)
#define MCF_PAD_PAR_DSPI_PAR_CS2_TOUT2       (0x200)
#define MCF_PAD_PAR_DSPI_PAR_CS2_DSPICS2     (0x300)
#define MCF_PAD_PAR_DSPI_PAR_CS3(x)          (((x)&0x3)<<0xA)
#define MCF_PAD_PAR_DSPI_PAR_CS3_GPIO        (0)
#define MCF_PAD_PAR_DSPI_PAR_CS3_TOUT3       (0x800)
#define MCF_PAD_PAR_DSPI_PAR_CS3_DSPICS3     (0xC00)
#define MCF_PAD_PAR_DSPI_PAR_CS5             (0x1000)

/* Bit definitions and macros for MCF_PAD_PAR_TIMER */
#define MCF_PAD_PAR_TIMER_PAR_TOUT2          (0x1)
#define MCF_PAD_PAR_TIMER_PAR_TIN2(x)        (((x)&0x3)<<0x1)
#define MCF_PAD_PAR_TIMER_PAR_TIN2_IRQ2      (0x4)
#define MCF_PAD_PAR_TIMER_PAR_TIN2_TIN2      (0x6)
#define MCF_PAD_PAR_TIMER_PAR_TOUT3          (0x8)
#define MCF_PAD_PAR_TIMER_PAR_TIN3(x)        (((x)&0x3)<<0x4)
#define MCF_PAD_PAR_TIMER_PAR_TIN3_IRQ3      (0x20)
#define MCF_PAD_PAR_TIMER_PAR_TIN3_TIN3      (0x30)


/*********************************************************************
*
* General Purpose I/O (GPIO)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_GPIO_PODR_FBCTL                  (*(vuint8 *)(&__MBAR[0xA00]))
#define MCF_GPIO_PDDR_FBCTL                  (*(vuint8 *)(&__MBAR[0xA10]))
#define MCF_GPIO_PPDSDR_FBCTL                (*(vuint8 *)(&__MBAR[0xA20]))
#define MCF_GPIO_PCLRR_FBCTL                 (*(vuint8 *)(&__MBAR[0xA30]))

#define MCF_GPIO_PODR_FBCS                   (*(vuint8 *)(&__MBAR[0xA01]))
#define MCF_GPIO_PDDR_FBCS                   (*(vuint8 *)(&__MBAR[0xA11]))
#define MCF_GPIO_PPDSDR_FBCS                 (*(vuint8 *)(&__MBAR[0xA21]))
#define MCF_GPIO_PCLRR_FBCS                  (*(vuint8 *)(&__MBAR[0xA31]))

#define MCF_GPIO_PODR_DMA                    (*(vuint8 *)(&__MBAR[0xA02]))
#define MCF_GPIO_PDDR_DMA                    (*(vuint8 *)(&__MBAR[0xA12]))
#define MCF_GPIO_PPDSDR_DMA                  (*(vuint8 *)(&__MBAR[0xA22]))
#define MCF_GPIO_PCLRR_DMA                   (*(vuint8 *)(&__MBAR[0xA32]))

#define MCF_GPIO_PODR_FEC0H                  (*(vuint8 *)(&__MBAR[0xA04]))
#define MCF_GPIO_PDDR_FEC0H                  (*(vuint8 *)(&__MBAR[0xA14]))
#define MCF_GPIO_PPDSDR_FEC0H                (*(vuint8 *)(&__MBAR[0xA24]))
#define MCF_GPIO_PCLRR_FEC0H                 (*(vuint8 *)(&__MBAR[0xA34]))

#define MCF_GPIO_PODR_FEC0L                  (*(vuint8 *)(&__MBAR[0xA05]))
#define MCF_GPIO_PDDR_FEC0L                  (*(vuint8 *)(&__MBAR[0xA15]))
#define MCF_GPIO_PPDSDR_FEC0L                (*(vuint8 *)(&__MBAR[0xA25]))
#define MCF_GPIO_PCLRR_FEC0L                 (*(vuint8 *)(&__MBAR[0xA35]))

#define MCF_GPIO_PODR_FEC1H                  (*(vuint8 *)(&__MBAR[0xA06]))
#define MCF_GPIO_PDDR_FEC1H                  (*(vuint8 *)(&__MBAR[0xA16]))
#define MCF_GPIO_PPDSDR_FEC1H                (*(vuint8 *)(&__MBAR[0xA26]))
#define MCF_GPIO_PCLRR_FEC1H                 (*(vuint8 *)(&__MBAR[0xA36]))

#define MCF_GPIO_PODR_FEC1L                  (*(vuint8 *)(&__MBAR[0xA07]))
#define MCF_GPIO_PDDR_FEC1L                  (*(vuint8 *)(&__MBAR[0xA17]))
#define MCF_GPIO_PPDSDR_FEC1L                (*(vuint8 *)(&__MBAR[0xA27]))
#define MCF_GPIO_PCLRR_FEC1L                 (*(vuint8 *)(&__MBAR[0xA37]))

#define MCF_GPIO_PODR_FECI2C                 (*(vuint8 *)(&__MBAR[0xA08]))
#define MCF_GPIO_PDDR_FECI2C                 (*(vuint8 *)(&__MBAR[0xA18]))
#define MCF_GPIO_PPDSDR_FECI2C               (*(vuint8 *)(&__MBAR[0xA28]))
#define MCF_GPIO_PCLRR_FECI2C                (*(vuint8 *)(&__MBAR[0xA38]))

#define MCF_GPIO_PODR_PCIBG                  (*(vuint8 *)(&__MBAR[0xA09]))
#define MCF_GPIO_PDDR_PCIBG                  (*(vuint8 *)(&__MBAR[0xA19]))
#define MCF_GPIO_PPDSDR_PCIBG                (*(vuint8 *)(&__MBAR[0xA29]))
#define MCF_GPIO_PCLRR_PCIBG                 (*(vuint8 *)(&__MBAR[0xA39]))

#define MCF_GPIO_PODR_PCIBR                  (*(vuint8 *)(&__MBAR[0xA0A]))
#define MCF_GPIO_PDDR_PCIBR                  (*(vuint8 *)(&__MBAR[0xA1A]))
#define MCF_GPIO_PPDSDR_PCIBR                (*(vuint8 *)(&__MBAR[0xA2A]))
#define MCF_GPIO_PCLRR_PCIBR                 (*(vuint8 *)(&__MBAR[0xA3A]))

#define MCF_GPIO2_PODR_PSC3PSC               (*(vuint8 *)(&__MBAR[0xA0C]))
#define MCF_GPIO2_PDDR_PSC3PSC               (*(vuint8 *)(&__MBAR[0xA1C]))
#define MCF_GPIO2_PPDSDR_PSC3PSC             (*(vuint8 *)(&__MBAR[0xA2C]))
#define MCF_GPIO2_PCLRR_PSC3PSC              (*(vuint8 *)(&__MBAR[0xA3C]))

#define MCF_GPIO0_PODR_PSC1PSC               (*(vuint8 *)(&__MBAR[0xA0D]))
#define MCF_GPIO0_PDDR_PSC1PSC               (*(vuint8 *)(&__MBAR[0xA1D]))
#define MCF_GPIO0_PPDSDR_PSC1PSC             (*(vuint8 *)(&__MBAR[0xA2D]))
#define MCF_GPIO0_PCLRR_PSC1PSC              (*(vuint8 *)(&__MBAR[0xA3D]))

#define MCF_GPIO_PODR_DSPI                   (*(vuint8 *)(&__MBAR[0xA0E]))
#define MCF_GPIO_PDDR_DSPI                   (*(vuint8 *)(&__MBAR[0xA1E]))
#define MCF_GPIO_PPDSDR_DSPI                 (*(vuint8 *)(&__MBAR[0xA2E]))
#define MCF_GPIO_PCLRR_DSPI                  (*(vuint8 *)(&__MBAR[0xA3E]))



/* Bit definitions and macros for MCF_GPIO_PODR_FBCTL */
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL0      (0x1)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL1      (0x2)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL2      (0x4)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL3      (0x8)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL4      (0x10)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL5      (0x20)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL6      (0x40)
#define MCF_GPIO_PODR_FBCTL_PODR_FBCTL7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBCTL */
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL0      (0x1)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL1      (0x2)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL2      (0x4)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL3      (0x8)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL4      (0x10)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL5      (0x20)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL6      (0x40)
#define MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBCTL */
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL0  (0x1)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL1  (0x2)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL2  (0x4)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL3  (0x8)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL4  (0x10)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL5  (0x20)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL6  (0x40)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDR_FBCTL7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBCTL */
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL0    (0x1)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL1    (0x2)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL2    (0x4)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL3    (0x8)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL4    (0x10)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL5    (0x20)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL6    (0x40)
#define MCF_GPIO_PCLRR_FBCTL_PCLRR_FBCTL7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FBCS */
#define MCF_GPIO_PODR_FBCS_PODR_FBCS1        (0x2)
#define MCF_GPIO_PODR_FBCS_PODR_FBCS2        (0x4)
#define MCF_GPIO_PODR_FBCS_PODR_FBCS3        (0x8)
#define MCF_GPIO_PODR_FBCS_PODR_FBCS4        (0x10)
#define MCF_GPIO_PODR_FBCS_PODR_FBCS5        (0x20)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBCS */
#define MCF_GPIO_PDDR_FBCS_PDDR_FBCS1        (0x2)
#define MCF_GPIO_PDDR_FBCS_PDDR_FBCS2        (0x4)
#define MCF_GPIO_PDDR_FBCS_PDDR_FBCS3        (0x8)
#define MCF_GPIO_PDDR_FBCS_PDDR_FBCS4        (0x10)
#define MCF_GPIO_PDDR_FBCS_PDDR_FBCS5        (0x20)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBCS */
#define MCF_GPIO_PPDSDR_FBCS_PPDSDR_FBCS1    (0x2)
#define MCF_GPIO_PPDSDR_FBCS_PPDSDR_FBCS2    (0x4)
#define MCF_GPIO_PPDSDR_FBCS_PPDSDR_FBCS3    (0x8)
#define MCF_GPIO_PPDSDR_FBCS_PPDSDR_FBCS4    (0x10)
#define MCF_GPIO_PPDSDR_FBCS_PPDSDR_FBCS5    (0x20)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBCS */
#define MCF_GPIO_PCLRR_FBCS_PCLRR_FBCS1      (0x2)
#define MCF_GPIO_PCLRR_FBCS_PCLRR_FBCS2      (0x4)
#define MCF_GPIO_PCLRR_FBCS_PCLRR_FBCS3      (0x8)
#define MCF_GPIO_PCLRR_FBCS_PCLRR_FBCS4      (0x10)
#define MCF_GPIO_PCLRR_FBCS_PCLRR_FBCS5      (0x20)

/* Bit definitions and macros for MCF_GPIO_PODR_DMA */
#define MCF_GPIO_PODR_DMA_PODR_DMA0          (0x1)
#define MCF_GPIO_PODR_DMA_PODR_DMA1          (0x2)
#define MCF_GPIO_PODR_DMA_PODR_DMA2          (0x4)
#define MCF_GPIO_PODR_DMA_PODR_DMA3          (0x8)

/* Bit definitions and macros for MCF_GPIO_PDDR_DMA */
#define MCF_GPIO_PDDR_DMA_PDDR_DMA0          (0x1)
#define MCF_GPIO_PDDR_DMA_PDDR_DMA1          (0x2)
#define MCF_GPIO_PDDR_DMA_PDDR_DMA2          (0x4)
#define MCF_GPIO_PDDR_DMA_PDDR_DMA3          (0x8)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_DMA */
#define MCF_GPIO_PPDSDR_DMA_PPDSDR_DMA0      (0x1)
#define MCF_GPIO_PPDSDR_DMA_PPDSDR_DMA1      (0x2)
#define MCF_GPIO_PPDSDR_DMA_PPDSDR_DMA2      (0x4)
#define MCF_GPIO_PPDSDR_DMA_PPDSDR_DMA3      (0x8)

/* Bit definitions and macros for MCF_GPIO_PCLRR_DMA */
#define MCF_GPIO_PCLRR_DMA_PCLRR_DMA0        (0x1)
#define MCF_GPIO_PCLRR_DMA_PCLRR_DMA1        (0x2)
#define MCF_GPIO_PCLRR_DMA_PCLRR_DMA2        (0x4)
#define MCF_GPIO_PCLRR_DMA_PCLRR_DMA3        (0x8)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC0H */
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H0      (0x1)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H1      (0x2)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H2      (0x4)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H3      (0x8)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H4      (0x10)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H5      (0x20)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H6      (0x40)
#define MCF_GPIO_PODR_FEC0H_PODR_FEC0H7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC0H */
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H0      (0x1)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H1      (0x2)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H2      (0x4)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H3      (0x8)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H4      (0x10)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H5      (0x20)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H6      (0x40)
#define MCF_GPIO_PDDR_FEC0H_PDDR_FEC0H7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC0H */
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H0  (0x1)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H1  (0x2)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H2  (0x4)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H3  (0x8)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H4  (0x10)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H5  (0x20)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H6  (0x40)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDR_FEC0H7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC0H */
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H0    (0x1)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H1    (0x2)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H2    (0x4)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H3    (0x8)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H4    (0x10)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H5    (0x20)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H6    (0x40)
#define MCF_GPIO_PCLRR_FEC0H_PCLRR_FEC0H7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC0L */
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L0      (0x1)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L1      (0x2)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L2      (0x4)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L3      (0x8)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L4      (0x10)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L5      (0x20)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L6      (0x40)
#define MCF_GPIO_PODR_FEC0L_PODR_FEC0L7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC0L */
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L0      (0x1)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L1      (0x2)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L2      (0x4)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L3      (0x8)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L4      (0x10)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L5      (0x20)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L6      (0x40)
#define MCF_GPIO_PDDR_FEC0L_PDDR_FEC0L7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC0L */
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L0  (0x1)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L1  (0x2)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L2  (0x4)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L3  (0x8)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L4  (0x10)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L5  (0x20)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L6  (0x40)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDR_FEC0L7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC0L */
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L0    (0x1)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L1    (0x2)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L2    (0x4)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L3    (0x8)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L4    (0x10)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L5    (0x20)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L6    (0x40)
#define MCF_GPIO_PCLRR_FEC0L_PCLRR_FEC0L7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC1H */
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H0      (0x1)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H1      (0x2)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H2      (0x4)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H3      (0x8)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H4      (0x10)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H5      (0x20)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H6      (0x40)
#define MCF_GPIO_PODR_FEC1H_PODR_FEC1H7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC1H */
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H0      (0x1)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H1      (0x2)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H2      (0x4)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H3      (0x8)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H4      (0x10)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H5      (0x20)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H6      (0x40)
#define MCF_GPIO_PDDR_FEC1H_PDDR_FEC1H7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC1H */
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H0  (0x1)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H1  (0x2)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H2  (0x4)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H3  (0x8)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H4  (0x10)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H5  (0x20)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H6  (0x40)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDR_FEC1H7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC1H */
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H0    (0x1)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H1    (0x2)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H2    (0x4)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H3    (0x8)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H4    (0x10)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H5    (0x20)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H6    (0x40)
#define MCF_GPIO_PCLRR_FEC1H_PCLRR_FEC1H7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC1L */
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L0      (0x1)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L1      (0x2)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L2      (0x4)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L3      (0x8)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L4      (0x10)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L5      (0x20)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L6      (0x40)
#define MCF_GPIO_PODR_FEC1L_PODR_FEC1L7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC1L */
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L0      (0x1)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L1      (0x2)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L2      (0x4)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L3      (0x8)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L4      (0x10)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L5      (0x20)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L6      (0x40)
#define MCF_GPIO_PDDR_FEC1L_PDDR_FEC1L7      (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC1L */
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L0  (0x1)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L1  (0x2)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L2  (0x4)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L3  (0x8)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L4  (0x10)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L5  (0x20)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L6  (0x40)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDR_FEC1L7  (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC1L */
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L0    (0x1)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L1    (0x2)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L2    (0x4)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L3    (0x8)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L4    (0x10)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L5    (0x20)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L6    (0x40)
#define MCF_GPIO_PCLRR_FEC1L_PCLRR_FEC1L7    (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FECI2C */
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C0    (0x1)
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C1    (0x2)
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C2    (0x4)
#define MCF_GPIO_PODR_FECI2C_PODR_FECI2C3    (0x8)

/* Bit definitions and macros for MCF_GPIO_PDDR_FECI2C */
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C0    (0x1)
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C1    (0x2)
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C2    (0x4)
#define MCF_GPIO_PDDR_FECI2C_PDDR_FECI2C3    (0x8)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FECI2C */
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C0 (0x1)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C1 (0x2)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C2 (0x4)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDR_FECI2C3 (0x8)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FECI2C */
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C0  (0x1)
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C1  (0x2)
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C2  (0x4)
#define MCF_GPIO_PCLRR_FECI2C_PCLRR_FECI2C3  (0x8)

/* Bit definitions and macros for MCF_GPIO_PODR_PCIBG */
#define MCF_GPIO_PODR_PCIBG_PODR_PCIBG0      (0x1)
#define MCF_GPIO_PODR_PCIBG_PODR_PCIBG1      (0x2)
#define MCF_GPIO_PODR_PCIBG_PODR_PCIBG2      (0x4)
#define MCF_GPIO_PODR_PCIBG_PODR_PCIBG3      (0x8)
#define MCF_GPIO_PODR_PCIBG_PODR_PCIBG4      (0x10)

/* Bit definitions and macros for MCF_GPIO_PDDR_PCIBG */
#define MCF_GPIO_PDDR_PCIBG_PDDR_PCIBG0      (0x1)
#define MCF_GPIO_PDDR_PCIBG_PDDR_PCIBG1      (0x2)
#define MCF_GPIO_PDDR_PCIBG_PDDR_PCIBG2      (0x4)
#define MCF_GPIO_PDDR_PCIBG_PDDR_PCIBG3      (0x8)
#define MCF_GPIO_PDDR_PCIBG_PDDR_PCIBG4      (0x10)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PCIBG */
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG0  (0x1)
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG1  (0x2)
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG2  (0x4)
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG3  (0x8)
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDR_PCIBG4  (0x10)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PCIBG */
#define MCF_GPIO_PCLRR_PCIBG_PCLRR_PCIBG0    (0x1)
#define MCF_GPIO_PCLRR_PCIBG_PCLRR_PCIBG1    (0x2)
#define MCF_GPIO_PCLRR_PCIBG_PCLRR_PCIBG2    (0x4)
#define MCF_GPIO_PCLRR_PCIBG_PCLRR_PCIBG3    (0x8)
#define MCF_GPIO_PCLRR_PCIBG_PCLRR_PCIBG4    (0x10)

/* Bit definitions and macros for MCF_GPIO_PODR_PCIBR */
#define MCF_GPIO_PODR_PCIBR_PODR_PCIBR0      (0x1)
#define MCF_GPIO_PODR_PCIBR_PODR_PCIBR1      (0x2)
#define MCF_GPIO_PODR_PCIBR_PODR_PCIBR2      (0x4)
#define MCF_GPIO_PODR_PCIBR_PODR_PCIBR3      (0x8)
#define MCF_GPIO_PODR_PCIBR_PODR_PCIBR4      (0x10)

/* Bit definitions and macros for MCF_GPIO_PDDR_PCIBR */
#define MCF_GPIO_PDDR_PCIBR_PDDR_PCIBR0      (0x1)
#define MCF_GPIO_PDDR_PCIBR_PDDR_PCIBR1      (0x2)
#define MCF_GPIO_PDDR_PCIBR_PDDR_PCIBR2      (0x4)
#define MCF_GPIO_PDDR_PCIBR_PDDR_PCIBR3      (0x8)
#define MCF_GPIO_PDDR_PCIBR_PDDR_PCIBR4      (0x10)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PCIBR */
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR0  (0x1)
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR1  (0x2)
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR2  (0x4)
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR3  (0x8)
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDR_PCIBR4  (0x10)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PCIBR */
#define MCF_GPIO_PCLRR_PCIBR_PCLRR_PCIBR0    (0x1)
#define MCF_GPIO_PCLRR_PCIBR_PCLRR_PCIBR1    (0x2)
#define MCF_GPIO_PCLRR_PCIBR_PCLRR_PCIBR2    (0x4)
#define MCF_GPIO_PCLRR_PCIBR_PCLRR_PCIBR3    (0x8)
#define MCF_GPIO_PCLRR_PCIBR_PCLRR_PCIBR4    (0x10)

/* Bit definitions and macros for MCF_GPIO_PODR_PSC3PSC */
#define MCF_GPIO_PODR_PSC3PSC_PODR_PSC3PSC20 (0x1)
#define MCF_GPIO_PODR_PSC3PSC_PODR_PSC3PSC21 (0x2)
#define MCF_GPIO_PODR_PSC3PSC_PODR_PSC3PSC22 (0x4)
#define MCF_GPIO_PODR_PSC3PSC_PODR_PSC3PSC23 (0x8)
#define MCF_GPIO_PODR_PSC3PSC_PODR_PSC3PSC24 (0x10)
#define MCF_GPIO_PODR_PSC3PSC_PODR_PSC3PSC25 (0x20)
#define MCF_GPIO_PODR_PSC3PSC_PODR_PSC3PSC26 (0x40)
#define MCF_GPIO_PODR_PSC3PSC_PODR_PSC3PSC27 (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_PSC3PSC */
#define MCF_GPIO_PDDR_PSC3PSC_PDDR_PSC3PSC20 (0x1)
#define MCF_GPIO_PDDR_PSC3PSC_PDDR_PSC3PSC21 (0x2)
#define MCF_GPIO_PDDR_PSC3PSC_PDDR_PSC3PSC22 (0x4)
#define MCF_GPIO_PDDR_PSC3PSC_PDDR_PSC3PSC23 (0x8)
#define MCF_GPIO_PDDR_PSC3PSC_PDDR_PSC3PSC24 (0x10)
#define MCF_GPIO_PDDR_PSC3PSC_PDDR_PSC3PSC25 (0x20)
#define MCF_GPIO_PDDR_PSC3PSC_PDDR_PSC3PSC26 (0x40)
#define MCF_GPIO_PDDR_PSC3PSC_PDDR_PSC3PSC27 (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PSC3PSC */
#define MCF_GPIO_PPDSDR_PSC3PSC_PPDSDR_PSC3PSC20 (0x1)
#define MCF_GPIO_PPDSDR_PSC3PSC_PPDSDR_PSC3PSC21 (0x2)
#define MCF_GPIO_PPDSDR_PSC3PSC_PPDSDR_PSC3PSC22 (0x4)
#define MCF_GPIO_PPDSDR_PSC3PSC_PPDSDR_PSC3PSC23 (0x8)
#define MCF_GPIO_PPDSDR_PSC3PSC_PPDSDR_PSC3PSC24 (0x10)
#define MCF_GPIO_PPDSDR_PSC3PSC_PPDSDR_PSC3PSC25 (0x20)
#define MCF_GPIO_PPDSDR_PSC3PSC_PPDSDR_PSC3PSC26 (0x40)
#define MCF_GPIO_PPDSDR_PSC3PSC_PPDSDR_PSC3PSC27 (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PSC3PSC */
#define MCF_GPIO_PCLRR_PSC3PSC_PCLRR_PSC3PSC20 (0x1)
#define MCF_GPIO_PCLRR_PSC3PSC_PCLRR_PSC3PSC21 (0x2)
#define MCF_GPIO_PCLRR_PSC3PSC_PCLRR_PSC3PSC22 (0x4)
#define MCF_GPIO_PCLRR_PSC3PSC_PCLRR_PSC3PSC23 (0x8)
#define MCF_GPIO_PCLRR_PSC3PSC_PCLRR_PSC3PSC24 (0x10)
#define MCF_GPIO_PCLRR_PSC3PSC_PCLRR_PSC3PSC25 (0x20)
#define MCF_GPIO_PCLRR_PSC3PSC_PCLRR_PSC3PSC26 (0x40)
#define MCF_GPIO_PCLRR_PSC3PSC_PCLRR_PSC3PSC27 (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_PSC1PSC */
#define MCF_GPIO_PODR_PSC1PSC_PODR_PSC1PSC00 (0x1)
#define MCF_GPIO_PODR_PSC1PSC_PODR_PSC1PSC01 (0x2)
#define MCF_GPIO_PODR_PSC1PSC_PODR_PSC1PSC02 (0x4)
#define MCF_GPIO_PODR_PSC1PSC_PODR_PSC1PSC03 (0x8)
#define MCF_GPIO_PODR_PSC1PSC_PODR_PSC1PSC04 (0x10)
#define MCF_GPIO_PODR_PSC1PSC_PODR_PSC1PSC05 (0x20)
#define MCF_GPIO_PODR_PSC1PSC_PODR_PSC1PSC06 (0x40)
#define MCF_GPIO_PODR_PSC1PSC_PODR_PSC1PSC07 (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_PSC1PSC */
#define MCF_GPIO_PDDR_PSC1PSC_PDDR_PSC1PSC00 (0x1)
#define MCF_GPIO_PDDR_PSC1PSC_PDDR_PSC1PSC01 (0x2)
#define MCF_GPIO_PDDR_PSC1PSC_PDDR_PSC1PSC02 (0x4)
#define MCF_GPIO_PDDR_PSC1PSC_PDDR_PSC1PSC03 (0x8)
#define MCF_GPIO_PDDR_PSC1PSC_PDDR_PSC1PSC04 (0x10)
#define MCF_GPIO_PDDR_PSC1PSC_PDDR_PSC1PSC05 (0x20)
#define MCF_GPIO_PDDR_PSC1PSC_PDDR_PSC1PSC06 (0x40)
#define MCF_GPIO_PDDR_PSC1PSC_PDDR_PSC1PSC07 (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PSC1PSC */
#define MCF_GPIO_PPDSDR_PSC1PSC_PPDSDR_PSC1PSC00 (0x1)
#define MCF_GPIO_PPDSDR_PSC1PSC_PPDSDR_PSC1PSC01 (0x2)
#define MCF_GPIO_PPDSDR_PSC1PSC_PPDSDR_PSC1PSC02 (0x4)
#define MCF_GPIO_PPDSDR_PSC1PSC_PPDSDR_PSC1PSC03 (0x8)
#define MCF_GPIO_PPDSDR_PSC1PSC_PPDSDR_PSC1PSC04 (0x10)
#define MCF_GPIO_PPDSDR_PSC1PSC_PPDSDR_PSC1PSC05 (0x20)
#define MCF_GPIO_PPDSDR_PSC1PSC_PPDSDR_PSC1PSC06 (0x40)
#define MCF_GPIO_PPDSDR_PSC1PSC_PPDSDR_PSC1PSC07 (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PSC1PSC */
#define MCF_GPIO_PCLRR_PSC1PSC_PCLRR_PSC1PSC00 (0x1)
#define MCF_GPIO_PCLRR_PSC1PSC_PCLRR_PSC1PSC01 (0x2)
#define MCF_GPIO_PCLRR_PSC1PSC_PCLRR_PSC1PSC02 (0x4)
#define MCF_GPIO_PCLRR_PSC1PSC_PCLRR_PSC1PSC03 (0x8)
#define MCF_GPIO_PCLRR_PSC1PSC_PCLRR_PSC1PSC04 (0x10)
#define MCF_GPIO_PCLRR_PSC1PSC_PCLRR_PSC1PSC05 (0x20)
#define MCF_GPIO_PCLRR_PSC1PSC_PCLRR_PSC1PSC06 (0x40)
#define MCF_GPIO_PCLRR_PSC1PSC_PCLRR_PSC1PSC07 (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_DSPI */
#define MCF_GPIO_PODR_DSPI_PODR_DSPI0        (0x1)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI1        (0x2)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI2        (0x4)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI3        (0x8)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI4        (0x10)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI5        (0x20)
#define MCF_GPIO_PODR_DSPI_PODR_DSPI6        (0x40)

/* Bit definitions and macros for MCF_GPIO_PDDR_DSPI */
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI0        (0x1)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI1        (0x2)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI2        (0x4)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI3        (0x8)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI4        (0x10)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI5        (0x20)
#define MCF_GPIO_PDDR_DSPI_PDDR_DSPI6        (0x40)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_DSPI */
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI0    (0x1)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI1    (0x2)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI2    (0x4)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI3    (0x8)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI4    (0x10)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI5    (0x20)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDR_DSPI6    (0x40)

/* Bit definitions and macros for MCF_GPIO_PCLRR_DSPI */
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI0      (0x1)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI1      (0x2)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI2      (0x4)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI3      (0x8)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI4      (0x10)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI5      (0x20)
#define MCF_GPIO_PCLRR_DSPI_PCLRR_DSPI6      (0x40)

/*********************************************************************
*
* Programmable Serial Controller (PSC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_PSC0_PSCMR2                      (*(vuint8 *)(&__MBAR[0x8600]))
#define MCF_PSC0_PSCMR1                      (*(vuint8 *)(&__MBAR[0x8600]))
#define MCF_PSC0_PSCCSR                      (*(vuint8 *)(&__MBAR[0x8604]))
#define MCF_PSC0_PSCSR                       (*(vuint16*)(&__MBAR[0x8604]))
#define MCF_PSC0_PSCCR                       (*(vuint8 *)(&__MBAR[0x8608]))
#define MCF_PSC0_PSCRB_8BIT                  (*(vuint32*)(&__MBAR[0x860C]))
#define MCF_PSC0_PSCTB_8BIT                  (*(vuint32*)(&__MBAR[0x860C]))
#define MCF_PSC0_PSCRB_16BIT                 (*(vuint32*)(&__MBAR[0x860C]))
#define MCF_PSC0_PSCTB_16BIT                 (*(vuint32*)(&__MBAR[0x860C]))
#define MCF_PSC0_PSCRB_AC97                  (*(vuint32*)(&__MBAR[0x860C]))
#define MCF_PSC0_PSCTB_AC97                  (*(vuint32*)(&__MBAR[0x860C]))
#define MCF_PSC0_PSCIPCR                     (*(vuint8 *)(&__MBAR[0x8610]))
#define MCF_PSC0_PSCACR                      (*(vuint8 *)(&__MBAR[0x8610]))
#define MCF_PSC0_PSCIMR                      (*(vuint16*)(&__MBAR[0x8614]))
#define MCF_PSC0_PSCISR                      (*(vuint16*)(&__MBAR[0x8614]))
#define MCF_PSC0_PSCCTUR                     (*(vuint8 *)(&__MBAR[0x8618]))
#define MCF_PSC0_PSCCTLR                     (*(vuint8 *)(&__MBAR[0x861C]))
#define MCF_PSC0_PSCIP                       (*(vuint8 *)(&__MBAR[0x8634]))
#define MCF_PSC0_PSCOPSET                    (*(vuint8 *)(&__MBAR[0x8638]))
#define MCF_PSC0_PSCOPRESET                  (*(vuint8 *)(&__MBAR[0x863C]))
#define MCF_PSC0_PSCSICR                     (*(vuint8 *)(&__MBAR[0x8640]))
#define MCF_PSC0_PSCIRCR1                    (*(vuint8 *)(&__MBAR[0x8644]))
#define MCF_PSC0_PSCIRCR2                    (*(vuint8 *)(&__MBAR[0x8648]))
#define MCF_PSC0_PSCIRSDR                    (*(vuint8 *)(&__MBAR[0x864C]))
#define MCF_PSC0_PSCIRMDR                    (*(vuint8 *)(&__MBAR[0x8650]))
#define MCF_PSC0_PSCIRFDR                    (*(vuint8 *)(&__MBAR[0x8654]))
#define MCF_PSC0_PSCRFCNT                    (*(vuint16*)(&__MBAR[0x8658]))
#define MCF_PSC0_PSCTFCNT                    (*(vuint16*)(&__MBAR[0x865C]))
#define MCF_PSC0_PSCRFDR                     (*(vuint32*)(&__MBAR[0x8660]))
#define MCF_PSC0_PSCRFSR                     (*(vuint16*)(&__MBAR[0x8664]))
#define MCF_PSC0_PSCRFCR                     (*(vuint32*)(&__MBAR[0x8668]))
#define MCF_PSC0_PSCRFAR                     (*(vuint16*)(&__MBAR[0x866E]))
#define MCF_PSC0_PSCRFRP                     (*(vuint16*)(&__MBAR[0x8672]))
#define MCF_PSC0_PSCRFWP                     (*(vuint16*)(&__MBAR[0x8676]))
#define MCF_PSC0_PSCRLRFP                    (*(vuint16*)(&__MBAR[0x867A]))
#define MCF_PSC0_PSCRLWFP                    (*(vuint16*)(&__MBAR[0x867E]))
#define MCF_PSC0_PSCTFDR                     (*(vuint32*)(&__MBAR[0x8680]))
#define MCF_PSC0_PSCTFSR                     (*(vuint16*)(&__MBAR[0x8684]))
#define MCF_PSC0_PSCTFCR                     (*(vuint32*)(&__MBAR[0x8688]))
#define MCF_PSC0_PSCTFAR                     (*(vuint16*)(&__MBAR[0x868E]))
#define MCF_PSC0_PSCTFRP                     (*(vuint16*)(&__MBAR[0x8692]))
#define MCF_PSC0_PSCTFWP                     (*(vuint16*)(&__MBAR[0x8696]))
#define MCF_PSC0_PSCTLRFP                    (*(vuint16*)(&__MBAR[0x869A]))
#define MCF_PSC0_PSCTLWFP                    (*(vuint16*)(&__MBAR[0x869E]))

#define MCF_PSC1_PSCMR2                      (*(vuint8 *)(&__MBAR[0x8700]))
#define MCF_PSC1_PSCMR1                      (*(vuint8 *)(&__MBAR[0x8700]))
#define MCF_PSC1_PSCCSR                      (*(vuint8 *)(&__MBAR[0x8704]))
#define MCF_PSC1_PSCSR                       (*(vuint16*)(&__MBAR[0x8704]))
#define MCF_PSC1_PSCCR                       (*(vuint8 *)(&__MBAR[0x8708]))
#define MCF_PSC1_PSCRB_8BIT                  (*(vuint32*)(&__MBAR[0x870C]))
#define MCF_PSC1_PSCTB_8BIT                  (*(vuint32*)(&__MBAR[0x870C]))
#define MCF_PSC1_PSCRB_16BIT                 (*(vuint32*)(&__MBAR[0x870C]))
#define MCF_PSC1_PSCTB_16BIT                 (*(vuint32*)(&__MBAR[0x870C]))
#define MCF_PSC1_PSCRB_AC97                  (*(vuint32*)(&__MBAR[0x870C]))
#define MCF_PSC1_PSCTB_AC97                  (*(vuint32*)(&__MBAR[0x870C]))
#define MCF_PSC1_PSCIPCR                     (*(vuint8 *)(&__MBAR[0x8710]))
#define MCF_PSC1_PSCACR                      (*(vuint8 *)(&__MBAR[0x8710]))
#define MCF_PSC1_PSCIMR                      (*(vuint16*)(&__MBAR[0x8714]))
#define MCF_PSC1_PSCISR                      (*(vuint16*)(&__MBAR[0x8714]))
#define MCF_PSC1_PSCCTUR                     (*(vuint8 *)(&__MBAR[0x8718]))
#define MCF_PSC1_PSCCTLR                     (*(vuint8 *)(&__MBAR[0x871C]))
#define MCF_PSC1_PSCIP                       (*(vuint8 *)(&__MBAR[0x8734]))
#define MCF_PSC1_PSCOPSET                    (*(vuint8 *)(&__MBAR[0x8738]))
#define MCF_PSC1_PSCOPRESET                  (*(vuint8 *)(&__MBAR[0x873C]))
#define MCF_PSC1_PSCSICR                     (*(vuint8 *)(&__MBAR[0x8740]))
#define MCF_PSC1_PSCIRCR1                    (*(vuint8 *)(&__MBAR[0x8744]))
#define MCF_PSC1_PSCIRCR2                    (*(vuint8 *)(&__MBAR[0x8748]))
#define MCF_PSC1_PSCIRSDR                    (*(vuint8 *)(&__MBAR[0x874C]))
#define MCF_PSC1_PSCIRMDR                    (*(vuint8 *)(&__MBAR[0x8750]))
#define MCF_PSC1_PSCIRFDR                    (*(vuint8 *)(&__MBAR[0x8754]))
#define MCF_PSC1_PSCRFCNT                    (*(vuint16*)(&__MBAR[0x8758]))
#define MCF_PSC1_PSCTFCNT                    (*(vuint16*)(&__MBAR[0x875C]))
#define MCF_PSC1_PSCRFDR                     (*(vuint32*)(&__MBAR[0x8760]))
#define MCF_PSC1_PSCRFSR                     (*(vuint16*)(&__MBAR[0x8764]))
#define MCF_PSC1_PSCRFCR                     (*(vuint32*)(&__MBAR[0x8768]))
#define MCF_PSC1_PSCRFAR                     (*(vuint16*)(&__MBAR[0x876E]))
#define MCF_PSC1_PSCRFRP                     (*(vuint16*)(&__MBAR[0x8772]))
#define MCF_PSC1_PSCRFWP                     (*(vuint16*)(&__MBAR[0x8776]))
#define MCF_PSC1_PSCRLRFP                    (*(vuint16*)(&__MBAR[0x877A]))
#define MCF_PSC1_PSCRLWFP                    (*(vuint16*)(&__MBAR[0x877E]))
#define MCF_PSC1_PSCTFDR                     (*(vuint32*)(&__MBAR[0x8780]))
#define MCF_PSC1_PSCTFSR                     (*(vuint16*)(&__MBAR[0x8784]))
#define MCF_PSC1_PSCTFCR                     (*(vuint32*)(&__MBAR[0x8788]))
#define MCF_PSC1_PSCTFAR                     (*(vuint16*)(&__MBAR[0x878E]))
#define MCF_PSC1_PSCTFRP                     (*(vuint16*)(&__MBAR[0x8792]))
#define MCF_PSC1_PSCTFWP                     (*(vuint16*)(&__MBAR[0x8796]))
#define MCF_PSC1_PSCTLRFP                    (*(vuint16*)(&__MBAR[0x879A]))
#define MCF_PSC1_PSCTLWFP                    (*(vuint16*)(&__MBAR[0x879E]))

#define MCF_PSC2_PSCMR2                      (*(vuint8 *)(&__MBAR[0x8800]))
#define MCF_PSC2_PSCMR1                      (*(vuint8 *)(&__MBAR[0x8800]))
#define MCF_PSC2_PSCCSR                      (*(vuint8 *)(&__MBAR[0x8804]))
#define MCF_PSC2_PSCSR                       (*(vuint16*)(&__MBAR[0x8804]))
#define MCF_PSC2_PSCCR                       (*(vuint8 *)(&__MBAR[0x8808]))
#define MCF_PSC2_PSCRB_8BIT                  (*(vuint32*)(&__MBAR[0x880C]))
#define MCF_PSC2_PSCTB_8BIT                  (*(vuint32*)(&__MBAR[0x880C]))
#define MCF_PSC2_PSCRB_16BIT                 (*(vuint32*)(&__MBAR[0x880C]))
#define MCF_PSC2_PSCTB_16BIT                 (*(vuint32*)(&__MBAR[0x880C]))
#define MCF_PSC2_PSCRB_AC97                  (*(vuint32*)(&__MBAR[0x880C]))
#define MCF_PSC2_PSCTB_AC97                  (*(vuint32*)(&__MBAR[0x880C]))
#define MCF_PSC2_PSCIPCR                     (*(vuint8 *)(&__MBAR[0x8810]))
#define MCF_PSC2_PSCACR                      (*(vuint8 *)(&__MBAR[0x8810]))
#define MCF_PSC2_PSCIMR                      (*(vuint16*)(&__MBAR[0x8814]))
#define MCF_PSC2_PSCISR                      (*(vuint16*)(&__MBAR[0x8814]))
#define MCF_PSC2_PSCCTUR                     (*(vuint8 *)(&__MBAR[0x8818]))
#define MCF_PSC2_PSCCTLR                     (*(vuint8 *)(&__MBAR[0x881C]))
#define MCF_PSC2_PSCIP                       (*(vuint8 *)(&__MBAR[0x8834]))
#define MCF_PSC2_PSCOPSET                    (*(vuint8 *)(&__MBAR[0x8838]))
#define MCF_PSC2_PSCOPRESET                  (*(vuint8 *)(&__MBAR[0x883C]))
#define MCF_PSC2_PSCSICR                     (*(vuint8 *)(&__MBAR[0x8840]))
#define MCF_PSC2_PSCIRCR1                    (*(vuint8 *)(&__MBAR[0x8844]))
#define MCF_PSC2_PSCIRCR2                    (*(vuint8 *)(&__MBAR[0x8848]))
#define MCF_PSC2_PSCIRSDR                    (*(vuint8 *)(&__MBAR[0x884C]))
#define MCF_PSC2_PSCIRMDR                    (*(vuint8 *)(&__MBAR[0x8850]))
#define MCF_PSC2_PSCIRFDR                    (*(vuint8 *)(&__MBAR[0x8854]))
#define MCF_PSC2_PSCRFCNT                    (*(vuint16*)(&__MBAR[0x8858]))
#define MCF_PSC2_PSCTFCNT                    (*(vuint16*)(&__MBAR[0x885C]))
#define MCF_PSC2_PSCRFDR                     (*(vuint32*)(&__MBAR[0x8860]))
#define MCF_PSC2_PSCRFSR                     (*(vuint16*)(&__MBAR[0x8864]))
#define MCF_PSC2_PSCRFCR                     (*(vuint32*)(&__MBAR[0x8868]))
#define MCF_PSC2_PSCRFAR                     (*(vuint16*)(&__MBAR[0x886E]))
#define MCF_PSC2_PSCRFRP                     (*(vuint16*)(&__MBAR[0x8872]))
#define MCF_PSC2_PSCRFWP                     (*(vuint16*)(&__MBAR[0x8876]))
#define MCF_PSC2_PSCRLRFP                    (*(vuint16*)(&__MBAR[0x887A]))
#define MCF_PSC2_PSCRLWFP                    (*(vuint16*)(&__MBAR[0x887E]))
#define MCF_PSC2_PSCTFDR                     (*(vuint32*)(&__MBAR[0x8880]))
#define MCF_PSC2_PSCTFSR                     (*(vuint16*)(&__MBAR[0x8884]))
#define MCF_PSC2_PSCTFCR                     (*(vuint32*)(&__MBAR[0x8888]))
#define MCF_PSC2_PSCTFAR                     (*(vuint16*)(&__MBAR[0x888E]))
#define MCF_PSC2_PSCTFRP                     (*(vuint16*)(&__MBAR[0x8892]))
#define MCF_PSC2_PSCTFWP                     (*(vuint16*)(&__MBAR[0x8896]))
#define MCF_PSC2_PSCTLRFP                    (*(vuint16*)(&__MBAR[0x889A]))
#define MCF_PSC2_PSCTLWFP                    (*(vuint16*)(&__MBAR[0x889E]))

#define MCF_PSC3_PSCMR2                      (*(vuint8 *)(&__MBAR[0x8900]))
#define MCF_PSC3_PSCMR1                      (*(vuint8 *)(&__MBAR[0x8900]))
#define MCF_PSC3_PSCCSR                      (*(vuint8 *)(&__MBAR[0x8904]))
#define MCF_PSC3_PSCSR                       (*(vuint16*)(&__MBAR[0x8904]))
#define MCF_PSC3_PSCCR                       (*(vuint8 *)(&__MBAR[0x8908]))
#define MCF_PSC3_PSCRB_8BIT                  (*(vuint32*)(&__MBAR[0x890C]))
#define MCF_PSC3_PSCTB_8BIT                  (*(vuint32*)(&__MBAR[0x890C]))
#define MCF_PSC3_PSCRB_16BIT                 (*(vuint32*)(&__MBAR[0x890C]))
#define MCF_PSC3_PSCTB_16BIT                 (*(vuint32*)(&__MBAR[0x890C]))
#define MCF_PSC3_PSCRB_AC97                  (*(vuint32*)(&__MBAR[0x890C]))
#define MCF_PSC3_PSCTB_AC97                  (*(vuint32*)(&__MBAR[0x890C]))
#define MCF_PSC3_PSCIPCR                     (*(vuint8 *)(&__MBAR[0x8910]))
#define MCF_PSC3_PSCACR                      (*(vuint8 *)(&__MBAR[0x8910]))
#define MCF_PSC3_PSCIMR                      (*(vuint16*)(&__MBAR[0x8914]))
#define MCF_PSC3_PSCISR                      (*(vuint16*)(&__MBAR[0x8914]))
#define MCF_PSC3_PSCCTUR                     (*(vuint8 *)(&__MBAR[0x8918]))
#define MCF_PSC3_PSCCTLR                     (*(vuint8 *)(&__MBAR[0x891C]))
#define MCF_PSC3_PSCIP                       (*(vuint8 *)(&__MBAR[0x8934]))
#define MCF_PSC3_PSCOPSET                    (*(vuint8 *)(&__MBAR[0x8938]))
#define MCF_PSC3_PSCOPRESET                  (*(vuint8 *)(&__MBAR[0x893C]))
#define MCF_PSC3_PSCSICR                     (*(vuint8 *)(&__MBAR[0x8940]))
#define MCF_PSC3_PSCIRCR1                    (*(vuint8 *)(&__MBAR[0x8944]))
#define MCF_PSC3_PSCIRCR2                    (*(vuint8 *)(&__MBAR[0x8948]))
#define MCF_PSC3_PSCIRSDR                    (*(vuint8 *)(&__MBAR[0x894C]))
#define MCF_PSC3_PSCIRMDR                    (*(vuint8 *)(&__MBAR[0x8950]))
#define MCF_PSC3_PSCIRFDR                    (*(vuint8 *)(&__MBAR[0x8954]))
#define MCF_PSC3_PSCRFCNT                    (*(vuint16*)(&__MBAR[0x8958]))
#define MCF_PSC3_PSCTFCNT                    (*(vuint16*)(&__MBAR[0x895C]))
#define MCF_PSC3_PSCRFDR                     (*(vuint32*)(&__MBAR[0x8960]))
#define MCF_PSC3_PSCRFSR                     (*(vuint16*)(&__MBAR[0x8964]))
#define MCF_PSC3_PSCRFCR                     (*(vuint32*)(&__MBAR[0x8968]))
#define MCF_PSC3_PSCRFAR                     (*(vuint16*)(&__MBAR[0x896E]))
#define MCF_PSC3_PSCRFRP                     (*(vuint16*)(&__MBAR[0x8972]))
#define MCF_PSC3_PSCRFWP                     (*(vuint16*)(&__MBAR[0x8976]))
#define MCF_PSC3_PSCRLRFP                    (*(vuint16*)(&__MBAR[0x897A]))
#define MCF_PSC3_PSCRLWFP                    (*(vuint16*)(&__MBAR[0x897E]))
#define MCF_PSC3_PSCTFDR                     (*(vuint32*)(&__MBAR[0x8980]))
#define MCF_PSC3_PSCTFSR                     (*(vuint16*)(&__MBAR[0x8984]))
#define MCF_PSC3_PSCTFCR                     (*(vuint32*)(&__MBAR[0x8988]))
#define MCF_PSC3_PSCTFAR                     (*(vuint16*)(&__MBAR[0x898E]))
#define MCF_PSC3_PSCTFRP                     (*(vuint16*)(&__MBAR[0x8992]))
#define MCF_PSC3_PSCTFWP                     (*(vuint16*)(&__MBAR[0x8996]))
#define MCF_PSC3_PSCTLRFP                    (*(vuint16*)(&__MBAR[0x899A]))
#define MCF_PSC3_PSCTLWFP                    (*(vuint16*)(&__MBAR[0x899E]))

#define MCF_PSC_PSCMR(x)                     (*(vuint8 *)(&__MBAR[0x8600 + ((x)*0x100)]))
#define MCF_PSC_PSCCSR(x)                    (*(vuint8 *)(&__MBAR[0x8604 + ((x)*0x100)]))
#define MCF_PSC_PSCSR(x)                     (*(vuint16*)(&__MBAR[0x8604 + ((x)*0x100)]))
#define MCF_PSC_PSCCR(x)                     (*(vuint8 *)(&__MBAR[0x8608 + ((x)*0x100)]))
#define MCF_PSC_PSCRB_8BIT(x)                (*(vuint32*)(&__MBAR[0x860C + ((x)*0x100)]))
#define MCF_PSC_PSCTB_8BIT(x)                (*(vuint32*)(&__MBAR[0x860C + ((x)*0x100)]))
#define MCF_PSC_PSCRB_16BIT(x)               (*(vuint32*)(&__MBAR[0x860C + ((x)*0x100)]))
#define MCF_PSC_PSCTB_16BIT(x)               (*(vuint32*)(&__MBAR[0x860C + ((x)*0x100)]))
#define MCF_PSC_PSCRB_AC97(x)                (*(vuint32*)(&__MBAR[0x860C + ((x)*0x100)]))
#define MCF_PSC_PSCTB_AC97(x)                (*(vuint32*)(&__MBAR[0x860C + ((x)*0x100)]))
#define MCF_PSC_PSCIPCR(x)                   (*(vuint8 *)(&__MBAR[0x8610 + ((x)*0x100)]))
#define MCF_PSC_PSCACR(x)                    (*(vuint8 *)(&__MBAR[0x8610 + ((x)*0x100)]))
#define MCF_PSC_PSCIMR(x)                    (*(vuint16*)(&__MBAR[0x8614 + ((x)*0x100)]))
#define MCF_PSC_PSCISR(x)                    (*(vuint16*)(&__MBAR[0x8614 + ((x)*0x100)]))
#define MCF_PSC_PSCCTUR(x)                   (*(vuint8 *)(&__MBAR[0x8618 + ((x)*0x100)]))
#define MCF_PSC_PSCCTLR(x)                   (*(vuint8 *)(&__MBAR[0x861C + ((x)*0x100)]))
#define MCF_PSC_PSCIP(x)                     (*(vuint8 *)(&__MBAR[0x8634 + ((x)*0x100)]))
#define MCF_PSC_PSCOPSET(x)                  (*(vuint8 *)(&__MBAR[0x8638 + ((x)*0x100)]))
#define MCF_PSC_PSCOPRESET(x)                (*(vuint8 *)(&__MBAR[0x863C + ((x)*0x100)]))
#define MCF_PSC_PSCSICR(x)                   (*(vuint8 *)(&__MBAR[0x8640 + ((x)*0x100)]))
#define MCF_PSC_PSCIRCR1(x)                  (*(vuint8 *)(&__MBAR[0x8644 + ((x)*0x100)]))
#define MCF_PSC_PSCIRCR2(x)                  (*(vuint8 *)(&__MBAR[0x8648 + ((x)*0x100)]))
#define MCF_PSC_PSCIRSDR(x)                  (*(vuint8 *)(&__MBAR[0x864C + ((x)*0x100)]))
#define MCF_PSC_PSCIRMDR(x)                  (*(vuint8 *)(&__MBAR[0x8650 + ((x)*0x100)]))
#define MCF_PSC_PSCIRFDR(x)                  (*(vuint8 *)(&__MBAR[0x8654 + ((x)*0x100)]))
#define MCF_PSC_PSCRFCNT(x)                  (*(vuint16*)(&__MBAR[0x8658 + ((x)*0x100)]))
#define MCF_PSC_PSCTFCNT(x)                  (*(vuint16*)(&__MBAR[0x865C + ((x)*0x100)]))
#define MCF_PSC_PSCRFDR(x)                   (*(vuint32*)(&__MBAR[0x8660 + ((x)*0x100)]))
#define MCF_PSC_PSCRFSR(x)                   (*(vuint16*)(&__MBAR[0x8664 + ((x)*0x100)]))
#define MCF_PSC_PSCRFCR(x)                   (*(vuint32*)(&__MBAR[0x8668 + ((x)*0x100)]))
#define MCF_PSC_PSCRFAR(x)                   (*(vuint16*)(&__MBAR[0x866E + ((x)*0x100)]))
#define MCF_PSC_PSCRFRP(x)                   (*(vuint16*)(&__MBAR[0x8672 + ((x)*0x100)]))
#define MCF_PSC_PSCRFWP(x)                   (*(vuint16*)(&__MBAR[0x8676 + ((x)*0x100)]))
#define MCF_PSC_PSCRLRFP(x)                  (*(vuint16*)(&__MBAR[0x867A + ((x)*0x100)]))
#define MCF_PSC_PSCRLWFP(x)                  (*(vuint16*)(&__MBAR[0x867E + ((x)*0x100)]))
#define MCF_PSC_PSCTFDR(x)                   (*(vuint32*)(&__MBAR[0x8680 + ((x)*0x100)]))
#define MCF_PSC_PSCTFSR(x)                   (*(vuint16*)(&__MBAR[0x8684 + ((x)*0x100)]))
#define MCF_PSC_PSCTFCR(x)                   (*(vuint32*)(&__MBAR[0x8688 + ((x)*0x100)]))
#define MCF_PSC_PSCTFAR(x)                   (*(vuint16*)(&__MBAR[0x868E + ((x)*0x100)]))
#define MCF_PSC_PSCTFRP(x)                   (*(vuint16*)(&__MBAR[0x8692 + ((x)*0x100)]))
#define MCF_PSC_PSCTFWP(x)                   (*(vuint16*)(&__MBAR[0x8696 + ((x)*0x100)]))
#define MCF_PSC_PSCTLRFP(x)                  (*(vuint16*)(&__MBAR[0x869A + ((x)*0x100)]))
#define MCF_PSC_PSCTLWFP(x)                  (*(vuint16*)(&__MBAR[0x869E + ((x)*0x100)]))

/* Bit definitions and macros for MCF_PSC_PSCMR */
#define MCF_PSC_PSCMR_SB(x)                  (((x)&0xF)<<0)
#define MCF_PSC_PSCMR_TXCTS                  (0x10)
#define MCF_PSC_PSCMR_TXRTS                  (0x20)
#define MCF_PSC_PSCMR_CM(x)                  (((x)&0x3)<<0x6)
#define MCF_PSC_PSCMR_CM_NORMAL              (0)
#define MCF_PSC_PSCMR_CM_ECHO                (0x40)
#define MCF_PSC_PSCMR_CM_LOCAL_LOOP          (0x80)
#define MCF_PSC_PSCMR_CM_REMOTE_LOOP         (0xC0)
#define MCF_PSC_PSCMR_SB_STOP_BITS_1         (0x7)
#define MCF_PSC_PSCMR_SB_STOP_BITS_15        (0x8)
#define MCF_PSC_PSCMR_SB_STOP_BITS_2         (0xF)
#define MCF_PSC_PSCMR_PM_MULTI_ADDR          (0x1C)
#define MCF_PSC_PSCMR_PM_MULTI_DATA          (0x18)
#define MCF_PSC_PSCMR_PM_NONE                (0x10)
#define MCF_PSC_PSCMR_PM_FORCE_HI            (0xC)
#define MCF_PSC_PSCMR_PM_FORCE_LO            (0x8)
#define MCF_PSC_PSCMR_PM_ODD                 (0x4)
#define MCF_PSC_PSCMR_PM_EVEN                (0)
#define MCF_PSC_PSCMR_BC(x)                  (((x)&0x3)<<0)
#define MCF_PSC_PSCMR_BC_5                   (0)
#define MCF_PSC_PSCMR_BC_6                   (0x1)
#define MCF_PSC_PSCMR_BC_7                   (0x2)
#define MCF_PSC_PSCMR_BC_8                   (0x3)
#define MCF_PSC_PSCMR_PT                     (0x4)
#define MCF_PSC_PSCMR_PM(x)                  (((x)&0x3)<<0x3)
#define MCF_PSC_PSCMR_ERR                    (0x20)
#define MCF_PSC_PSCMR_RXIRQ_FU               (0x40)
#define MCF_PSC_PSCMR_RXRTS                  (0x80)

/* Bit definitions and macros for MCF_PSC_PSCCSR */
#define MCF_PSC_PSCCSR_TCSEL(x)              (((x)&0xF)<<0)
#define MCF_PSC_PSCCSR_RCSEL(x)              (((x)&0xF)<<0x4)
#define MCF_PSC_PSCCSR_TCSEL_SYS_CLK         (0x0D)
#define MCF_PSC_PSCCSR_TCSEL_CTM16           (0x0E)
#define MCF_PSC_PSCCSR_TCSEL_CTM             (0x0F)
#define MCF_PSC_PSCCSR_RCSEL_SYS_CLK         (0xD0)
#define MCF_PSC_PSCCSR_RCSEL_CTM16           (0xE0)
#define MCF_PSC_PSCCSR_RCSEL_CTM             (0xF0)

/* Bit definitions and macros for MCF_PSC_PSCSR */
#define MCF_PSC_PSCSR_ERR                    (0x40)
#define MCF_PSC_PSCSR_CDE_DEOF               (0x80)
#define MCF_PSC_PSCSR_RXRDY                  (0x100)
#define MCF_PSC_PSCSR_FU                     (0x200)
#define MCF_PSC_PSCSR_TXRDY                  (0x400)
#define MCF_PSC_PSCSR_TXEMP_URERR            (0x800)
#define MCF_PSC_PSCSR_OE                     (0x1000)
#define MCF_PSC_PSCSR_PE_CRCERR              (0x2000)
#define MCF_PSC_PSCSR_FE_PHYERR              (0x4000)
#define MCF_PSC_PSCSR_RB_NEOF                (0x8000)

/* Bit definitions and macros for MCF_PSC_PSCCR */
#define MCF_PSC_PSCCR_RXC(x)                 (((x)&0x3)<<0)
#define MCF_PSC_PSCCR_RX_ENABLED             (0x1)
#define MCF_PSC_PSCCR_RX_DISABLED            (0x2)
#define MCF_PSC_PSCCR_TXC(x)                 (((x)&0x3)<<0x2)
#define MCF_PSC_PSCCR_TX_ENABLED             (0x4)
#define MCF_PSC_PSCCR_TX_DISABLED            (0x8)
#define MCF_PSC_PSCCR_MISC(x)                (((x)&0x7)<<0x4)
#define MCF_PSC_PSCCR_NONE                   (0)
#define MCF_PSC_PSCCR_RESET_MR               (0x10)
#define MCF_PSC_PSCCR_RESET_RX               (0x20)
#define MCF_PSC_PSCCR_RESET_TX               (0x30)
#define MCF_PSC_PSCCR_RESET_ERROR            (0x40)
#define MCF_PSC_PSCCR_RESET_BKCHGINT         (0x50)
#define MCF_PSC_PSCCR_START_BREAK            (0x60)
#define MCF_PSC_PSCCR_STOP_BREAK             (0x70)

/* Bit definitions and macros for MCF_PSC_PSCRB_8BIT */
#define MCF_PSC_PSCRB_8BIT_RB3(x)            (((x)&0xFF)<<0)
#define MCF_PSC_PSCRB_8BIT_RB2(x)            (((x)&0xFF)<<0x8)
#define MCF_PSC_PSCRB_8BIT_RB1(x)            (((x)&0xFF)<<0x10)
#define MCF_PSC_PSCRB_8BIT_RB0(x)            (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PSC_PSCTB_8BIT */
#define MCF_PSC_PSCTB_8BIT_TB3(x)            (((x)&0xFF)<<0)
#define MCF_PSC_PSCTB_8BIT_TB2(x)            (((x)&0xFF)<<0x8)
#define MCF_PSC_PSCTB_8BIT_TB1(x)            (((x)&0xFF)<<0x10)
#define MCF_PSC_PSCTB_8BIT_TB0(x)            (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_PSC_PSCRB_16BIT */
#define MCF_PSC_PSCRB_16BIT_RB1(x)           (((x)&0xFFFF)<<0)
#define MCF_PSC_PSCRB_16BIT_RB0(x)           (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_PSC_PSCTB_16BIT */
#define MCF_PSC_PSCTB_16BIT_TB1(x)           (((x)&0xFFFF)<<0)
#define MCF_PSC_PSCTB_16BIT_TB0(x)           (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_PSC_PSCRB_AC97 */
#define MCF_PSC_PSCRB_AC97_SOF               (0x800)
#define MCF_PSC_PSCRB_AC97_RB(x)             (((x)&0xFFFFF)<<0xC)

/* Bit definitions and macros for MCF_PSC_PSCTB_AC97 */
#define MCF_PSC_PSCTB_AC97_TB(x)             (((x)&0xFFFFF)<<0xC)

/* Bit definitions and macros for MCF_PSC_PSCIPCR */
#define MCF_PSC_PSCIPCR_RESERVED             (0xC)
#define MCF_PSC_PSCIPCR_CTS                  (0xD)
#define MCF_PSC_PSCIPCR_D_CTS                (0x1C)
#define MCF_PSC_PSCIPCR_SYNC                 (0x8C)

/* Bit definitions and macros for MCF_PSC_PSCACR */
#define MCF_PSC_PSCACR_IEC0                  (0x1)

/* Bit definitions and macros for MCF_PSC_PSCIMR */
#define MCF_PSC_PSCIMR_ERR                   (0x40)
#define MCF_PSC_PSCIMR_DEOF                  (0x80)
#define MCF_PSC_PSCIMR_TXRDY                 (0x100)
#define MCF_PSC_PSCIMR_RXRDY_FU              (0x200)
#define MCF_PSC_PSCIMR_DB                    (0x400)
#define MCF_PSC_PSCIMR_IPC                   (0x8000)

/* Bit definitions and macros for MCF_PSC_PSCISR */
#define MCF_PSC_PSCISR_ERR                   (0x40)
#define MCF_PSC_PSCISR_DEOF                  (0x80)
#define MCF_PSC_PSCISR_TXRDY                 (0x100)
#define MCF_PSC_PSCISR_RXRDY_FU              (0x200)
#define MCF_PSC_PSCISR_DB                    (0x400)
#define MCF_PSC_PSCISR_IPC                   (0x8000)

/* Bit definitions and macros for MCF_PSC_PSCCTUR */
#define MCF_PSC_PSCCTUR_CT(x)                (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCCTLR */
#define MCF_PSC_PSCCTLR_CT(x)                (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCIP */
#define MCF_PSC_PSCIP_CTS                    (0x1)
#define MCF_PSC_PSCIP_TGL                    (0x40)
#define MCF_PSC_PSCIP_LPWR_B                 (0x80)

/* Bit definitions and macros for MCF_PSC_PSCOPSET */
#define MCF_PSC_PSCOPSET_RTS                 (0x1)

/* Bit definitions and macros for MCF_PSC_PSCOPRESET */
#define MCF_PSC_PSCOPRESET_RTS               (0x1)

/* Bit definitions and macros for MCF_PSC_PSCSICR */
#define MCF_PSC_PSCSICR_SIM(x)               (((x)&0x7)<<0)
#define MCF_PSC_PSCSICR_SIM_UART             (0)
#define MCF_PSC_PSCSICR_SIM_MODEM8           (0x1)
#define MCF_PSC_PSCSICR_SIM_MODEM16          (0x2)
#define MCF_PSC_PSCSICR_SIM_AC97             (0x3)
#define MCF_PSC_PSCSICR_SIM_SIR              (0x4)
#define MCF_PSC_PSCSICR_SIM_MIR              (0x5)
#define MCF_PSC_PSCSICR_SIM_FIR              (0x6)
#define MCF_PSC_PSCSICR_SHDIR                (0x10)
#define MCF_PSC_PSCSICR_DTS1                 (0x20)
#define MCF_PSC_PSCSICR_AWR                  (0x40)
#define MCF_PSC_PSCSICR_ACRB                 (0x80)

/* Bit definitions and macros for MCF_PSC_PSCIRCR1 */
#define MCF_PSC_PSCIRCR1_SPUL                (0x1)
#define MCF_PSC_PSCIRCR1_SIPEN               (0x2)
#define MCF_PSC_PSCIRCR1_FD                  (0x4)

/* Bit definitions and macros for MCF_PSC_PSCIRCR2 */
#define MCF_PSC_PSCIRCR2_NXTEOF              (0x1)
#define MCF_PSC_PSCIRCR2_ABORT               (0x2)
#define MCF_PSC_PSCIRCR2_SIPREQ              (0x4)

/* Bit definitions and macros for MCF_PSC_PSCIRSDR */
#define MCF_PSC_PSCIRSDR_IRSTIM(x)           (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCIRMDR */
#define MCF_PSC_PSCIRMDR_M_FDIV(x)           (((x)&0x7F)<<0)
#define MCF_PSC_PSCIRMDR_FREQ                (0x80)

/* Bit definitions and macros for MCF_PSC_PSCIRFDR */
#define MCF_PSC_PSCIRFDR_F_FDIV(x)           (((x)&0xF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCRFCNT */
#define MCF_PSC_PSCRFCNT_CNT(x)              (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCTFCNT */
#define MCF_PSC_PSCTFCNT_CNT(x)              (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCRFDR */
#define MCF_PSC_PSCRFDR_DATA(x)              (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCRFSR */
#define MCF_PSC_PSCRFSR_EMT                  (0x1)
#define MCF_PSC_PSCRFSR_ALARM                (0x2)
#define MCF_PSC_PSCRFSR_FU                   (0x4)
#define MCF_PSC_PSCRFSR_FRMRDY               (0x8)
#define MCF_PSC_PSCRFSR_OF                   (0x10)
#define MCF_PSC_PSCRFSR_UF                   (0x20)
#define MCF_PSC_PSCRFSR_RXW                  (0x40)
#define MCF_PSC_PSCRFSR_FAE                  (0x80)
#define MCF_PSC_PSCRFSR_FRM(x)               (((x)&0xF)<<0x8)
#define MCF_PSC_PSCRFSR_FRM_BYTE0            (0x800)
#define MCF_PSC_PSCRFSR_FRM_BYTE1            (0x400)
#define MCF_PSC_PSCRFSR_FRM_BYTE2            (0x200)
#define MCF_PSC_PSCRFSR_FRM_BYTE3            (0x100)
#define MCF_PSC_PSCRFSR_TAG(x)               (((x)&0x3)<<0xC)
#define MCF_PSC_PSCRFSR_TXW                  (0x4000)
#define MCF_PSC_PSCRFSR_IP                   (0x8000)

/* Bit definitions and macros for MCF_PSC_PSCRFCR */
#define MCF_PSC_PSCRFCR_CNTR(x)              (((x)&0xFFFF)<<0)
#define MCF_PSC_PSCRFCR_TXW_MSK              (0x40000)
#define MCF_PSC_PSCRFCR_OF_MSK               (0x80000)
#define MCF_PSC_PSCRFCR_UF_MSK               (0x100000)
#define MCF_PSC_PSCRFCR_RXW_MSK              (0x200000)
#define MCF_PSC_PSCRFCR_FAE_MSK              (0x400000)
#define MCF_PSC_PSCRFCR_IP_MSK               (0x800000)
#define MCF_PSC_PSCRFCR_GR(x)                (((x)&0x7)<<0x18)
#define MCF_PSC_PSCRFCR_FRMEN                (0x8000000)
#define MCF_PSC_PSCRFCR_TIMER                (0x10000000)

/* Bit definitions and macros for MCF_PSC_PSCRFAR */
#define MCF_PSC_PSCRFAR_ALARM(x)             (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCRFRP */
#define MCF_PSC_PSCRFRP_READ(x)              (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCRFWP */
#define MCF_PSC_PSCRFWP_WRITE(x)             (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCRLRFP */
#define MCF_PSC_PSCRLRFP_LRFP(x)             (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCRLWFP */
#define MCF_PSC_PSCRLWFP_LWFP(x)             (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCTFDR */
#define MCF_PSC_PSCTFDR_DATA(x)              (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCTFSR */
#define MCF_PSC_PSCTFSR_EMT                  (0x1)
#define MCF_PSC_PSCTFSR_ALARM                (0x2)
#define MCF_PSC_PSCTFSR_FU                   (0x4)
#define MCF_PSC_PSCTFSR_FRMRDY               (0x8)
#define MCF_PSC_PSCTFSR_OF                   (0x10)
#define MCF_PSC_PSCTFSR_UF                   (0x20)
#define MCF_PSC_PSCTFSR_RXW                  (0x40)
#define MCF_PSC_PSCTFSR_FAE                  (0x80)
#define MCF_PSC_PSCTFSR_FRM(x)               (((x)&0xF)<<0x8)
#define MCF_PSC_PSCTFSR_FRM_BYTE0            (0x800)
#define MCF_PSC_PSCTFSR_FRM_BYTE1            (0x400)
#define MCF_PSC_PSCTFSR_FRM_BYTE2            (0x200)
#define MCF_PSC_PSCTFSR_FRM_BYTE3            (0x100)
#define MCF_PSC_PSCTFSR_TAG(x)               (((x)&0x3)<<0xC)
#define MCF_PSC_PSCTFSR_TXW                  (0x4000)
#define MCF_PSC_PSCTFSR_IP                   (0x8000)

/* Bit definitions and macros for MCF_PSC_PSCTFCR */
#define MCF_PSC_PSCTFCR_CNTR(x)              (((x)&0xFFFF)<<0)
#define MCF_PSC_PSCTFCR_TXW_MSK              (0x40000)
#define MCF_PSC_PSCTFCR_OF_MSK               (0x80000)
#define MCF_PSC_PSCTFCR_UF_MSK               (0x100000)
#define MCF_PSC_PSCTFCR_RXW_MSK              (0x200000)
#define MCF_PSC_PSCTFCR_FAE_MSK              (0x400000)
#define MCF_PSC_PSCTFCR_IP_MSK               (0x800000)
#define MCF_PSC_PSCTFCR_GR(x)                (((x)&0x7)<<0x18)
#define MCF_PSC_PSCTFCR_FRMEN                (0x8000000)
#define MCF_PSC_PSCTFCR_TIMER                (0x10000000)
#define MCF_PSC_PSCTFCR_WFR                  (0x20000000)

/* Bit definitions and macros for MCF_PSC_PSCTFAR */
#define MCF_PSC_PSCTFAR_ALARM(x)             (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCTFRP */
#define MCF_PSC_PSCTFRP_READ(x)              (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCTFWP */
#define MCF_PSC_PSCTFWP_WRITE(x)             (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCTLRFP */
#define MCF_PSC_PSCTLRFP_LRFP(x)             (((x)&0x1FF)<<0)

/* Bit definitions and macros for MCF_PSC_PSCTLWFP */
#define MCF_PSC_PSCTLWFP_LWFP(x)             (((x)&0x1FF)<<0)


/*********************************************************************
*
* DMA Serial Peripheral Interface (DSPI)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_DSPI_DMCR         (*(vuint32*)(void*)(&__MBAR[0x008A00]))
#define MCF_DSPI_DTCR         (*(vuint32*)(void*)(&__MBAR[0x008A08]))
#define MCF_DSPI_DCTAR0       (*(vuint32*)(void*)(&__MBAR[0x008A0C]))
#define MCF_DSPI_DCTAR1       (*(vuint32*)(void*)(&__MBAR[0x008A10]))
#define MCF_DSPI_DCTAR2       (*(vuint32*)(void*)(&__MBAR[0x008A14]))
#define MCF_DSPI_DCTAR3       (*(vuint32*)(void*)(&__MBAR[0x008A18]))
#define MCF_DSPI_DCTAR4       (*(vuint32*)(void*)(&__MBAR[0x008A1C]))
#define MCF_DSPI_DCTAR5       (*(vuint32*)(void*)(&__MBAR[0x008A20]))
#define MCF_DSPI_DCTAR6       (*(vuint32*)(void*)(&__MBAR[0x008A24]))
#define MCF_DSPI_DCTAR7       (*(vuint32*)(void*)(&__MBAR[0x008A28]))
#define MCF_DSPI_DCTAR(x)     (*(vuint32*)(void*)(&__MBAR[0x008A0C+((x)*0x004)]))
#define MCF_DSPI_DSR          (*(vuint32*)(void*)(&__MBAR[0x008A2C]))
#define MCF_DSPI_DIRSR        (*(vuint32*)(void*)(&__MBAR[0x008A30]))
#define MCF_DSPI_DTFR         (*(vuint32*)(void*)(&__MBAR[0x008A34]))
#define MCF_DSPI_DRFR         (*(vuint32*)(void*)(&__MBAR[0x008A38]))
#define MCF_DSPI_DTFDR0       (*(vuint32*)(void*)(&__MBAR[0x008A3C]))
#define MCF_DSPI_DTFDR1       (*(vuint32*)(void*)(&__MBAR[0x008A40]))
#define MCF_DSPI_DTFDR2       (*(vuint32*)(void*)(&__MBAR[0x008A44]))
#define MCF_DSPI_DTFDR3       (*(vuint32*)(void*)(&__MBAR[0x008A48]))
#define MCF_DSPI_DTFDR(x)     (*(vuint32*)(void*)(&__MBAR[0x008A3C+((x)*0x004)]))
#define MCF_DSPI_DRFDR0       (*(vuint32*)(void*)(&__MBAR[0x008A7C]))
#define MCF_DSPI_DRFDR1       (*(vuint32*)(void*)(&__MBAR[0x008A80]))
#define MCF_DSPI_DRFDR2       (*(vuint32*)(void*)(&__MBAR[0x008A84]))
#define MCF_DSPI_DRFDR3       (*(vuint32*)(void*)(&__MBAR[0x008A88]))
#define MCF_DSPI_DRFDR(x)     (*(vuint32*)(void*)(&__MBAR[0x008A7C+((x)*0x004)]))

/* Bit definitions and macros for MCF_DSPI_DMCR */
#define MCF_DSPI_DMCR_HALT             (0x00000001)
#define MCF_DSPI_DMCR_SMPL_PT(x)       (((x)&0x00000003)<<8)
#define MCF_DSPI_DMCR_CRXF             (0x00000400)
#define MCF_DSPI_DMCR_CTXF             (0x00000800)
#define MCF_DSPI_DMCR_DRXF             (0x00001000)
#define MCF_DSPI_DMCR_DTXF             (0x00002000)
#define MCF_DSPI_DMCR_CSIS0            (0x00010000)
#define MCF_DSPI_DMCR_CSIS2            (0x00040000)
#define MCF_DSPI_DMCR_CSIS3            (0x00080000)
#define MCF_DSPI_DMCR_CSIS5            (0x00200000)
#define MCF_DSPI_DMCR_ROOE             (0x01000000)
#define MCF_DSPI_DMCR_PCSSE            (0x02000000)
#define MCF_DSPI_DMCR_MTFE             (0x04000000)
#define MCF_DSPI_DMCR_FRZ              (0x08000000)
#define MCF_DSPI_DMCR_DCONF(x)         (((x)&0x00000003)<<28)
#define MCF_DSPI_DMCR_CSCK             (0x40000000)
#define MCF_DSPI_DMCR_MSTR             (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DTCR */
#define MCF_DSPI_DTCR_SPI_TCNT(x)      (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_DSPI_DCTAR [0 thru 7] */
#define MCF_DSPI_DCTAR_BR(x)           (((x)&0x0000000F)<<0)
#define MCF_DSPI_DCTAR_DT(x)           (((x)&0x0000000F)<<4)
#define MCF_DSPI_DCTAR_ASC(x)          (((x)&0x0000000F)<<8)
#define MCF_DSPI_DCTAR_CSSCK(x)        (((x)&0x0000000F)<<12)
#define MCF_DSPI_DCTAR_PBR(x)          (((x)&0x00000003)<<16)
#define MCF_DSPI_DCTAR_PDT(x)          (((x)&0x00000003)<<18)
#define MCF_DSPI_DCTAR_PASC(x)         (((x)&0x00000003)<<20)
#define MCF_DSPI_DCTAR_PCSSCK(x)       (((x)&0x00000003)<<22)
#define MCF_DSPI_DCTAR_LSBFE           (0x01000000)
#define MCF_DSPI_DCTAR_CPHA            (0x02000000)
#define MCF_DSPI_DCTAR_CPOL            (0x04000000)
#define MCF_DSPI_DCTAR_TRSZ(x)         (((x)&0x0000000F)<<27)
#define MCF_DSPI_DCTAR_PCSSCK_1CLK     (0x00000000)
#define MCF_DSPI_DCTAR_PCSSCK_3CLK     (0x00400000)
#define MCF_DSPI_DCTAR_PCSSCK_5CLK     (0x00800000)
#define MCF_DSPI_DCTAR_PCSSCK_7CLK     (0x00A00000)
#define MCF_DSPI_DCTAR_PASC_1CLK       (0x00000000)
#define MCF_DSPI_DCTAR_PASC_3CLK       (0x00100000)
#define MCF_DSPI_DCTAR_PASC_5CLK       (0x00200000)
#define MCF_DSPI_DCTAR_PASC_7CLK       (0x00300000)
#define MCF_DSPI_DCTAR_PDT_1CLK        (0x00000000)
#define MCF_DSPI_DCTAR_PDT_3CLK        (0x00040000)
#define MCF_DSPI_DCTAR_PDT_5CLK        (0x00080000)
#define MCF_DSPI_DCTAR_PDT_7CLK        (0x000A0000)
#define MCF_DSPI_DCTAR_PBR_2CLK        (0x00000000)
#define MCF_DSPI_DCTAR_PBR_3CLK        (0x00010000)
#define MCF_DSPI_DCTAR_PBR_5CLK        (0x00020000)
#define MCF_DSPI_DCTAR_PBR_7CLK        (0x00030000)

/* Bit definitions and macros for MCF_DSPI_DSR */
#define MCF_DSPI_DSR_RXPTR(x)          (((x)&0x0000000F)<<0)
#define MCF_DSPI_DSR_RXCTR(x)          (((x)&0x0000000F)<<4)
#define MCF_DSPI_DSR_TXPTR(x)          (((x)&0x0000000F)<<8)
#define MCF_DSPI_DSR_TXCTR(x)          (((x)&0x0000000F)<<12)
#define MCF_DSPI_DSR_RFDF              (0x00020000)
#define MCF_DSPI_DSR_RFOF              (0x00080000)
#define MCF_DSPI_DSR_TFFF              (0x02000000)
#define MCF_DSPI_DSR_TFUF              (0x08000000)
#define MCF_DSPI_DSR_EOQF              (0x10000000)
#define MCF_DSPI_DSR_TXRXS             (0x40000000)
#define MCF_DSPI_DSR_TCF               (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DIRSR */
#define MCF_DSPI_DIRSR_RFDFS           (0x00010000)
#define MCF_DSPI_DIRSR_RFDFE           (0x00020000)
#define MCF_DSPI_DIRSR_RFOFE           (0x00080000)
#define MCF_DSPI_DIRSR_TFFFS           (0x01000000)
#define MCF_DSPI_DIRSR_TFFFE           (0x02000000)
#define MCF_DSPI_DIRSR_TFUFE           (0x08000000)
#define MCF_DSPI_DIRSR_EOQFE           (0x10000000)
#define MCF_DSPI_DIRSR_TCFE            (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DTFR */
#define MCF_DSPI_DTFR_TXDATA(x)        (((x)&0x0000FFFF)<<0)
#define MCF_DSPI_DTFR_CS0              (0x00010000)
#define MCF_DSPI_DTFR_CS2              (0x00040000)
#define MCF_DSPI_DTFR_CS3              (0x00080000)
#define MCF_DSPI_DTFR_CS5              (0x00200000)
#define MCF_DSPI_DTFR_CTCNT            (0x04000000)
#define MCF_DSPI_DTFR_EOQ              (0x08000000)
#define MCF_DSPI_DTFR_CTAS(x)          (((x)&0x00000007)<<28)
#define MCF_DSPI_DTFR_CONT             (0x80000000)

/* Bit definitions and macros for MCF_DSPI_DRFR */
#define MCF_DSPI_DRFR_RXDATA(x)        (((x)&0x0000FFFF)<<0)

/* Bit definitions and macros for MCF_DSPI_DTFDR [0 thru 3] */
#define MCF_DSPI_DTFDR_TXDATA(x)       (((x)&0x0000FFFF)<<0)
#define MCF_DSPI_DTFDR_TXCMD(x)        (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_DSPI_DRFDR [0 thru 3] */
#define MCF_DSPI_DRFDR_RXDATA(x)       (((x)&0x0000FFFF)<<0)


/*********************************************************************
*
* Synchronous DRAM Controller (SDRAMC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_SDRAMC_SDRAMDS                   (*(vuint32*)(&__MBAR[0x4]))
#define MCF_SDRAMC_CS0CFG                    (*(vuint32*)(&__MBAR[0x20]))
#define MCF_SDRAMC_CS1CFG                    (*(vuint32*)(&__MBAR[0x24]))
#define MCF_SDRAMC_CS2CFG                    (*(vuint32*)(&__MBAR[0x28]))
#define MCF_SDRAMC_CS3CFG                    (*(vuint32*)(&__MBAR[0x2C]))
#define MCF_SDRAMC_SDMR                      (*(vuint32*)(&__MBAR[0x100]))
#define MCF_SDRAMC_SDCR                      (*(vuint32*)(&__MBAR[0x104]))
#define MCF_SDRAMC_SDCFG1                    (*(vuint32*)(&__MBAR[0x108]))
#define MCF_SDRAMC_SDCFG2                    (*(vuint32*)(&__MBAR[0x10C]))
#define MCF_SDRAMC_CSCFG(x)                  (*(vuint32*)(&__MBAR[0x20 + ((x)*0x4)]))


/* Bit definitions and macros for MCF_SDRAMC_SDRAMDS */
#define MCF_SDRAMC_SDRAMDS_SB_D(x)           (((x)&0x3)<<0)
#define MCF_SDRAMC_SDRAMDS_SB_S(x)           (((x)&0x3)<<0x2)
#define MCF_SDRAMC_SDRAMDS_SB_A(x)           (((x)&0x3)<<0x4)
#define MCF_SDRAMC_SDRAMDS_SB_C(x)           (((x)&0x3)<<0x6)
#define MCF_SDRAMC_SDRAMDS_SB_E(x)           (((x)&0x3)<<0x8)
#define MCF_SDRAMC_SDRAMDS_DRIVE_24MA        (0)
#define MCF_SDRAMC_SDRAMDS_DRIVE_16MA        (0x1)
#define MCF_SDRAMC_SDRAMDS_DRIVE_8MA         (0x2)
#define MCF_SDRAMC_SDRAMDS_DRIVE_NONE        (0x3)

/* Bit definitions and macros for MCF_SDRAMC_CSCFG */
#define MCF_SDRAMC_CSCFG_CSSZ(x)             (((x)&0x1F)<<0)
#define MCF_SDRAMC_CSCFG_CSSZ_DISABLED       (0)
#define MCF_SDRAMC_CSCFG_CSSZ_1MBYTE         (0x13)
#define MCF_SDRAMC_CSCFG_CSSZ_2MBYTE         (0x14)
#define MCF_SDRAMC_CSCFG_CSSZ_4MBYTE         (0x15)
#define MCF_SDRAMC_CSCFG_CSSZ_8MBYTE         (0x16)
#define MCF_SDRAMC_CSCFG_CSSZ_16MBYTE        (0x17)
#define MCF_SDRAMC_CSCFG_CSSZ_32MBYTE        (0x18)
#define MCF_SDRAMC_CSCFG_CSSZ_64MBYTE        (0x19)
#define MCF_SDRAMC_CSCFG_CSSZ_128MBYTE       (0x1A)
#define MCF_SDRAMC_CSCFG_CSSZ_256MBYTE       (0x1B)
#define MCF_SDRAMC_CSCFG_CSSZ_512MBYTE       (0x1C)
#define MCF_SDRAMC_CSCFG_CSSZ_1GBYTE         (0x1D)
#define MCF_SDRAMC_CSCFG_CSSZ_2GBYTE         (0x1E)
#define MCF_SDRAMC_CSCFG_CSSZ_4GBYTE         (0x1F)
#define MCF_SDRAMC_CSCFG_CSBA(x)             (((x)&0xFFF)<<0x14)
#define MCF_SDRAMC_CSCFG_BA(x)               ((x)&0xFFF00000)

/* Bit definitions and macros for MCF_SDRAMC_SDMR */
#define MCF_SDRAMC_SDMR_CMD                  (0x10000)
#define MCF_SDRAMC_SDMR_AD(x)                (((x)&0xFFF)<<0x12)
#define MCF_SDRAMC_SDMR_BNKAD(x)             (((x)&0x3)<<0x1E)
#define MCF_SDRAMC_SDMR_BK_LMR               (0)
#define MCF_SDRAMC_SDMR_BK_LEMR              (0x40000000)

/* Bit definitions and macros for MCF_SDRAMC_SDCR */
#define MCF_SDRAMC_SDCR_IPALL                (0x2)
#define MCF_SDRAMC_SDCR_IREF                 (0x4)
#define MCF_SDRAMC_SDCR_BUFF                 (0x10)
#define MCF_SDRAMC_SDCR_DQS_OE(x)            (((x)&0xF)<<0x8)
#define MCF_SDRAMC_SDCR_RCNT(x)              (((x)&0x3F)<<0x10)
#define MCF_SDRAMC_SDCR_DRIVE                (0x400000)
#define MCF_SDRAMC_SDCR_AP                   (0x800000)
#define MCF_SDRAMC_SDCR_MUX(x)               (((x)&0x3)<<0x18)
#define MCF_SDRAMC_SDCR_REF                  (0x10000000)
#define MCF_SDRAMC_SDCR_DDR                  (0x20000000)
#define MCF_SDRAMC_SDCR_CKE                  (0x40000000)
#define MCF_SDRAMC_SDCR_MODE_EN              (0x80000000)

/* Bit definitions and macros for MCF_SDRAMC_SDCFG1 */
#define MCF_SDRAMC_SDCFG1_WTLAT(x)           (((x)&0x7)<<0x4)
#define MCF_SDRAMC_SDCFG1_REF2ACT(x)         (((x)&0xF)<<0x8)
#define MCF_SDRAMC_SDCFG1_PRE2ACT(x)         (((x)&0x7)<<0xC)
#define MCF_SDRAMC_SDCFG1_ACT2RW(x)          (((x)&0x7)<<0x10)
#define MCF_SDRAMC_SDCFG1_RDLAT(x)           (((x)&0xF)<<0x14)
#define MCF_SDRAMC_SDCFG1_SWT2RD(x)          (((x)&0x7)<<0x18)
#define MCF_SDRAMC_SDCFG1_SRD2RW(x)          (((x)&0xF)<<0x1C)

/* Bit definitions and macros for MCF_SDRAMC_SDCFG2 */
#define MCF_SDRAMC_SDCFG2_BL(x)              (((x)&0xF)<<0x10)
#define MCF_SDRAMC_SDCFG2_BRD2WT(x)          (((x)&0xF)<<0x14)
#define MCF_SDRAMC_SDCFG2_BWT2RW(x)          (((x)&0xF)<<0x18)
#define MCF_SDRAMC_SDCFG2_BRD2PRE(x)         (((x)&0xF)<<0x1C)


/*********************************************************************
*
* FlexBus Chip Select Module (FBCS)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_FBCS0_CSAR                       (*(vuint32*)(&__MBAR[0x500]))
#define MCF_FBCS0_CSMR                       (*(vuint32*)(&__MBAR[0x504]))
#define MCF_FBCS0_CSCR                       (*(vuint32*)(&__MBAR[0x508]))

#define MCF_FBCS1_CSAR                       (*(vuint32*)(&__MBAR[0x50C]))
#define MCF_FBCS1_CSMR                       (*(vuint32*)(&__MBAR[0x510]))
#define MCF_FBCS1_CSCR                       (*(vuint32*)(&__MBAR[0x514]))

#define MCF_FBCS2_CSAR                       (*(vuint32*)(&__MBAR[0x518]))
#define MCF_FBCS2_CSMR                       (*(vuint32*)(&__MBAR[0x51C]))
#define MCF_FBCS2_CSCR                       (*(vuint32*)(&__MBAR[0x520]))

#define MCF_FBCS3_CSAR                       (*(vuint32*)(&__MBAR[0x524]))
#define MCF_FBCS3_CSMR                       (*(vuint32*)(&__MBAR[0x528]))
#define MCF_FBCS3_CSCR                       (*(vuint32*)(&__MBAR[0x52C]))

#define MCF_FBCS4_CSAR                       (*(vuint32*)(&__MBAR[0x530]))
#define MCF_FBCS4_CSMR                       (*(vuint32*)(&__MBAR[0x534]))
#define MCF_FBCS4_CSCR                       (*(vuint32*)(&__MBAR[0x538]))

#define MCF_FBCS5_CSAR                       (*(vuint32*)(&__MBAR[0x53C]))
#define MCF_FBCS5_CSMR                       (*(vuint32*)(&__MBAR[0x540]))
#define MCF_FBCS5_CSCR                       (*(vuint32*)(&__MBAR[0x544]))

#define MCF_FBCS_CSAR(x)                     (*(vuint32*)(&__MBAR[0x500 + ((x)*0xC)]))
#define MCF_FBCS_CSMR(x)                     (*(vuint32*)(&__MBAR[0x504 + ((x)*0xC)]))
#define MCF_FBCS_CSCR(x)                     (*(vuint32*)(&__MBAR[0x508 + ((x)*0xC)]))


/* Bit definitions and macros for MCF_FBCS_CSAR */
#define MCF_FBCS_CSAR_BA(x)                  ((x)&0xFFFF0000)

/* Bit definitions and macros for MCF_FBCS_CSMR */
#define MCF_FBCS_CSMR_V                      (0x1)
#define MCF_FBCS_CSMR_WP                     (0x100)
#define MCF_FBCS_CSMR_BAM(x)                 (((x)&0xFFFF)<<0x10)
#define MCF_FBCS_CSMR_BAM_4G                 (0xFFFF0000)
#define MCF_FBCS_CSMR_BAM_2G                 (0x7FFF0000)
#define MCF_FBCS_CSMR_BAM_1G                 (0x3FFF0000)
#define MCF_FBCS_CSMR_BAM_1024M              (0x3FFF0000)
#define MCF_FBCS_CSMR_BAM_512M               (0x1FFF0000)
#define MCF_FBCS_CSMR_BAM_256M               (0xFFF0000)
#define MCF_FBCS_CSMR_BAM_128M               (0x7FF0000)
#define MCF_FBCS_CSMR_BAM_64M                (0x3FF0000)
#define MCF_FBCS_CSMR_BAM_32M                (0x1FF0000)
#define MCF_FBCS_CSMR_BAM_16M                (0xFF0000)
#define MCF_FBCS_CSMR_BAM_8M                 (0x7F0000)
#define MCF_FBCS_CSMR_BAM_4M                 (0x3F0000)
#define MCF_FBCS_CSMR_BAM_2M                 (0x1F0000)
#define MCF_FBCS_CSMR_BAM_1M                 (0xF0000)
#define MCF_FBCS_CSMR_BAM_1024K              (0xF0000)
#define MCF_FBCS_CSMR_BAM_512K               (0x70000)
#define MCF_FBCS_CSMR_BAM_256K               (0x30000)
#define MCF_FBCS_CSMR_BAM_128K               (0x10000)
#define MCF_FBCS_CSMR_BAM_64K                (0)

/* Bit definitions and macros for MCF_FBCS_CSCR */
#define MCF_FBCS_CSCR_BSTW                   (0x8)
#define MCF_FBCS_CSCR_BSTR                   (0x10)
#define MCF_FBCS_CSCR_BEM                    (0x20)
#define MCF_FBCS_CSCR_PS(x)                  (((x)&0x3)<<0x6)
#define MCF_FBCS_CSCR_PS_32                  (0)
#define MCF_FBCS_CSCR_PS_8                   (0x40)
#define MCF_FBCS_CSCR_PS_16                  (0x80)
#define MCF_FBCS_CSCR_AA                     (0x100)
#define MCF_FBCS_CSCR_WS(x)                  (((x)&0x3F)<<0xA)
#define MCF_FBCS_CSCR_WRAH(x)                (((x)&0x3)<<0x10)
#define MCF_FBCS_CSCR_RDAH(x)                (((x)&0x3)<<0x12)
#define MCF_FBCS_CSCR_ASET(x)                (((x)&0x3L)<<0x14)
#define MCF_FBCS_CSCR_SWSEN                  (0x800000)
#define MCF_FBCS_CSCR_SWS(x)                 (((x)&0x3F)<<0x1A)

/*********************************************************************
*
* Slice Timers (SLT)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_SLT0_STCNT                       (*(vuint32*)(&__MBAR[0x900]))
#define MCF_SLT0_SCR                         (*(vuint32*)(&__MBAR[0x904]))
#define MCF_SLT0_SCNT                        (*(vuint32*)(&__MBAR[0x908]))
#define MCF_SLT0_SSR                         (*(vuint32*)(&__MBAR[0x90C]))

#define MCF_SLT1_STCNT                       (*(vuint32*)(&__MBAR[0x910]))
#define MCF_SLT1_SCR                         (*(vuint32*)(&__MBAR[0x914]))
#define MCF_SLT1_SCNT                        (*(vuint32*)(&__MBAR[0x918]))
#define MCF_SLT1_SSR                         (*(vuint32*)(&__MBAR[0x91C]))

#define MCF_SLT_STCNT(x)                     (*(vuint32*)(&__MBAR[0x900 + ((x)*0x10)]))
#define MCF_SLT_SCR(x)                       (*(vuint32*)(&__MBAR[0x904 + ((x)*0x10)]))
#define MCF_SLT_SCNT(x)                      (*(vuint32*)(&__MBAR[0x908 + ((x)*0x10)]))
#define MCF_SLT_SSR(x)                       (*(vuint32*)(&__MBAR[0x90C + ((x)*0x10)]))


/* Bit definitions and macros for MCF_SLT_STCNT */
#define MCF_SLT_STCNT_TC(x)                  (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_SLT_SCR */
#define MCF_SLT_SCR_TEN                      (0x1000000)
#define MCF_SLT_SCR_IEN                      (0x2000000)
#define MCF_SLT_SCR_RUN                      (0x4000000)

/* Bit definitions and macros for MCF_SLT_SCNT */
#define MCF_SLT_SCNT_CNT(x)                  (((x)&0xFFFFFFFF)<<0)

/* Bit definitions and macros for MCF_SLT_SSR */
#define MCF_SLT_SSR_ST                       (0x1000000)
#define MCF_SLT_SSR_BE                       (0x2000000)

/*********************************************************************
*
* General Purpose Timers (GPT)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_GPT0_GMS                         (*(vuint32*)(&__MBAR[0x800]))
#define MCF_GPT0_GCIR                        (*(vuint32*)(&__MBAR[0x804]))
#define MCF_GPT0_GPWM                        (*(vuint32*)(&__MBAR[0x808]))
#define MCF_GPT0_GSR                         (*(vuint32*)(&__MBAR[0x80C]))

#define MCF_GPT1_GMS                         (*(vuint32*)(&__MBAR[0x810]))
#define MCF_GPT1_GCIR                        (*(vuint32*)(&__MBAR[0x814]))
#define MCF_GPT1_GPWM                        (*(vuint32*)(&__MBAR[0x818]))
#define MCF_GPT1_GSR                         (*(vuint32*)(&__MBAR[0x81C]))

#define MCF_GPT2_GMS                         (*(vuint32*)(&__MBAR[0x820]))
#define MCF_GPT2_GCIR                        (*(vuint32*)(&__MBAR[0x824]))
#define MCF_GPT2_GPWM                        (*(vuint32*)(&__MBAR[0x828]))
#define MCF_GPT2_GSR                         (*(vuint32*)(&__MBAR[0x82C]))

#define MCF_GPT3_GMS                         (*(vuint32*)(&__MBAR[0x830]))
#define MCF_GPT3_GCIR                        (*(vuint32*)(&__MBAR[0x834]))
#define MCF_GPT3_GPWM                        (*(vuint32*)(&__MBAR[0x838]))
#define MCF_GPT3_GSR                         (*(vuint32*)(&__MBAR[0x83C]))

#define MCF_GPT_GMS(x)                       (*(vuint32*)(&__MBAR[0x800 + ((x)*0x10)]))
#define MCF_GPT_GCIR(x)                      (*(vuint32*)(&__MBAR[0x804 + ((x)*0x10)]))
#define MCF_GPT_GPWM(x)                      (*(vuint32*)(&__MBAR[0x808 + ((x)*0x10)]))
#define MCF_GPT_GSR(x)                       (*(vuint32*)(&__MBAR[0x80C + ((x)*0x10)]))


/* Bit definitions and macros for MCF_GPT_GMS */
#define MCF_GPT_GMS_TMS(x)                   (((x)&0x7)<<0)
#define MCF_GPT_GMS_TMS_DISABLE              (0)
#define MCF_GPT_GMS_TMS_INCAPT               (0x1)
#define MCF_GPT_GMS_TMS_OUTCAPT              (0x2)
#define MCF_GPT_GMS_TMS_PWM                  (0x3)
#define MCF_GPT_GMS_TMS_GPIO                 (0x4)
#define MCF_GPT_GMS_GPIO(x)                  (((x)&0x3)<<0x4)
#define MCF_GPT_GMS_GPIO_INPUT               (0)
#define MCF_GPT_GMS_GPIO_OUTLO               (0x20)
#define MCF_GPT_GMS_GPIO_OUTHI               (0x30)
#define MCF_GPT_GMS_IEN                      (0x100)
#define MCF_GPT_GMS_OD                       (0x200)
#define MCF_GPT_GMS_SC                       (0x400)
#define MCF_GPT_GMS_CE                       (0x1000)
#define MCF_GPT_GMS_WDEN                     (0x8000)
#define MCF_GPT_GMS_ICT(x)                   (((x)&0x3)<<0x10)
#define MCF_GPT_GMS_ICT_ANY                  (0)
#define MCF_GPT_GMS_ICT_RISE                 (0x10000)
#define MCF_GPT_GMS_ICT_FALL                 (0x20000)
#define MCF_GPT_GMS_ICT_PULSE                (0x30000)
#define MCF_GPT_GMS_OCT(x)                   (((x)&0x3)<<0x14)
#define MCF_GPT_GMS_OCT_FRCLOW               (0)
#define MCF_GPT_GMS_OCT_PULSEHI              (0x100000)
#define MCF_GPT_GMS_OCT_PULSELO              (0x200000)
#define MCF_GPT_GMS_OCT_TOGGLE               (0x300000)
#define MCF_GPT_GMS_OCPW(x)                  (((x)&0xFF)<<0x18)

/* Bit definitions and macros for MCF_GPT_GCIR */
#define MCF_GPT_GCIR_CNT(x)                  (((x)&0xFFFF)<<0)
#define MCF_GPT_GCIR_PRE(x)                  (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_GPT_GPWM */
#define MCF_GPT_GPWM_LOAD                    (0x1)
#define MCF_GPT_GPWM_PWMOP                   (0x100)
#define MCF_GPT_GPWM_WIDTH(x)                (((x)&0xFFFF)<<0x10)

/* Bit definitions and macros for MCF_GPT_GSR */
#define MCF_GPT_GSR_CAPT                     (0x1)
#define MCF_GPT_GSR_COMP                     (0x2)
#define MCF_GPT_GSR_PWMP                     (0x4)
#define MCF_GPT_GSR_TEXP                     (0x8)
#define MCF_GPT_GSR_PIN                      (0x100)
#define MCF_GPT_GSR_OVF(x)                   (((x)&0x7)<<0xC)
#define MCF_GPT_GSR_CAPTURE(x)               (((x)&0xFFFF)<<0x10)

/*********************************************************************
*
* Interrupt Controller (INTC)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_INTC_IPRH                        (*(vuint32*)(&__MBAR[0x700]))
#define MCF_INTC_IPRL                        (*(vuint32*)(&__MBAR[0x704]))
#define MCF_INTC_IMRH                        (*(vuint32*)(&__MBAR[0x708]))
#define MCF_INTC_IMRL                        (*(vuint32*)(&__MBAR[0x70C]))
#define MCF_INTC_INTFRCH                     (*(vuint32*)(&__MBAR[0x710]))
#define MCF_INTC_INTFRCL                     (*(vuint32*)(&__MBAR[0x714]))
#define MCF_INTC_IRLR                        (*(vuint8 *)(&__MBAR[0x718]))
#define MCF_INTC_IACKLPR                     (*(vuint8 *)(&__MBAR[0x719]))
#define MCF_INTC_ICR01                       (*(vuint8 *)(&__MBAR[0x741]))
#define MCF_INTC_ICR02                       (*(vuint8 *)(&__MBAR[0x742]))
#define MCF_INTC_ICR03                       (*(vuint8 *)(&__MBAR[0x743]))
#define MCF_INTC_ICR04                       (*(vuint8 *)(&__MBAR[0x744]))
#define MCF_INTC_ICR05                       (*(vuint8 *)(&__MBAR[0x745]))
#define MCF_INTC_ICR06                       (*(vuint8 *)(&__MBAR[0x746]))
#define MCF_INTC_ICR07                       (*(vuint8 *)(&__MBAR[0x747]))
#define MCF_INTC_ICR08                       (*(vuint8 *)(&__MBAR[0x748]))
#define MCF_INTC_ICR09                       (*(vuint8 *)(&__MBAR[0x749]))
#define MCF_INTC_ICR10                       (*(vuint8 *)(&__MBAR[0x74A]))
#define MCF_INTC_ICR11                       (*(vuint8 *)(&__MBAR[0x74B]))
#define MCF_INTC_ICR12                       (*(vuint8 *)(&__MBAR[0x74C]))
#define MCF_INTC_ICR13                       (*(vuint8 *)(&__MBAR[0x74D]))
#define MCF_INTC_ICR14                       (*(vuint8 *)(&__MBAR[0x74E]))
#define MCF_INTC_ICR15                       (*(vuint8 *)(&__MBAR[0x74F]))
#define MCF_INTC_ICR16                       (*(vuint8 *)(&__MBAR[0x750]))
#define MCF_INTC_ICR17                       (*(vuint8 *)(&__MBAR[0x751]))
#define MCF_INTC_ICR18                       (*(vuint8 *)(&__MBAR[0x752]))
#define MCF_INTC_ICR19                       (*(vuint8 *)(&__MBAR[0x753]))
#define MCF_INTC_ICR20                       (*(vuint8 *)(&__MBAR[0x754]))
#define MCF_INTC_ICR21                       (*(vuint8 *)(&__MBAR[0x755]))
#define MCF_INTC_ICR22                       (*(vuint8 *)(&__MBAR[0x756]))
#define MCF_INTC_ICR23                       (*(vuint8 *)(&__MBAR[0x757]))
#define MCF_INTC_ICR24                       (*(vuint8 *)(&__MBAR[0x758]))
#define MCF_INTC_ICR25                       (*(vuint8 *)(&__MBAR[0x759]))
#define MCF_INTC_ICR26                       (*(vuint8 *)(&__MBAR[0x75A]))
#define MCF_INTC_ICR27                       (*(vuint8 *)(&__MBAR[0x75B]))
#define MCF_INTC_ICR28                       (*(vuint8 *)(&__MBAR[0x75C]))
#define MCF_INTC_ICR29                       (*(vuint8 *)(&__MBAR[0x75D]))
#define MCF_INTC_ICR30                       (*(vuint8 *)(&__MBAR[0x75E]))
#define MCF_INTC_ICR31                       (*(vuint8 *)(&__MBAR[0x75F]))
#define MCF_INTC_ICR32                       (*(vuint8 *)(&__MBAR[0x760]))
#define MCF_INTC_ICR33                       (*(vuint8 *)(&__MBAR[0x761]))
#define MCF_INTC_ICR34                       (*(vuint8 *)(&__MBAR[0x762]))
#define MCF_INTC_ICR35                       (*(vuint8 *)(&__MBAR[0x763]))
#define MCF_INTC_ICR36                       (*(vuint8 *)(&__MBAR[0x764]))
#define MCF_INTC_ICR37                       (*(vuint8 *)(&__MBAR[0x765]))
#define MCF_INTC_ICR38                       (*(vuint8 *)(&__MBAR[0x766]))
#define MCF_INTC_ICR39                       (*(vuint8 *)(&__MBAR[0x767]))
#define MCF_INTC_ICR40                       (*(vuint8 *)(&__MBAR[0x768]))
#define MCF_INTC_ICR41                       (*(vuint8 *)(&__MBAR[0x769]))
#define MCF_INTC_ICR42                       (*(vuint8 *)(&__MBAR[0x76A]))
#define MCF_INTC_ICR43                       (*(vuint8 *)(&__MBAR[0x76B]))
#define MCF_INTC_ICR44                       (*(vuint8 *)(&__MBAR[0x76C]))
#define MCF_INTC_ICR45                       (*(vuint8 *)(&__MBAR[0x76D]))
#define MCF_INTC_ICR46                       (*(vuint8 *)(&__MBAR[0x76E]))
#define MCF_INTC_ICR47                       (*(vuint8 *)(&__MBAR[0x76F]))
#define MCF_INTC_ICR48                       (*(vuint8 *)(&__MBAR[0x770]))
#define MCF_INTC_ICR49                       (*(vuint8 *)(&__MBAR[0x771]))
#define MCF_INTC_ICR50                       (*(vuint8 *)(&__MBAR[0x772]))
#define MCF_INTC_ICR51                       (*(vuint8 *)(&__MBAR[0x773]))
#define MCF_INTC_ICR52                       (*(vuint8 *)(&__MBAR[0x774]))
#define MCF_INTC_ICR53                       (*(vuint8 *)(&__MBAR[0x775]))
#define MCF_INTC_ICR54                       (*(vuint8 *)(&__MBAR[0x776]))
#define MCF_INTC_ICR55                       (*(vuint8 *)(&__MBAR[0x777]))
#define MCF_INTC_ICR56                       (*(vuint8 *)(&__MBAR[0x778]))
#define MCF_INTC_ICR57                       (*(vuint8 *)(&__MBAR[0x779]))
#define MCF_INTC_ICR58                       (*(vuint8 *)(&__MBAR[0x77A]))
#define MCF_INTC_ICR59                       (*(vuint8 *)(&__MBAR[0x77B]))
#define MCF_INTC_ICR60                       (*(vuint8 *)(&__MBAR[0x77C]))
#define MCF_INTC_ICR61                       (*(vuint8 *)(&__MBAR[0x77D]))
#define MCF_INTC_ICR62                       (*(vuint8 *)(&__MBAR[0x77E]))
#define MCF_INTC_ICR63                       (*(vuint8 *)(&__MBAR[0x77F]))
#define MCF_INTC_SWIACK                      (*(vuint8 *)(&__MBAR[0x7E0]))
#define MCF_INTC_L1IACK                      (*(vuint8 *)(&__MBAR[0x7E4]))
#define MCF_INTC_L2IACK                      (*(vuint8 *)(&__MBAR[0x7E8]))
#define MCF_INTC_L3IACK                      (*(vuint8 *)(&__MBAR[0x7EC]))
#define MCF_INTC_L4IACK                      (*(vuint8 *)(&__MBAR[0x7F0]))
#define MCF_INTC_L5IACK                      (*(vuint8 *)(&__MBAR[0x7F4]))
#define MCF_INTC_L6IACK                      (*(vuint8 *)(&__MBAR[0x7F8]))
#define MCF_INTC_L7IACK                      (*(vuint8 *)(&__MBAR[0x7FC]))
#define MCF_INTC_ICR(x)                      (*(vuint8 *)(&__MBAR[0x741 + ((x-1)*0x1)]))
#define MCF_INTC_LIACK(x)                    (*(vuint8 *)(&__MBAR[0x7E4 + ((x-1)*0x4)]))



/* Bit definitions and macros for MCF_INTC_IPRH */
#define MCF_INTC_IPRH_INT32                  (0x1)
#define MCF_INTC_IPRH_INT33                  (0x2)
#define MCF_INTC_IPRH_INT34                  (0x4)
#define MCF_INTC_IPRH_INT35                  (0x8)
#define MCF_INTC_IPRH_INT36                  (0x10)
#define MCF_INTC_IPRH_INT37                  (0x20)
#define MCF_INTC_IPRH_INT38                  (0x40)
#define MCF_INTC_IPRH_INT39                  (0x80)
#define MCF_INTC_IPRH_INT40                  (0x100)
#define MCF_INTC_IPRH_INT41                  (0x200)
#define MCF_INTC_IPRH_INT42                  (0x400)
#define MCF_INTC_IPRH_INT43                  (0x800)
#define MCF_INTC_IPRH_INT44                  (0x1000)
#define MCF_INTC_IPRH_INT45                  (0x2000)
#define MCF_INTC_IPRH_INT46                  (0x4000)
#define MCF_INTC_IPRH_INT47                  (0x8000)
#define MCF_INTC_IPRH_INT48                  (0x10000)
#define MCF_INTC_IPRH_INT49                  (0x20000)
#define MCF_INTC_IPRH_INT50                  (0x40000)
#define MCF_INTC_IPRH_INT51                  (0x80000)
#define MCF_INTC_IPRH_INT52                  (0x100000)
#define MCF_INTC_IPRH_INT53                  (0x200000)
#define MCF_INTC_IPRH_INT54                  (0x400000)
#define MCF_INTC_IPRH_INT55                  (0x800000)
#define MCF_INTC_IPRH_INT56                  (0x1000000)
#define MCF_INTC_IPRH_INT57                  (0x2000000)
#define MCF_INTC_IPRH_INT58                  (0x4000000)
#define MCF_INTC_IPRH_INT59                  (0x8000000)
#define MCF_INTC_IPRH_INT60                  (0x10000000)
#define MCF_INTC_IPRH_INT61                  (0x20000000)
#define MCF_INTC_IPRH_INT62                  (0x40000000)
#define MCF_INTC_IPRH_INT63                  (0x80000000)

/* Bit definitions and macros for MCF_INTC_IPRL */
#define MCF_INTC_IPRL_INT1                   (0x2)
#define MCF_INTC_IPRL_INT2                   (0x4)
#define MCF_INTC_IPRL_INT3                   (0x8)
#define MCF_INTC_IPRL_INT4                   (0x10)
#define MCF_INTC_IPRL_INT5                   (0x20)
#define MCF_INTC_IPRL_INT6                   (0x40)
#define MCF_INTC_IPRL_INT7                   (0x80)
#define MCF_INTC_IPRL_INT8                   (0x100)
#define MCF_INTC_IPRL_INT9                   (0x200)
#define MCF_INTC_IPRL_INT10                  (0x400)
#define MCF_INTC_IPRL_INT11                  (0x800)
#define MCF_INTC_IPRL_INT12                  (0x1000)
#define MCF_INTC_IPRL_INT13                  (0x2000)
#define MCF_INTC_IPRL_INT14                  (0x4000)
#define MCF_INTC_IPRL_INT15                  (0x8000)
#define MCF_INTC_IPRL_INT16                  (0x10000)
#define MCF_INTC_IPRL_INT17                  (0x20000)
#define MCF_INTC_IPRL_INT18                  (0x40000)
#define MCF_INTC_IPRL_INT19                  (0x80000)
#define MCF_INTC_IPRL_INT20                  (0x100000)
#define MCF_INTC_IPRL_INT21                  (0x200000)
#define MCF_INTC_IPRL_INT22                  (0x400000)
#define MCF_INTC_IPRL_INT23                  (0x800000)
#define MCF_INTC_IPRL_INT24                  (0x1000000)
#define MCF_INTC_IPRL_INT25                  (0x2000000)
#define MCF_INTC_IPRL_INT26                  (0x4000000)
#define MCF_INTC_IPRL_INT27                  (0x8000000)
#define MCF_INTC_IPRL_INT28                  (0x10000000)
#define MCF_INTC_IPRL_INT29                  (0x20000000)
#define MCF_INTC_IPRL_INT30                  (0x40000000)
#define MCF_INTC_IPRL_INT31                  (0x80000000)

/* Bit definitions and macros for MCF_INTC_IMRH */
#define MCF_INTC_IMRH_INT_MASK32             (0x1)
#define MCF_INTC_IMRH_INT_MASK33             (0x2)
#define MCF_INTC_IMRH_INT_MASK34             (0x4)
#define MCF_INTC_IMRH_INT_MASK35             (0x8)
#define MCF_INTC_IMRH_INT_MASK36             (0x10)
#define MCF_INTC_IMRH_INT_MASK37             (0x20)
#define MCF_INTC_IMRH_INT_MASK38             (0x40)
#define MCF_INTC_IMRH_INT_MASK39             (0x80)
#define MCF_INTC_IMRH_INT_MASK40             (0x100)
#define MCF_INTC_IMRH_INT_MASK41             (0x200)
#define MCF_INTC_IMRH_INT_MASK42             (0x400)
#define MCF_INTC_IMRH_INT_MASK43             (0x800)
#define MCF_INTC_IMRH_INT_MASK44             (0x1000)
#define MCF_INTC_IMRH_INT_MASK45             (0x2000)
#define MCF_INTC_IMRH_INT_MASK46             (0x4000)
#define MCF_INTC_IMRH_INT_MASK47             (0x8000)
#define MCF_INTC_IMRH_INT_MASK48             (0x10000)
#define MCF_INTC_IMRH_INT_MASK49             (0x20000)
#define MCF_INTC_IMRH_INT_MASK50             (0x40000)
#define MCF_INTC_IMRH_INT_MASK51             (0x80000)
#define MCF_INTC_IMRH_INT_MASK52             (0x100000)
#define MCF_INTC_IMRH_INT_MASK53             (0x200000)
#define MCF_INTC_IMRH_INT_MASK54             (0x400000)
#define MCF_INTC_IMRH_INT_MASK55             (0x800000)
#define MCF_INTC_IMRH_INT_MASK56             (0x1000000)
#define MCF_INTC_IMRH_INT_MASK57             (0x2000000)
#define MCF_INTC_IMRH_INT_MASK58             (0x4000000)
#define MCF_INTC_IMRH_INT_MASK59             (0x8000000)
#define MCF_INTC_IMRH_INT_MASK60             (0x10000000)
#define MCF_INTC_IMRH_INT_MASK61             (0x20000000)
#define MCF_INTC_IMRH_INT_MASK62             (0x40000000)
#define MCF_INTC_IMRH_INT_MASK63             (0x80000000)

/* Bit definitions and macros for MCF_INTC_IMRL */
#define MCF_INTC_IMRL_MASKALL                (0x1)
#define MCF_INTC_IMRL_INT_MASK1              (0x2)
#define MCF_INTC_IMRL_INT_MASK2              (0x4)
#define MCF_INTC_IMRL_INT_MASK3              (0x8)
#define MCF_INTC_IMRL_INT_MASK4              (0x10)
#define MCF_INTC_IMRL_INT_MASK5              (0x20)
#define MCF_INTC_IMRL_INT_MASK6              (0x40)
#define MCF_INTC_IMRL_INT_MASK7              (0x80)
#define MCF_INTC_IMRL_INT_MASK8              (0x100)
#define MCF_INTC_IMRL_INT_MASK9              (0x200)
#define MCF_INTC_IMRL_INT_MASK10             (0x400)
#define MCF_INTC_IMRL_INT_MASK11             (0x800)
#define MCF_INTC_IMRL_INT_MASK12             (0x1000)
#define MCF_INTC_IMRL_INT_MASK13             (0x2000)
#define MCF_INTC_IMRL_INT_MASK14             (0x4000)
#define MCF_INTC_IMRL_INT_MASK15             (0x8000)
#define MCF_INTC_IMRL_INT_MASK16             (0x10000)
#define MCF_INTC_IMRL_INT_MASK17             (0x20000)
#define MCF_INTC_IMRL_INT_MASK18             (0x40000)
#define MCF_INTC_IMRL_INT_MASK19             (0x80000)
#define MCF_INTC_IMRL_INT_MASK20             (0x100000)
#define MCF_INTC_IMRL_INT_MASK21             (0x200000)
#define MCF_INTC_IMRL_INT_MASK22             (0x400000)
#define MCF_INTC_IMRL_INT_MASK23             (0x800000)
#define MCF_INTC_IMRL_INT_MASK24             (0x1000000)
#define MCF_INTC_IMRL_INT_MASK25             (0x2000000)
#define MCF_INTC_IMRL_INT_MASK26             (0x4000000)
#define MCF_INTC_IMRL_INT_MASK27             (0x8000000)
#define MCF_INTC_IMRL_INT_MASK28             (0x10000000)
#define MCF_INTC_IMRL_INT_MASK29             (0x20000000)
#define MCF_INTC_IMRL_INT_MASK30             (0x40000000)
#define MCF_INTC_IMRL_INT_MASK31             (0x80000000)

/* Bit definitions and macros for MCF_INTC_INTFRCH */
#define MCF_INTC_INTFRCH_INTFRC32            (0x1)
#define MCF_INTC_INTFRCH_INTFRC33            (0x2)
#define MCF_INTC_INTFRCH_INTFRC34            (0x4)
#define MCF_INTC_INTFRCH_INTFRC35            (0x8)
#define MCF_INTC_INTFRCH_INTFRC36            (0x10)
#define MCF_INTC_INTFRCH_INTFRC37            (0x20)
#define MCF_INTC_INTFRCH_INTFRC38            (0x40)
#define MCF_INTC_INTFRCH_INTFRC39            (0x80)
#define MCF_INTC_INTFRCH_INTFRC40            (0x100)
#define MCF_INTC_INTFRCH_INTFRC41            (0x200)
#define MCF_INTC_INTFRCH_INTFRC42            (0x400)
#define MCF_INTC_INTFRCH_INTFRC43            (0x800)
#define MCF_INTC_INTFRCH_INTFRC44            (0x1000)
#define MCF_INTC_INTFRCH_INTFRC45            (0x2000)
#define MCF_INTC_INTFRCH_INTFRC46            (0x4000)
#define MCF_INTC_INTFRCH_INTFRC47            (0x8000)
#define MCF_INTC_INTFRCH_INTFRC48            (0x10000)
#define MCF_INTC_INTFRCH_INTFRC49            (0x20000)
#define MCF_INTC_INTFRCH_INTFRC50            (0x40000)
#define MCF_INTC_INTFRCH_INTFRC51            (0x80000)
#define MCF_INTC_INTFRCH_INTFRC52            (0x100000)
#define MCF_INTC_INTFRCH_INTFRC53            (0x200000)
#define MCF_INTC_INTFRCH_INTFRC54            (0x400000)
#define MCF_INTC_INTFRCH_INTFRC55            (0x800000)
#define MCF_INTC_INTFRCH_INTFRC56            (0x1000000)
#define MCF_INTC_INTFRCH_INTFRC57            (0x2000000)
#define MCF_INTC_INTFRCH_INTFRC58            (0x4000000)
#define MCF_INTC_INTFRCH_INTFRC59            (0x8000000)
#define MCF_INTC_INTFRCH_INTFRC60            (0x10000000)
#define MCF_INTC_INTFRCH_INTFRC61            (0x20000000)
#define MCF_INTC_INTFRCH_INTFRC62            (0x40000000)
#define MCF_INTC_INTFRCH_INTFRC63            (0x80000000)

/* Bit definitions and macros for MCF_INTC_INTFRCL */
#define MCF_INTC_INTFRCL_INTFRC1             (0x2)
#define MCF_INTC_INTFRCL_INTFRC2             (0x4)
#define MCF_INTC_INTFRCL_INTFRC3             (0x8)
#define MCF_INTC_INTFRCL_INTFRC4             (0x10)
#define MCF_INTC_INTFRCL_INTFRC5             (0x20)
#define MCF_INTC_INTFRCL_INTFRC6             (0x40)
#define MCF_INTC_INTFRCL_INTFRC7             (0x80)
#define MCF_INTC_INTFRCL_INTFRC8             (0x100)
#define MCF_INTC_INTFRCL_INTFRC9             (0x200)
#define MCF_INTC_INTFRCL_INTFRC10            (0x400)
#define MCF_INTC_INTFRCL_INTFRC11            (0x800)
#define MCF_INTC_INTFRCL_INTFRC12            (0x1000)
#define MCF_INTC_INTFRCL_INTFRC13            (0x2000)
#define MCF_INTC_INTFRCL_INTFRC14            (0x4000)
#define MCF_INTC_INTFRCL_INTFRC15            (0x8000)
#define MCF_INTC_INTFRCL_INTFRC16            (0x10000)
#define MCF_INTC_INTFRCL_INTFRC17            (0x20000)
#define MCF_INTC_INTFRCL_INTFRC18            (0x40000)
#define MCF_INTC_INTFRCL_INTFRC19            (0x80000)
#define MCF_INTC_INTFRCL_INTFRC20            (0x100000)
#define MCF_INTC_INTFRCL_INTFRC21            (0x200000)
#define MCF_INTC_INTFRCL_INTFRC22            (0x400000)
#define MCF_INTC_INTFRCL_INTFRC23            (0x800000)
#define MCF_INTC_INTFRCL_INTFRC24            (0x1000000)
#define MCF_INTC_INTFRCL_INTFRC25            (0x2000000)
#define MCF_INTC_INTFRCL_INTFRC26            (0x4000000)
#define MCF_INTC_INTFRCL_INTFRC27            (0x8000000)
#define MCF_INTC_INTFRCL_INTFRC28            (0x10000000)
#define MCF_INTC_INTFRCL_INTFRC29            (0x20000000)
#define MCF_INTC_INTFRCL_INTFRC30            (0x40000000)
#define MCF_INTC_INTFRCL_INTFRC31            (0x80000000)

/* Bit definitions and macros for MCF_INTC_IRLR */
#define MCF_INTC_IRLR_IRQ(x)                 (((x)&0x7F)<<0x1)

/* Bit definitions and macros for MCF_INTC_IACKLPR */
#define MCF_INTC_IACKLPR_PRI(x)              (((x)&0xF)<<0)
#define MCF_INTC_IACKLPR_LEVEL(x)            (((x)&0x7)<<0x4)

/* Bit definitions and macros for MCF_INTC_ICR */
#define MCF_INTC_ICR_IP(x)                   (((x)&0x7)<<0)
#define MCF_INTC_ICR_IL(x)                   (((x)&0x7)<<0x3)

/* Bit definitions and macros for MCF_INTC_SWIACK */
#define MCF_INTC_SWIACK_VECTOR(x)            (((x)&0xFF)<<0)

/* Bit definitions and macros for MCF_INTC_LIACK */
#define MCF_INTC_LIACK_VECTOR(x)             (((x)&0xFF)<<0)

/*********************************************************************
*
* System Integration Unit (SIU)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_SIU_SBCR                         (*(vuint32*)(&__MBAR[0x10]))
#define MCF_SIU_SECSACR                      (*(vuint32*)(&__MBAR[0x38]))
#define MCF_SIU_RSR                          (*(vuint32*)(&__MBAR[0x44]))
#define MCF_SIU_JTAGID                       (*(vuint32*)(&__MBAR[0x50]))


/* Bit definitions and macros for MCF_SIU_SBCR */
#define MCF_SIU_SBCR_PIN2DSPI                (0x8000000)
#define MCF_SIU_SBCR_DMA2CPU                 (0x10000000)
#define MCF_SIU_SBCR_CPU2DMA                 (0x20000000)
#define MCF_SIU_SBCR_PIN2DMA                 (0x40000000)
#define MCF_SIU_SBCR_PIN2CPU                 (0x80000000)

/* Bit definitions and macros for MCF_SIU_SECSACR */
#define MCF_SIU_SECSACR_SEQEN                (0x1)

/* Bit definitions and macros for MCF_SIU_RSR */
#define MCF_SIU_RSR_RST                      (0x1)
#define MCF_SIU_RSR_RSTWD                    (0x2)
#define MCF_SIU_RSR_RSTJTG                   (0x8)

/* Bit definitions and macros for MCF_SIU_JTAGID */
#define MCF_SIU_JTAGID_REV          (0xF0000000)
#define MCF_SIU_JTAGID_PROCESSOR    (0x0FFFFFFF)
#define MCF_SIU_JTAGID_MCF5485      (0x0800C01D)
#define MCF_SIU_JTAGID_MCF5484      (0x0800D01D)
#define MCF_SIU_JTAGID_MCF5483      (0x0800E01D)
#define MCF_SIU_JTAGID_MCF5482      (0x0800F01D)
#define MCF_SIU_JTAGID_MCF5481      (0x0801001D)
#define MCF_SIU_JTAGID_MCF5480      (0x0801101D)
#define MCF_SIU_JTAGID_MCF5475      (0x0801201D)
#define MCF_SIU_JTAGID_MCF5474      (0x0801301D)
#define MCF_SIU_JTAGID_MCF5473      (0x0801401D)
#define MCF_SIU_JTAGID_MCF5472      (0x0801501D)
#define MCF_SIU_JTAGID_MCF5471      (0x0801601D)
#define MCF_SIU_JTAGID_MCF5470      (0x0801701D)

/*********************************************************************
*
* FlexCAN Module (CAN)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CAN_CANMCR0        (*(vuint32*)(void*)(&__MBAR[0x00A000]))
#define MCF_CAN_CANCTRL0       (*(vuint32*)(void*)(&__MBAR[0x00A004]))
#define MCF_CAN_TIMER0         (*(vuint32*)(void*)(&__MBAR[0x00A008]))
#define MCF_CAN_RXGMASK0       (*(vuint32*)(void*)(&__MBAR[0x00A010]))
#define MCF_CAN_RX14MASK0      (*(vuint32*)(void*)(&__MBAR[0x00A014]))
#define MCF_CAN_RX15MASK0      (*(vuint32*)(void*)(&__MBAR[0x00A018]))
#define MCF_CAN_ERRCNT0        (*(vuint32*)(void*)(&__MBAR[0x00A01C]))
#define MCF_CAN_ERRSTAT0       (*(vuint32*)(void*)(&__MBAR[0x00A020]))
#define MCF_CAN_IMASK0         (*(vuint16*)(void*)(&__MBAR[0x00A02A]))
#define MCF_CAN_IFLAG0         (*(vuint16*)(void*)(&__MBAR[0x00A032]))
#define MCF_CAN_CANMCR1        (*(vuint32*)(void*)(&__MBAR[0x00A800]))
#define MCF_CAN_CANCTRL1       (*(vuint32*)(void*)(&__MBAR[0x00A804]))
#define MCF_CAN_TIMER1         (*(vuint32*)(void*)(&__MBAR[0x00A808]))
#define MCF_CAN_RXGMASK1       (*(vuint32*)(void*)(&__MBAR[0x00A810]))
#define MCF_CAN_RX14MASK1      (*(vuint32*)(void*)(&__MBAR[0x00A814]))
#define MCF_CAN_RX15MASK1      (*(vuint32*)(void*)(&__MBAR[0x00A818]))
#define MCF_CAN_ERRCNT1        (*(vuint32*)(void*)(&__MBAR[0x00A81C]))
#define MCF_CAN_ERRSTAT1       (*(vuint32*)(void*)(&__MBAR[0x00A820]))
#define MCF_CAN_IMASK1         (*(vuint16*)(void*)(&__MBAR[0x00A82A]))
#define MCF_CAN_IFLAG1         (*(vuint16*)(void*)(&__MBAR[0x00A832]))
#define MCF_CAN_CANMCR(x)      (*(vuint32*)(void*)(&__MBAR[0x00A000+((x)*0x800)]))
#define MCF_CAN_CANCTRL(x)     (*(vuint32*)(void*)(&__MBAR[0x00A004+((x)*0x800)]))
#define MCF_CAN_TIMER(x)       (*(vuint32*)(void*)(&__MBAR[0x00A008+((x)*0x800)]))
#define MCF_CAN_RXGMASK(x)     (*(vuint32*)(void*)(&__MBAR[0x00A010+((x)*0x800)]))
#define MCF_CAN_RX14MASK(x)    (*(vuint32*)(void*)(&__MBAR[0x00A014+((x)*0x800)]))
#define MCF_CAN_RX15MASK(x)    (*(vuint32*)(void*)(&__MBAR[0x00A018+((x)*0x800)]))
#define MCF_CAN_ERRCNT(x)      (*(vuint32*)(void*)(&__MBAR[0x00A01C+((x)*0x800)]))
#define MCF_CAN_ERRSTAT(x)     (*(vuint32*)(void*)(&__MBAR[0x00A020+((x)*0x800)]))
#define MCF_CAN_IMASK(x)       (*(vuint16*)(void*)(&__MBAR[0x00A02A+((x)*0x800)]))
#define MCF_CAN_IFLAG(x)       (*(vuint16*)(void*)(&__MBAR[0x00A032+((x)*0x800)]))

/* Bit definitions and macros for MCF_CAN_CANMCR */
#define MCF_CAN_CANMCR_MAXMB(x)            (((x)&0x0000000F)<<0)
#define MCF_CAN_CANMCR_SUPV                (0x00800000)
#define MCF_CAN_CANMCR_FRZACK              (0x01000000)
#define MCF_CAN_CANMCR_SOFTRST             (0x02000000)
#define MCF_CAN_CANMCR_HALT                (0x10000000)
#define MCF_CAN_CANMCR_FRZ                 (0x40000000)
#define MCF_CAN_CANMCR_MDIS                (0x80000000)

/* Bit definitions and macros for MCF_CAN_CANCTRL */
#define MCF_CAN_CANCTRL_PROPSEG(x)         (((x)&0x00000007)<<0)
#define MCF_CAN_CANCTRL_LOM                (0x00000008)
#define MCF_CAN_CANCTRL_LBUF               (0x00000010)
#define MCF_CAN_CANCTRL_TSYNC              (0x00000020)
#define MCF_CAN_CANCTRL_BOFFREC            (0x00000040)
#define MCF_CAN_CANCTRL_SAMP               (0x00000080)
#define MCF_CAN_CANCTRL_LPB                (0x00001000)
#define MCF_CAN_CANCTRL_CLKSRC             (0x00002000)
#define MCF_CAN_CANCTRL_ERRMSK             (0x00004000)
#define MCF_CAN_CANCTRL_BOFFMSK            (0x00008000)
#define MCF_CAN_CANCTRL_PSEG2(x)           (((x)&0x00000007L)<<16)
#define MCF_CAN_CANCTRL_PSEG1(x)           (((x)&0x00000007L)<<19)
#define MCF_CAN_CANCTRL_RJW(x)             (((x)&0x00000003L)<<22)
#define MCF_CAN_CANCTRL_PRESDIV(x)         (((x)&0x000000FFL)<<24)

/* Bit definitions and macros for MCF_CAN_TIMER */
#define MCF_CAN_TIMER_TIMER(x)             (((x)&0x0000FFFF)<<0)

/* Bit definitions and macros for MCF_CAN_RXGMASK */
#define MCF_CAN_RXGMASK_MI(x)              (((x)&0x1FFFFFFF)<<0)

/* Bit definitions and macros for MCF_CAN_RX14MASK */
#define MCF_CAN_RX14MASK_MI(x)             (((x)&0x1FFFFFFF)<<0)

/* Bit definitions and macros for MCF_CAN_RX15MASK */
#define MCF_CAN_RX15MASK_MI(x)             (((x)&0x1FFFFFFF)<<0)

/* Bit definitions and macros for MCF_CAN_ERRCNT */
#define MCF_CAN_ERRCNT_TXECTR(x)           (((x)&0x000000FF)<<0)
#define MCF_CAN_ERRCNT_RXECTR(x)           (((x)&0x000000FF)<<8)

/* Bit definitions and macros for MCF_CAN_ERRSTAT */
#define MCF_CAN_ERRSTAT_WAKINT             (0x00000001)
#define MCF_CAN_ERRSTAT_ERRINT             (0x00000002)
#define MCF_CAN_ERRSTAT_BOFFINT            (0x00000004)
#define MCF_CAN_ERRSTAT_FLTCONF(x)         (((x)&0x00000003)<<4)
#define MCF_CAN_ERRSTAT_TXRX               (0x00000040)
#define MCF_CAN_ERRSTAT_IDLE               (0x00000080)
#define MCF_CAN_ERRSTAT_RXWRN              (0x00000100)
#define MCF_CAN_ERRSTAT_TXWRN              (0x00000200)
#define MCF_CAN_ERRSTAT_STFERR             (0x00000400)
#define MCF_CAN_ERRSTAT_FRMERR             (0x00000800)
#define MCF_CAN_ERRSTAT_CRCERR             (0x00001000)
#define MCF_CAN_ERRSTAT_ACKERR             (0x00002000)
#define MCF_CAN_ERRSTAT_BITERR(x)          (((x)&0x00000003)<<14)
#define MCF_CAN_ERRSTAT_FLTCONF_ACTIVE     (0x00000000)
#define MCF_CAN_ERRSTAT_FLTCONF_PASSIVE    (0x00000010)
#define MCF_CAN_ERRSTAT_FLTCONF_BUSOFF     (0x00000020)

/* Bit definitions and macros for MCF_CAN_IMASK */
#define MCF_CAN_IMASK_BUF0M                (0x0001)
#define MCF_CAN_IMASK_BUF1M                (0x0002)
#define MCF_CAN_IMASK_BUF2M                (0x0004)
#define MCF_CAN_IMASK_BUF3M                (0x0008)
#define MCF_CAN_IMASK_BUF4M                (0x0010)
#define MCF_CAN_IMASK_BUF5M                (0x0020)
#define MCF_CAN_IMASK_BUF6M                (0x0040)
#define MCF_CAN_IMASK_BUF7M                (0x0080)
#define MCF_CAN_IMASK_BUF8M                (0x0100)
#define MCF_CAN_IMASK_BUF9M                (0x0200)
#define MCF_CAN_IMASK_BUF10M               (0x0400)
#define MCF_CAN_IMASK_BUF11M               (0x0800)
#define MCF_CAN_IMASK_BUF12M               (0x1000)
#define MCF_CAN_IMASK_BUF13M               (0x2000)
#define MCF_CAN_IMASK_BUF14M               (0x4000)
#define MCF_CAN_IMASK_BUF15M               (0x8000)

/* Bit definitions and macros for MCF_CAN_IFLAG */
#define MCF_CAN_IFLAG_BUF0I                (0x0001)
#define MCF_CAN_IFLAG_BUF1I                (0x0002)
#define MCF_CAN_IFLAG_BUF2I                (0x0004)
#define MCF_CAN_IFLAG_BUF3I                (0x0008)
#define MCF_CAN_IFLAG_BUF4I                (0x0010)
#define MCF_CAN_IFLAG_BUF5I                (0x0020)
#define MCF_CAN_IFLAG_BUF6I                (0x0040)
#define MCF_CAN_IFLAG_BUF7I                (0x0080)
#define MCF_CAN_IFLAG_BUF8I                (0x0100)
#define MCF_CAN_IFLAG_BUF9I                (0x0200)
#define MCF_CAN_IFLAG_BUF10I               (0x0400)
#define MCF_CAN_IFLAG_BUF11I               (0x0800)
#define MCF_CAN_IFLAG_BUF12I               (0x1000)
#define MCF_CAN_IFLAG_BUF13I               (0x2000)
#define MCF_CAN_IFLAG_BUF14I               (0x4000)
#define MCF_CAN_IFLAG_BUF15I               (0x8000)

/* Buffer macros (they were missing in dBUG sources) */
#define MCF_CAN_MBUF_CTRL(x,y)  (*(vuint16*)(void*)(&__MBAR[0x00A080+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_TMSTP(x,y) (*(vuint16*)(void*)(&__MBAR[0x00A082+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_ID(x,y)    (*(vuint32*)(void*)(&__MBAR[0x00A084+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_BYTE0(x,y) (*(vuint8*)(void*)(&__MBAR[0x00A088+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_BYTE1(x,y) (*(vuint8*)(void*)(&__MBAR[0x00A089+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_BYTE2(x,y) (*(vuint8*)(void*)(&__MBAR[0x00A08A+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_BYTE3(x,y) (*(vuint8*)(void*)(&__MBAR[0x00A08B+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_BYTE4(x,y) (*(vuint8*)(void*)(&__MBAR[0x00A08C+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_BYTE5(x,y) (*(vuint8*)(void*)(&__MBAR[0x00A08D+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_BYTE6(x,y) (*(vuint8*)(void*)(&__MBAR[0x00A08E+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_BYTE7(x,y) (*(vuint8*)(void*)(&__MBAR[0x00A08F+((x)*0x800)+((y)*0x10)]))
#define MCF_CAN_MBUF_CTRL_LENGTH(x) ((x)&0x000FL)

/* message buffer cfg bits */

#define MBOX_RXCODE_NOT_ACTIVE 0x00
#define MBOX_RXCODE_BUSY       0x01
#define MBOX_RXCODE_EMPTY      0x04
#define MBOX_RXCODE_FULL       0x02
#define MBOX_RXCODE_OVERRUN    0x06
#define MBOX_TXCODE_NOT_READY  0x08
#define MBOX_TXCODE_RESPONSE   0x0A
#define MBOX_TXCODE_TRANSMIT   0x0C

/* Bit definitions and macros for MCF_CAN_MBUF_CTRL */
#define MCF_CAN_MBUF_CTRL_CODE(x) (((x)&0x000FL)<<8)

/* Bit definitions and macros for MCF_CAN_MBUF_ID */
#define MCF_CAN_MBUF_ID_STD(x) (((x)&0x000007FFL)<<18)
#define MCF_CAN_MBUF_ID_EXT(x) (((x)&0x0003FFFFL)<<0)

/* FIXME: check bits */
#define MBOX_DATA_FRAME        0x00 /* data frame */
#define MBOX_REMOTE_FRAME      0x01 /* remote frame */
#define MBOX_STD_ID            0x00 /* standard identifier */
#define MBOX_EXT_ID            0x01 /* remote identifier */
#define MBOX_TX                0x08 /* tx message box */
#define MBOX_RX                0x00 /* rx messge box */

#define MBOX_CFG_IDE           0x08
#define MBOX_CFG_RTR_EXT       0x01
#define MBOX_CFG_RTR_STD       0x10
#define MBOX_CFG_SSR           0x10
#define MBOX_CFG_DLC_MASK      0x0F
#define MBOX_CFG_STAT_MASK     0xF0

/*********************************************************************
*
* M548X specific defines
*
*********************************************************************/

#ifdef MACHINE_M548X

#if CONF_WITH_BAS_MEMORY_MAP
#define FIRE_ENGINE_CS4_BASE 0x00000000L
#define FIRE_ENGINE_CS5_BASE 0x60000000L
#else
#define FIRE_ENGINE_CS4_BASE 0xC0000000L
#define FIRE_ENGINE_CS5_BASE 0xE0000000L
#endif /* CONF_WITH_BAS_MEMORY_MAP */

#define FIRE_ENGINE_CS4_SIZE 0x10000000L
#define FIRE_ENGINE_CS4_ACCESS \
        (MCF_FBCS_CSCR_ASET(1) + MCF_FBCS_CSCR_WS(25) + MCF_FBCS_CSCR_AA + MCF_FBCS_CSCR_PS_16) /* 0x106580 */

#define FIRE_ENGINE_CS5_SIZE 0x10000000L
#define FIRE_ENGINE_CS5_ACCESS \
        (MCF_FBCS_CSCR_ASET(1) + MCF_FBCS_CSCR_WS(10) + MCF_FBCS_CSCR_AA + MCF_FBCS_CSCR_PS_16) /* 0x102980 */

#define FIRE_ENGINE_CPLD_HW_REVISION (FIRE_ENGINE_CS5_BASE + 0x08000000)
#define FIRE_ENGINE_CPLD_SW_REVISION (FIRE_ENGINE_CS5_BASE + 0x07000000)
#define FIRE_ENGINE_CPLD_PWRMGT (FIRE_ENGINE_CS5_BASE + 0x04000000)

#define COMPACTFLASH_BASE (FIRE_ENGINE_CS5_BASE + 0x0A000000)

#endif /* MACHINE_M548X */

#endif /* COLDPRIV_H */
