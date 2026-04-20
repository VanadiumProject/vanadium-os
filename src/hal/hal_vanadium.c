#include "hal.h"
#include "hardware.h"
#include "kernel.h"
#include "types.h"

#define UART_RBR   REG8(UART0_BASE + 0x00)
#define UART_THR   REG8(UART0_BASE + 0x00)
#define UART_LSR   REG8(UART0_BASE + 0x14)

void vanadium_uart_putc(char c){
	while (!(UART_LSR & (1 << 5)));
	UART_THR = c;
}

void vanadium_uart_puts(const char * s){
	while (*s) vanadium_uart_putc(*s++);
}

ui8_t vanadium_uart_getc(void){
	while (!(UART_LSR & (1 << 0)));
	return UART_RBR;
}

const uart_hal_t vanadium_hal = {
	.uart_putc = vanadium_uart_putc,
	.uart_puts = vanadium_uart_puts,
	.uart_getc = vanadium_uart_getc
};
const uart_hal_t* get_hal(void){
	return &vanadium_hal;
}
