#ifndef ADCREADER_H
#define ADCREADER_H

#include <avr/interrupt.h>

namespace InterruptGlobals
{
    typedef void (*InterruptHandler_t)(void);
    const InterruptHandler_t NOOP_INTERRUPT(){return;};

} /* InterruptGlobals */

enum class ADCStatus {
    Finished,
    Idle,
    Error,
    Busy
};

namespace Globals {
    constexpr auto ADCSampleBits = 10;

}

class ADCReader
{
    typedef void (ADCReader::*ADCISRHandler_t)(void) ;
    typedef uint8_t ADCPin_t;

private:
    volatile bool adcFinished;
    static InterruptGlobals::InterruptHandler_t ADCInterrupt = InterruptGlobals::NOOP_INTERRUPT;
    ADCPin_t pinNo_;
    bool inProgress;
    volatile uint16_t adcMeasurement;

public:
    ADCReader(const ADCPin_t pinNo) : pinNo_(pinNo){};
    virtual ~ADCReader();
    void init();

    // Prepare the Interrupt registers
    void startSingleShotADC();
    void startClockedADC();
    bool busy();

    // Use the registers in an ISR
    void ISR_finishedMeasurement();
    // void ISR_CopyADCRegisterTo(volatile uint16_t& measurement);

    void copyADCRegisterOut(uint16_t& measurementCopy );

    // Set the Interrupt Handler
    void setInterruptHandler(const InterruptGlobals::InterruptHandler_t callback);
    void poll();

    // Sample ISR
    static InterruptGlobals::InterruptHandler_t ISRReadADCRegister;

};

namespace  InterruptGloabls {
    void ADCISR_HANDLER_WRAPPER(){
        ADCReader::ADCInterrupt();
    }
} /*  InterruptGloabls */

#endif /* ADCREADER_H */

