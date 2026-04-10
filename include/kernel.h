#ifndef KERNEL_H
#define KERNEL_H

#include "hardware.h"

#define REG(addr) (*((volatile unsigned char *)(addr)))

#define UART_THR  REG(QEMU_UART_BASE + 0x00)
#define UART_LSR  REG(QEMU_UART_BASE + 0x05)


void uart_putc(char c);
void uart_puts(const char* s);
#endif


