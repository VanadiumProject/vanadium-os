#include "hal.h"
#include "hardware.h"
#include "kernel.h"

#define USART1_THR  REG(USART1_BASE + 0x00)
#define USART1_LSR  REG(USART1_BASE + 0x05)

void vanadium_uart_putc(char c){
	while (!(USART1_LSR & (1 << 5)));
	USART1_THR = c;
}

void vanadium_uart_puts(const char * s){
	while (*s) vanadium_uart_putc(*s++);
}

const uart_hal_t virt_hal = {
	.uart_putc = vanadium_uart_putc,
	.uart_puts = vanadium_uart_puts
};
const uart_hal_t* get_hal(void){
	return &virt_hal;
}
