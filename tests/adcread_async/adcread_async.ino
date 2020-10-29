#include "Interrupts.h"
#include "Thermistor.h"
#include "serial.h"
//#include <AutoPID.h>
#include <avr/pgmspace.h>
#include "pid.h"

ISR(ADC_vect){ (*InterruptGlobals::ADCInterrupt)(); }
/* ISR(ADC_vect){ ADCAveragingFilter::AveragingISRHandler(); }; */
Thermistor thermistor(0);
double temperature;


bool heaterOn = false;
double  setPoint, outputVal;


void setup () {

  Serial.begin(9600);
  Serial.println();
  pinMode(A1, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(A1, HIGH);
  cli();
  for (int i=0; i<9; ++i){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  initPID();
  setPoint = 70.0;

  thermistor.init();
  sei();
  delay(100);
}  // end of setup


void update(){
  thermistor.run();
}

void loop () {
  // do something with the reading, for example, print it
  TemperatureStatus tstat = thermistor.readTemperature(temperature);
  /*
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
    case TemperatureStatus::ShortCircuit:
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Short circuit!");
      break;
    default:
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Some other type of error!");
      break;
  };
  */
  

  runPID();
  //Serial.println(temperature);
  //erial.println("HEATER ON\n\n");
  //Serial.println(outputVal);
  //Serial.println("\n\n");
  
  processCom();
  Serial.flush();
  update();
}  // end of loop


