#ifndef INTERRUPTS_H_ABZSEOYR
#define INTERRUPTS_H_ABZSEOYR

#include "Arduino.h"

namespace InterruptGlobals {
    typedef void (*InterruptHandler_t)(void);
    inline InterruptHandler_t ADCInterrupt;

__attribute__((noinline)) void setADCInterruptHandler(const InterruptHandler_t callback);
}

#endif /* end of include guard: INTERRUPTS_H_ABZSEOYR */
