#include "ktime.h"
#include "hal.h"

ui64_t ktime_ticks(void) {
	ui64_t t;

	__asm__ volatile ("rdtime %0" : "=r"(t));
	return t;
}

ui64_t ktime_ms(void) {
	// Multiply before dividing so a tick rate that is not a round multiple
	// of 1000 still converts exactly. The product only overflows after
	// decades of uptime, which is not a session this console will have.
	return (ktime_ticks() * 1000) / get_timebase_hz();
}
