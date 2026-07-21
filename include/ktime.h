#ifndef KTIME_H
#define KTIME_H

#include "types.h"

// Time since reset, from the free running counter the architecture gives us.
//
// rdtime is architectural, so it is the same instruction on the emulator and
// on the board and does not belong in the HAL. Only the tick rate differs
// between them, and that is what the HAL answers.
//
// This is a counter, not a timer: it costs no interrupt, no SBI call and no
// setup, which is why logging can have timestamps long before scheduling can
// have preemption.
ui64_t ktime_ticks(void);
ui64_t ktime_ms(void);

#endif
