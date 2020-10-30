#include "pins.h"
#include "serial.h"
#include "steppers.h"
#include "pid.h"
#include "interrupts.h"

TMC2130Stepper extruderDriver(STEPPER_0_CS, STEPPER_0_R_SENSE, 0);
Stepper extruderStepper(EXTRUDER_EN, EXTRUDER_STP, EXTRUDER_DIR, EXTRUDER, false);

// ADC0 is pin 0
Thermistor thermistor(0);

double temperature;
double setPoint, outputVal;
bool heaterOn;

ISR(ADC_vect){ (*InterruptGlobals::ADCInterrupt)(); }

void setup() {
  // put your setup code here, to run once:
  setupPinModes();
  extruderStepper.setupPins();

  Serial.begin(230400);
  while(!Serial);
  Serial.println("\nStart...");
  SPI.begin();
  extruderStepper.setup(); // after SPI


  extruderInstance->enable();

  therimstor.init();
  initPID();
  timersSetup();
  extruderStepper.setupStepperTimer5();
}

void errorCondition(){
  heaterOn = false;
  //TODO disable motor
  /* actually call the cmomands, don't wait for the loop */

}

void poll(){
  therimstor.run()
}

void loop() {

  thermistor.run();
  TemperatureStatus tstat = thermistor.readTemperature(temperature);
  switch (tstat) {
    case TemperatureStatus::NotReady:
      break;
    case TemperatureStatus::OpenCircuit:
      heaterOn = false;
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Open circuit!");
      errorCondition();
      break;
    case TemperatureStatus::ShortCircuit:
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Short circuit!");
      errorCondition();
      break;
  }
  runPID();

  processCom(); // Process the serial commands
  poll();
  Serial.flush();

  static uint16_t reverseTime = millis();
  static uint16_t speedChangeTime = millis();
  static uint16_t diagTime = millis();
  /* currentMicros = micros(); */
  currentMillis = millis();
  /* Serial.print("."); */

  if((currentMillis - speedChangeTime) > 500) {
    diagTime = millis();
    Serial.print("drv_err: "); Serial.println(STEPPER0.drv_err());
    Serial.print("uv_cp: "); Serial.println(STEPPER0.uv_cp());
    bool isEnabled();
    STEPPER0.push();
    Serial.print("is enabled: "); Serial.println(STEPPER0.isEnabled());
    Serial.print("DRV_STATUS: "); Serial.println(STEPPER0.DRV_STATUS(), HEX);
    Serial.print(targetPulse[0]);
  }

  if((currentMillis - speedChangeTime) > 500) {
    speedChangeTime = millis();
    Serial.print("\nspeeding up ...");
    Serial.print(targetPulse[0]);
    targetPulse[0] -= 100;
    speedChange[0]=true; speedChange[1]=true;
  }


  if((currentMillis - last_time) > 100000) {
    reverseTime = millis();
    Serial.println("\nlets reverse...");
    extruderStepper.changeDirection();
    /* changeStepper1Direction(); */
    last_time = millis();
  }

}

