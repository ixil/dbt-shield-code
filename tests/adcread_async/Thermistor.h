#ifndef THERMISTOR_H
#define THERMISTOR_H

#include "Arduino.h"
// #include "TemperatureSensor.h"
#include "ADCAveragingFilter.h"

namespace Globals {
    constexpr auto ThermistorOverSampleBits = 1;
    constexpr auto ThermistorFilterLength = 4<<2*ThermistorOverSampleBits;
    constexpr auto ThermistorExpectedLowerBound = 10.0;
    constexpr auto ThermistorExpectedUpperBound = 320.0;
    constexpr auto ADCSampleBits = 10;
}

#define ABS_ZERO (-273.15)

enum class TemperatureStatus{
    Success,
    NotReady,
    OpenCircuit,
    Error
};

//Uses Steinhardt-Hardt equation
class Thermistor {
    private:
        using ADCPin_t = ADCAveragingFilter::ADCPin_t;
        ADCAveragingFilter filter;

        const float Rseries = 1e5;

        // Semitech 104NT-4-R025H42G (recommend B4267)
        // Beta and C taken from https://wiki.e3d-online.com/E3D-v6_Assembly#Configure_Firmware_.28Easy.21.29
        // const uint16_t ParamBeta = 4725;
        const float R25 = 1e5;
        //Steinhart-Hardt parameters
        const float shhC = 7.068e-8;
        const float shhB = 1.0/4725;
        float shhA; //Derived

    public:
        Thermistor(const ADCPin_t pinNo);
        virtual ~Thermistor();

        void init();
        void run();
        TemperatureStatus readTemperature(float& temperature);
};

#endif /* THERMISTOR_H */

