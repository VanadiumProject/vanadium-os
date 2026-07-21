#include "kprintf.h"
#include "hal.h"

// Longest thing we render is a 64 bit value in base 10 (20 digits).
#define KP_BUF	24

static void kp_pad(const uart_hal_t *hal, char c, i32_t count) {
	while (count-- > 0)
		hal->uart_putc(c);
}

// Renders v into buf backwards and returns how many digits it wrote. Backwards
// because that is the direction division hands them to us; the caller walks it
// in reverse. rv64gc has the M extension, so the 64 bit divide below is a real
// instruction and not a call into a compiler runtime we do not link.
static i32_t kp_utoa(ui64_t v, ui32_t base, i32_t upper, char *buf) {
	const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
	i32_t len = 0;

	do {
		buf[len++] = digits[v % base];
		v /= base;
	} while (v);

	return len;
}

void kvprintf(const char *fmt, va_list ap) {
	const uart_hal_t *hal = get_hal();
	char buf[KP_BUF];

	while (*fmt) {
		if (*fmt != '%') {
			hal->uart_putc(*fmt++);
			continue;
		}
		fmt++;

		// Only zero padding: it is what a register dump needs and nothing
		// else has earned its place yet.
		char pad = ' ';
		if (*fmt == '0') {
			pad = '0';
			fmt++;
		}

		i32_t width = 0;
		while (*fmt >= '0' && *fmt <= '9')
			width = width * 10 + (*fmt++ - '0');

		i32_t is_long = 0;
		while (*fmt == 'l') {
			is_long = 1;
			fmt++;
		}
		if (*fmt == 'z') {
			is_long = 1;
			fmt++;
		}

		ui64_t uval = 0;
		i32_t len = 0;
		i32_t neg = 0;

		switch (*fmt) {
			case 'c':
				buf[0] = (char)va_arg(ap, int);
				len = 1;
				break;

			case 's': {
				const char *s = va_arg(ap, const char *);
				i32_t sl = 0;

				if (!s)
					s = "(null)";
				while (s[sl])
					sl++;

				kp_pad(hal, ' ', width - sl);
				while (*s)
					hal->uart_putc(*s++);

				fmt++;
				continue;
			}

			case 'd':
			case 'i': {
				i64_t sv = is_long ? va_arg(ap, i64_t)
						   : (i64_t)va_arg(ap, i32_t);

				if (sv < 0) {
					neg = 1;
					// Negate in unsigned space so the most negative
					// value does not overflow on the way out.
					uval = -(ui64_t)sv;
				} else {
					uval = (ui64_t)sv;
				}
				len = kp_utoa(uval, 10, 0, buf);
				break;
			}

			case 'u':
				uval = is_long ? va_arg(ap, ui64_t)
					       : (ui64_t)va_arg(ap, ui32_t);
				len = kp_utoa(uval, 10, 0, buf);
				break;

			case 'x':
			case 'X':
				uval = is_long ? va_arg(ap, ui64_t)
					       : (ui64_t)va_arg(ap, ui32_t);
				len = kp_utoa(uval, 16, *fmt == 'X', buf);
				break;

			case 'p':
				// Pointers always come out full width; a half printed
				// address is worse than useless when you are lost.
				uval = (ui64_t)(uintptr_t)va_arg(ap, void *);
				hal->uart_puts("0x");
				len = kp_utoa(uval, 16, 0, buf);
				kp_pad(hal, '0', 16 - len);
				width = 0;	// already padded, don't do it twice
				break;

			case '%':
				buf[0] = '%';
				len = 1;
				break;

			case '\0':
				// Trailing '%' with nothing after it. Stop rather than
				// walking off the end of the string.
				return;

			default:
				// Unknown conversion: show it as written so the bug is
				// visible instead of silently swallowed.
				hal->uart_putc('%');
				hal->uart_putc(*fmt++);
				continue;
		}

		// Sign goes before zero padding but after space padding, otherwise
		// you get things like "00-42".
		if (pad == '0') {
			if (neg)
				hal->uart_putc('-');
			kp_pad(hal, '0', width - len - neg);
		} else {
			kp_pad(hal, ' ', width - len - neg);
			if (neg)
				hal->uart_putc('-');
		}

		while (len > 0)
			hal->uart_putc(buf[--len]);

		fmt++;
	}
}

void kprintf(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	kvprintf(fmt, ap);
	va_end(ap);
}
