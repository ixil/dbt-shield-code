#include "Interrupts.h"
#include "Thermistor.h"
#include "math.h"

Thermistor::Thermistor(const ADCPin_t pinNo) : filter(pinNo, (1<<Globals::ADCSampleBits)-1){
  // filter = ADCAveragingFilter(pinNo, (1<<Globals::ADCSampleBits)-1);
  // filter{pinNo, (1<<Globals::ADCSampleBits)-1};
  const float lnR25 = logf(R25);
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

TemperatureStatus Thermistor::readTemperature(float& temperature){
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
      Serial.println("idle");
    case ADCStatus::Error:
      return TemperatureStatus::Error;
      break;
  }
  const ADCAveragingFilter::uint12_t Vref = 0x0FFF; // Max 12 bit number, 5V
  const float denom = (float)(Vref - Vin);
  Serial.println(denom);
  Serial.flush();

  if (denom <= 0.0) {
    temperature = ABS_ZERO;
    return TemperatureStatus::OpenCircuit;
  }
  const double resistance  =  Rseries * ((float)(Vin) / denom);
  const float logR = log(resistance);
  const float recipT = shhA + shhB * logR + shhC * logR * logR * logR;
  temperature = (recipT > 0.0) ? (1.0/recipT) + ABS_ZERO : ABS_ZERO;
  if (recipT < 0.0){
    return TemperatureStatus::Error;
  }
  return TemperatureStatus::Success;
}

    // TODO maybe this needs to be a look up table
  // double Thermistor::convertResistance(double resistance){
  //   const float logR = log(resistance);
  //   const float recipT = shhA + shhB * logR + shhC * logR * logR * logR;
  //   return  (recipT > 0.0) ? (1.0/recipT) + ABS_ZERO : BadErrorTemperature;
  // }

// Calculate shA and shB from the other parameters
// void Thermistor::calcDerivedParameters()
// {
// 	shhB = 1.0/beta;
// 	const float lnR25 = logf(r25);
// 	shA = 1.0/(25.0 - ABS_ZERO) - shB * lnR25 - shC * lnR25 * lnR25 * lnR25;
// }



// Calculate shA and shB from the other parameters
// void Thermistor::CalcDerivedParameters() {
// 	betaInv = 1.0/beta;
// 	const float lnR25 = logf(r25);
// 	shA = 1.0/(25.0 - ABS_ZERO) - shB * lnR25 - shC * lnR25 * lnR25 * lnR25;
// }
