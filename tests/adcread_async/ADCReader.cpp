#include "ADCReader.h"

// Largely from, http://www.gammon.com.au/interrupts

static void ADCReader::interruptRoutine(){
    byte low, high;
    // Order matters here, ACDL then ADCH here
    low = ADCL;
    high = ADCH;
    adcReading = high << 8 | low;
    adcDone = true
}

void ADCReader::run(){
    if (!adcReading) {
        started = false;
    } else {
        //Start reading here
        adcStarted = true;
        ISR(ADC_VECT);
    }
}
