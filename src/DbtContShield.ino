#include "pins.h"
#include "controller.h"
#include "serialcommand.h"
#include "steppers.h"
#include "Thermistor.h"
#include "pid.h"
#include "interrupts.h"

TMC2130Stepper extruderDriver(EXTRUDER_CS, STEPPER_R_SENSE, 0);
Stepper extruderStepper(EXTRUDER_EN, EXTRUDER_STP, EXTRUDER_DIR, extruderDriver, false);
Stepper* extruderInstance = &extruderStepper;

// ADC0 is pin 0
Thermistor thermistor(0);

bool motorsEnabled;
double temperature;
double setPoint, outputVal;
bool heaterOn;
volatile double extruderTargetSpeed;
bool speedUpdate;
 

ISR(ADC_vect){ (*InterruptGlobals::ADCInterrupt)(); }

ISR(TIMER5_COMPA_vect){
  Stepper::StepperISR();
}

using namespace Controller;

void setup() {
  // put your setup code here, to run once:
  setupPinModes();
  setupKillPins();

  Serial.begin(230400);
  while(!Serial);
  Serial.println("\nStart...");
  SPI.begin();

  thermistor.init();
  extruderStepper.setup(); // after SPI
  initPID();

  setupControlTimers();
  extruderStepper.setupTimers();
  Controller::enableExtruder();
}

void errorCondition(){
  heaterOn = false;
  extruderStepper.setTargetStepperSpeed(0.0);
  // extruderStepper.disable();
  Serial.println("Error condition, Disabling");
}

void poll(){
  thermistor.run();
  pollControlPins();
  //FIXME only update on a change
  if (motorsEnabled){
    Serial.println("e enabled");
    extruderStepper.enable();
  }
  else{
    Serial.println("e disabled");
    extruderStepper.disable();
  }
  extruderStepper.setTargetStepperSpeed(extruderTargetSpeed);
  Serial.print("Target speed is currently: ");
  Serial.println(extruderTargetSpeed);
}

void loop() {
  thermistor.run();
  TemperatureStatus tstat = thermistor.readTemperature(temperature);
  switch (tstat) {
    case TemperatureStatus::NotReady:
      break;
    case TemperatureStatus::OpenCircuit:
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Open circuit!");
    case TemperatureStatus::ShortCircuit:
      Serial.print(temperature); Serial.println("C.");
      Serial.print(" Short circuit!");
    case TemperatureStatus::Error:
      // errorCondition(); //TODO uncomment once finished testing
      break;
    default:
      break;
  }
  runPID();

  processCom(); // Process the serial commands
  poll();
  Serial.flush();

  // static uint16_t reverseTime = millis();
  // static uint16_t speedChangeTime = millis();
  // static uint16_t diagTime = millis();
  // /* currentMicros = micros(); */
  // currentMillis = millis();
  // /* Serial.print("."); */

  // if((currentMillis - speedChangeTime) > 500) {
  //   diagTime = millis();
  //   Serial.print("drv_err: "); Serial.println(STEPPER0.drv_err());
  //   Serial.print("uv_cp: "); Serial.println(STEPPER0.uv_cp());
  //   bool isEnabled();
  //   STEPPER0.push();
  //   Serial.print("is enabled: "); Serial.println(STEPPER0.isEnabled());
  //   Serial.print("DRV_STATUS: "); Serial.println(STEPPER0.DRV_STATUS(), HEX);
  //   Serial.print(targetPulse[0]);
  // }

  // if((currentMillis - speedChangeTime) > 500) {
  //   speedChangeTime = millis();
  //   Serial.print("\nspeeding up ...");
  //   Serial.print(targetPulse[0]);
  //   targetPulse[0] -= 100;
  //   speedChange[0]=true; speedChange[1]=true;
  // }


  // if((currentMillis - last_time) > 100000) {
  //   reverseTime = millis();
  //   Serial.println("\nlets reverse...");
  //   extruderStepper.changeDirection();
  //   /* changeStepper1Direction(); */
  //   last_time = millis();
  // }

}

