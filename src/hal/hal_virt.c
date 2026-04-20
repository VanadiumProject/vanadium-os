#include "hardware.h"
#include "kernel.h"
#include "types.h"
#include "hal.h"

#define UART_RBR  REG8(UART0_BASE + 0x00)
#define UART_THR  REG8(UART0_BASE + 0x00)
#define UART_LSR  REG8(UART0_BASE + 0x05)

void virt_uart_putc(char c){
	while (!(UART_LSR & (1 << 5)));
	UART_THR = c;
}

void virt_uart_puts(const char * s){
	while (*s) virt_uart_putc(*s++);
}

ui8_t virt_uart_getc(void){
	while (!(UART_LSR & (1 << 0)));
	return UART_RBR;
}

const uart_hal_t virt_hal = {
	.uart_putc = virt_uart_putc,
	.uart_puts = virt_uart_puts,
	.uart_getc = virt_uart_getc
};
const uart_hal_t* get_hal(void){
	return &virt_hal;
}
