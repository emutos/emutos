/* board/rpi0/rpiboot.c */
/*
 * EmuTOS for Raspberry Pi Zero
 *
 * This is a simple bootloader that prints a welcome message to the UART.
 * It is intended to be used with QEMU for testing purposes.
 *
 * Compile with:
 *   make rpi0
 *
 * Run with QEMU:
 *   qemu-system-arm -M raspi0 -cpu arm1176 -kernel kernel.img -nographic -device loader,file=kernel.img,addr=0x8000
 */

typedef unsigned int uint32_t;

/* Forward declaration of EmuTOS BIOS entrypoint */
extern void biosmain(void);


/* PL011 UART0 on Raspberry Pi Zero (peripheral base 0x20000000) */
#define UART0_BASE  0x20201000
#define UART0_DR    (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_FR    (*(volatile uint32_t *)(UART0_BASE + 0x18))

static void uart_putc(char c) {
    /* wait until TX FIFO not full */
    while (UART0_FR & (1 << 5)) { }
    UART0_DR = (uint32_t)c;
}

void _rpiboot(void) {
    const char *msg = "Welcome to emuTOS\n";
    for (const char *p = msg; *p; p++) {
        uart_putc(*p);
    }
    biosmain();
    /* hang here */
    while (1) { }
}