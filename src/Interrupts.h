#ifndef INTERRUPTS_H_ABZSEOYR
#define INTERRUPTS_H_ABZSEOYR

#define TIMER_INTERRUPT_DEBUG      0

#define USE_TIMER_1     false
#define USE_TIMER_2     false
#define USE_TIMER_3     true
#define USE_TIMER_4     false
#define USE_TIMER_5     false

namespace InterruptGlobals {
    typedef void (*InterruptHandler_t)(void);
    inline InterruptHandler_t ADCInterrupt;

__attribute__((noinline))  void setADCInterruptHandler(const InterruptHandler_t callback){
    if (callback != nullptr) {
        ADCInterrupt = callback;
    }
}
}

#endif /* end of include guard: INTERRUPTS_H_ABZSEOYR */
