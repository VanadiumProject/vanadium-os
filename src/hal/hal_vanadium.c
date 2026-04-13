#include "hal.h"
#include "hardware.h"
#include "kernel.h"
#include "types.h"

#define USART1_RBR  REG8(USART1_BASE + 0x00)
#define USART1_THR  REG8(USART1_BASE + 0x00)
#define USART1_LSR  REG8(USART1_BASE + 0x05)

void vanadium_uart_putc(char c){
	while (!(USART1_LSR & (1 << 5)));
	USART1_THR = c;
}

void vanadium_uart_puts(const char * s){
	while (*s) vanadium_uart_putc(*s++);
}

ui8_t vanadium_uart_getc(void){
	while (!(USART1_LSR & (1 << 0)));
	return USART1_RBR;
}

const uart_hal_t vanadium_hal = {
	.uart_putc = vanadium_uart_putc,
	.uart_puts = vanadium_uart_puts,
	.uart_getc = vanadium_uart_getc
};
const uart_hal_t* get_hal(void){
	return &vanadium_hal;
}
