#include "pins.h"
#include "serial.h"
#include "steppers.h"
#include "pid.h"
#include "interrupts.h"

TMC2130Stepper *extruderDriver;
Stepper *extruderInstance;

void setup() {
  // put your setup code here, to run once:
  setupPinModes();

  Serial.begin(230400);
  while(!Serial);
  Serial.println("\nStart...");
  SPI.begin();

  extruderDriver = new TMC2130Stepper();
  extruderInstance = new Stepper(EXTRUDER_EN, EXTRUDER_STP, EXTRUDER_DIR, *extruderDriver, false);

  extruderInstance->enable();

  timersSetup();
}

void loop() {

  void exec(char);
  void processCom();

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
    changeStepper0Direction();
    /* changeStepper1Direction(); */
    last_time = millis();
  }

}

