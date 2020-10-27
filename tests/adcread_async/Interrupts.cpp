#include "Interrupts.h"

namespace InterruptGlobals
{

__attribute__((noinline))  void setADCInterruptHandler(const InterruptHandler_t callback){
      if (callback != nullptr) {
        ADCInterrupt = callback;
      }
    }

} /* InterruptGlobals */ 
