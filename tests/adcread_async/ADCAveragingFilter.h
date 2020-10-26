#ifndef ADCAVERAGINGFILTER_H_KYQFO5WM
#define ADCAVERAGINGFILTER_H_KYQFO5WM

#include "Arduino.h"

enum class ADCStatus {
    Success,
    Busy,
    Active,
    Idle,
    Error
};

enum class ADCTriggerSource: uint8_t {
    FreeRunning=0x0,
    AnalogComparator=0x1,
    ExternalInterruptRequest=0x2,
    Counter0CompareA=0x3,
    Counter0Overflow=0x4,
    Counter1CompareB=0x5,
    Counter1Overflow=0x6,
    Counter1Capture=0x7
};

/*! Prescale can be 2,4,8,16,32,64,128 */
typedef uint8_t ADCPreScaler_t;


class ADCAveragingFilter {
    public:
        typedef void (ADCAveragingFilter::*ADCISRHandler_t)(void) ;
        typedef uint8_t ADCPin_t;
        typedef uint16_t uint12_t;

    public:
        // Max 10bit samples that can fit into a uint6_t is 2^6.
        // 4^2 samples used for Oversampling and Decimation, and 4 samples used for averaging them
        static constexpr uint8_t ThermistorOverSampleBits = 2;
        static constexpr uint8_t AveragingSamples = 4;
        static constexpr uint8_t FilterLength = AveragingSamples * 4 << 2*(ThermistorOverSampleBits-1) ;

    // private:
        ADCPin_t pinNo;

        bool isActive;

        volatile uint16_t sum;
        volatile uint8_t idx;
        volatile uint16_t readings[FilterLength];
        volatile bool isValid;

        static ADCAveragingFilter *instance;

    public:
        ADCAveragingFilter(){};
        ADCAveragingFilter(const ADCPin_t pinNo, const uint16_t initVal);
        // ADCAveragingFilter& getInstance();
        void disableADC();
        void disableTriggering();
        void enableTriggering();

        // Due to singleton
        ADCAveragingFilter(ADCAveragingFilter const&) = delete;
        void operator= (ADCAveragingFilter const&)  = delete;

        ADCStatus getFilteredValue(uint12_t &filteredValue);
        ~ADCAveragingFilter();

        void init();

        // Prepare the Interrupt registers
        static void AveragingISRHandler();
        void startConversion();
        void startClockedADC();
        void setSingleShotMode(const ADCPreScaler_t prescale=64);
        void setupTriggeredADC(const ADCTriggerSource source, const ADCPreScaler_t prescale, const bool interrupt=false);
        void startFreeRunningADC(const uint8_t prescale=64);

        void run();

        // Return true if we have enough samples
        const bool valid() const;

        // Return the raw sum
        uint16_t getSum() const;
};
#endif /* end of include guard: ADCAVERAGINGFILTER_H_KYQFO5WM */
