#include "Interrupts.h"
#include "Thermistor.h"


volatile bool WDTCRAZY=false;
volatile bool blink = false;
/* ISR(ADC_vect){ ADCAveragingFilter::ADCAveragingFilter(); }; */
ISR(ADC_vect){ InterruptGlobals::ADCInterrupt(); };
ISR(__vector_default){blink=true; digitalWrite(LED_BUILTIN, HIGH);};
ISR(WDT_vect){WDTCRAZY=true;}
Thermistor thermistor(1);

void setup () {

  Serial.begin(230400);
  Serial.println();
  cli();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(A1, INPUT);
  pinMode(A0, OUTPUT);
  for (int i=0; i<9; ++i){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  digitalWrite(A0, HIGH);
  Serial.print("ADC int address before: ");
  Serial.print((uint16_t)InterruptGlobals::ADCInterrupt);
  blink = blink ? false : false;
  Serial.flush();
  thermistor.init();
  Serial.print("ADC int address now: ");
  Serial.print((uint16_t)InterruptGlobals::ADCInterrupt);
  Serial.flush();

  if (WDTCRAZY){
    Serial.println("WATCHDOG INTERRUPTED");
    Serial.flush();
    while(true);
  }
  sei();
  Serial.println("reenabling interrupts");
  Serial.flush();
}  // end of setup


void update(){
  thermistor.run();
}

void loop () {
  // do something with the reading, for example, print it
  Serial.print("ADC int address is: ");
  Serial.print((uint16_t)InterruptGlobals::ADCInterrupt);
  blink = blink ? false : false;
  float temperature;
  TemperatureStatus tstat = thermistor.readTemperature(temperature);
  switch (tstat) {
    case TemperatureStatus::Success:
      Serial.print("X");
      Serial.print(temperature); Serial.print("C\n");
      break;
    case TemperatureStatus::NotReady:
      Serial.print("Not ready yet.");
      break;
    case TemperatureStatus::OpenCircuit:
      Serial.print("Open circuit!");
      break;
    default:
      Serial.print("S");
      break;
  };
  Serial.println();
  Serial.flush();
  /* update(); */
}  // end of loop


