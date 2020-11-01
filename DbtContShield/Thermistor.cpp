#include "Thermistor.h"
#include "interrupts.h"
#include "math.h"

Thermistor::Thermistor(const ADCPin_t pinNo) : filter(pinNo, (1<<Globals::ADCSampleBits)-1){
  const double lnR25 = log(R25);
  shhA = 1.0/(25.0 - ABS_ZERO) - shhB * lnR25 - shhC * lnR25 * lnR25 * lnR25;
}
Thermistor::~Thermistor() = default;

void Thermistor::init(){
  filter.init();
  filter.setSingleShotMode();
}

void Thermistor::run(){
  filter.run();
}

TemperatureStatus Thermistor::readTemperature(double& temperature){
  ADCAveragingFilter::uint12_t Vin=0;

  const ADCStatus status = filter.getFilteredValue(Vin);
  switch (status) {
    case ADCStatus::Success:
      break;
    case ADCStatus::Busy:
      return TemperatureStatus::NotReady;
      break;
    case ADCStatus::Active:
      return TemperatureStatus::NotReady;
      break;
    case ADCStatus::Idle:
      Serial.println("Thermistor idle");
    case ADCStatus::Error:
      return TemperatureStatus::Error;
      break;
  }
  // Max Vin could be
  const int16_t Vref = ((1 << Globals::ADCSampleBits) - 1) <<  ADCAveragingFilter::ThermistorOverSampleBits;
 /*   Serial.print(" Vref: ");
   Serial.print(Vref);
   Serial.print("v, Vin: ");
   Serial.println((int16_t)Vin);
   Serial.println("v"); */
  const double denom = (double)(Vref - (int16_t)Vin);
  if (Vin == 0){
    // Serial.println(" Vin is 0.0 ");
    return TemperatureStatus::ShortCircuit;

  }
  if (denom <= 0.0) {
    temperature = ErrorBadTemp;
    return TemperatureStatus::OpenCircuit;
  }
  const double resistance  =  Rseries * ((double)(Vin) / denom);
  //Serial.print(" resistance: ");
  //Serial.println(resistance);
  //Serial.flush();
  const double logR = log(resistance);
  const double recipT = shhA + shhB * logR + shhC * logR * logR * logR;
  temperature = (recipT > 0.0) ? (1.0/recipT) + ABS_ZERO : ErrorBadTemp;
  if (recipT < 0.0){
    return TemperatureStatus::Error;
  }
  return TemperatureStatus::Success;
}

    // TODO maybe this needs to be a look up table
  // double Thermistor::convertResistance(double resistance){
  //   const double logR = log(resistance);
  //   const double recipT = shhA + shhB * logR + shhC * logR * logR * logR;
  //   return  (recipT > 0.0) ? (1.0/recipT) + ABS_ZERO : BadErrorTemperature;
  // }

// Calculate shA and shB from the other parameters
// void Thermistor::calcDerivedParameters()
// {
// 	shhB = 1.0/beta;
// 	const double lnR25 = logf(r25);
// 	shA = 1.0/(25.0 - ABS_ZERO) - shB * lnR25 - shC * lnR25 * lnR25 * lnR25;
// }



// Calculate shA and shB from the other parameters
// void Thermistor::CalcDerivedParameters() {
// 	betaInv = 1.0/beta;
// 	const double lnR25 = logf(r25);
// 	shA = 1.0/(25.0 - ABS_ZERO) - shB * lnR25 - shC * lnR25 * lnR25 * lnR25;
// }
