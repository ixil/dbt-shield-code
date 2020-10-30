#include "interrupts.h"
#include "ADCAveragingFilter.h"
#include <util/atomic.h>

ADCAveragingFilter* ADCAveragingFilter::instance;

__attribute__((always_inline)) uint16_t readADC() {
    uint8_t low, high;
    // Order matters here, ACDL then ADCH here
    low = ADCL;
    high = ADCH;
    return high << 8 | low;
}

__attribute__((always_inline)) bool ADCMeasuring(){
    return (ADCSRA & bit(ADSC));
}

ADCAveragingFilter::ADCAveragingFilter(const ADCPin_t pinNo, const uint16_t initVal) : pinNo(pinNo), isValid{false}, idx{0}, sum{initVal*FilterLength}{
    ADCAveragingFilter::instance = this;
    ADCSRA &= ~(bit(ADPS0) | bit(ADPS1) | bit(ADPS2) | bit(ADIE)); // clear prescaler
    readADC();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        for (uint8_t i = 0; i < FilterLength; ++i) {
            readings[i] = initVal;
        }
    }
}

void ADCAveragingFilter::init() {
    InterruptGlobals::setADCInterruptHandler(&ADCAveragingFilter::AveragingISRHandler);
    isActive = false;
    // analogReference(EXTERNAL);
    ADCSRA = bit(ADEN); // turn on the ADC
    ADMUX = bit(REFS0); //AVcc
    // ADMUX =  bit(REFS1); //AVext
    ADMUX |= (pinNo & 0x07); // Select ADC MUX pin and set AREF
}

void ADCAveragingFilter::enableTriggering() {
    isActive=true;
    ADCSRA |= bit(ADATE);
    readADC();
}
void ADCAveragingFilter::disableTriggering() {
    isActive=false;
    ADCSRA &= ~bit(ADATE);
}

void ADCAveragingFilter::disableADC() {
    isActive=false;
    ADCSRA &= ~bit(ADEN); // Turn off the ADC
    ADCSRA &= bit(ADIF);
}

ADCAveragingFilter::~ADCAveragingFilter() {
    disableADC(); // Turn off the ADC
}

ADCStatus ADCAveragingFilter::getFilteredValue(uint12_t &averagedDecimatedValue) {
    if (isValid) {
        isValid = false;
        Serial.print(" decimating: ");
        Serial.print(getSum());
        Serial.print(" averaging using ");
        Serial.print(ADCAveragingFilter::AveragingSamples);
        Serial.print(" oversamples ");
        Serial.print(ADCAveragingFilter::OverSampler);
        Serial.print("  bits ");
        Serial.print(ADCAveragingFilter::ThermistorOverSampleBits);
        Serial.print(" Total samples: ");
        Serial.print(ADCAveragingFilter::FilterLength);
        averagedDecimatedValue = (getSum() / ADCAveragingFilter::AveragingSamples) >> ADCAveragingFilter::ThermistorOverSampleBits;
        return ADCStatus::Success;
    } else {
        if (ADCMeasuring())
            return ADCStatus::Busy;
        if (isActive)
            return ADCStatus::Active;
        else
            return ADCStatus::Idle;
    }
}

// Call this (in an ISR) to put a new reading into the filter
void ADCAveragingFilter::AveragingISRHandler() {
    const uint16_t r = readADC();
        (*instance).sum += r - (*instance).readings[instance->idx];
        (*instance).readings[instance->idx] = r;
        (*instance).idx += 1;
    if (instance->idx == FilterLength) {
        instance->idx = 0;
        instance->isValid = true;
    }
}

inline constexpr uint8_t logPrescale(const ADCPreScaler_t prescale){
    return 0 | (((prescale & 0xF0) !=0 ) << 2) |
        (((prescale & 0xCC) !=0 ) << 1) |
        ((prescale & 0xAA) !=0);
    // uint8_t log2prescale = 0;
    // log2prescale |= ((prescale & 0xF0) !=0 ) << 2;
    // log2prescale |= ((prescale & 0xCC) !=0 ) << 1;
    // log2prescale |= (prescale & 0xAA) !=0;
    // return log2prescale;
}

void ADCAveragingFilter::setSingleShotMode(const ADCPreScaler_t prescale) {
    isActive = true;
    if (!instance){
        Serial.println("instance not set correctly!");
        Serial.flush();
    }
    // ADCSRA |= bit(ADIE) | bit(ADEN) | logPrescale(prescale);

    ADCSRA &= ~bit(ADATE);
    ADCSRA |= bit(ADEN) | logPrescale(prescale) | bit(ADIE);
    // ADCSRA |= bit(ADEN) | logPrescale(prescale);
    readADC();
}

/*! Enables Free running ADC
 * Requires polling the device, the ADC interupt are disabled*/
void ADCAveragingFilter::startFreeRunningADC(const uint8_t prescale) {
    setupTriggeredADC(ADCTriggerSource::FreeRunning, prescale, false);
}

void ADCAveragingFilter::startConversion(){
    isActive = true;
    ADCSRA |= bit(ADSC);
}


/*! If interrupt is true, and FreeRunning is selected, startConversion must be used. */
void ADCAveragingFilter::setupTriggeredADC(const ADCTriggerSource source, const ADCPreScaler_t prescale, const bool interrupt) {
    if (!interrupt){
        if (source == ADCTriggerSource::FreeRunning)
            isActive = true;
        else
            isActive = false;
    }
    if (!instance){
        Serial.print("instance not set correctly!");
        abort();
    }
    ADCSRB = (0x07 & static_cast<uint8_t>(source));
    ADCSRA = (
            bit(ADEN) |
            bit(ADATE) | // Auto-trigger (set with ADTS in ADCSRB)
            (interrupt ? bit(ADIE) : 0) |
            // (interrupt ? 0 : bit(ADSC)) |
            logPrescale(prescale) | // static_cast<uint8_t>(logPrescale)
            bit(ADIF) /* & ~bit(ADSC) */ );  // unset last interrupt flag
}

uint16_t ADCAveragingFilter::getSum() const {
    uint16_t copy;
    // uint8_t tmp = SREG;
    // noInterrupts();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        copy=sum;
    }
    // SREG = tmp;
    return copy;
}

// Return true if we have a valid average
bool ADCAveragingFilter::valid() const{
    return isValid;
}

void ADCAveragingFilter::run(){
    // Start a new conversion, if we're not measuring
    if(!ADCMeasuring()) {
        startConversion();
    }
}


// void ADCAveragingFilter::copyADCRegisterOut(uint16_t& measurementCopy ){
//     uint8_t tmp = SREG;
//     noInterrupts();
//     measurementCopy=adcMeasurement;
//     SREG = tmp;
// }

// void ADCAveragingFilter::ISR_CopyADCRegisterTo(volatile uint16_t& measurement ){
//     byte low, high;
//     // Order matters here, ACDL then ADCH here
//     low = ADCL;
//     high = ADCH;
//     measurement = high << 8 | low;
// }

