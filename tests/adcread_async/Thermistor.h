#ifndef THERMISTOR_H
#define THERMISTOR_H

#include "TemperatureSensor.h"

class Thermistor : public TemperatureSensor, public ADCReader {
private:
    double paramA;
    double paramB;
    double paramC;

public:
    Thermistor(const double nomVRef, const double paramA, const double paramB, const double paramC);
    Thermistor(const double nomVRef, const double paramA, const double paramB);
    virtual ~Thermistor;
    void init() override;
};

#endif /* THERMISTOR_H */

