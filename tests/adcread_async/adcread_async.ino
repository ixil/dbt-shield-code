#include "Interrupts.h"
#include "Thermistor.h"

ISR(ADC_vect){ (*InterruptGlobals::ADCInterrupt)(); }
/* ISR(ADC_vect){ ADCAveragingFilter::AveragingISRHandler(); }; */
Thermistor thermistor(0);

void setup () {

  Serial.begin(230400);
  Serial.println();
  cli();
  pinMode(A1, OUTPUT);
  for (int i=0; i<9; ++i){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  digitalWrite(A1, HIGH);
  thermistor.init();
  sei();
}  // end of setup


void update(){
  thermistor.run();
}

void loop () {
  // do something with the reading, for example, print it
  double temperature;
  TemperatureStatus tstat = thermistor.readTemperature(temperature);
  switch (tstat) {
    case TemperatureStatus::Success:
      Serial.print(temperature); Serial.println("C.");
      break;
    case TemperatureStatus::NotReady:
      break;
    case TemperatureStatus::OpenCircuit:
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Open circuit!");
      break;
    default:
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Some type of error!");
      break;
  };
  Serial.flush();
  update();
}  // end of loop


