#ifndef INTERRUPTS_H_ABZSEOYR
#define INTERRUPTS_H_ABZSEOYR

#include "Arduino.h"

namespace InterruptGlobals {
    typedef void (*InterruptHandler_t)(void);
    /* static InterruptHandler_t ADCInterrupt; */
    static void (*ADCInterrupt)(void);

__attribute__((noinline))  static void setADCInterruptHandler(const InterruptHandler_t callback){
      if (callback != nullptr) {
        ADCInterrupt = callback;
        Serial.print("adcint is in the seter:");
        Serial.print((uint16_t)(ADCInterrupt));
        Serial.print("Callback is no set to:");
        Serial.print((uint16_t)(callback));
      } else {
        Serial.print("Callback is NULL!");
        while(true);
      }
    };
}


#endif /* end of include guard: INTERRUPTS_H_ABZSEOYR */
