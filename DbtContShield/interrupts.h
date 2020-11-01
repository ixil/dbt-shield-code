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

__attribute__((noinline))  void setADCInterruptHandler(const InterruptHandler_t callback);
}


/* 
   ISR(TIMER4_COMPA_vect){
   STEP_PORT ^= 1 << STEPPER1_BIT_POS; //Toggle the bit, using dedge
   if (speedChange[1]){
   updateStepper1Speed();
   }
   }
   */


#endif /* end of include guard: INTERRUPTS_H_ABZSEOYR */
