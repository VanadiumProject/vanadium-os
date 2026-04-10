#include "kernel.h"
#include "hardware.h"

void uart_putc(char c){
	while (!(UART_LSR & (1 << 5)));
	UART_THR = c;
}

void uart_puts(const char * s){
	while (*s) uart_putc(*s++);
}

void kmain(void) {
	uart_puts("Yes, I am alive!\n");
	while (1);
}
