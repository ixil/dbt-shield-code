#include "Thermistor.h"

Thermistor::Thermistor(const double VRef const double paramA, const double paramB, const double paramC);
Thermistor::Thermistor(const double VRef const double paramA, const double paramB) {

}

Thermistor::~Thermistor override{};
Thermistor::init() override;



  // if we aren't taking a reading, start a new one
  if (!adcStarted)
    {
    adcStarted = true;
    // start the conversion
    ADCSRA |= bit (ADSC) | bit (ADIE);
    }    

