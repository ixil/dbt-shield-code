#include <Arduino.h>
#include "TMCStepper.h"
#include "pins.h"
#include "steppers.h"

TMC2130Stepper driver(STEPPER_1_CS, 0.11);
Stepper stepper(STEPPER_1_EN, STEPPER_1_STP, STEPPER_1_DIR, driver);
void setup()
{
  setupPinModes();
  Serial.begin(230400);
  stepper.setup();
  Serial.print("Testing connection: ");
  Serial.println(driver.test_connection());
  stepper.enable();
  digitalWrite(STEPPER_0_CS, HIGH);
}


void loop()
{

  /* stepper.enable(); */
  digitalWrite(STEPPER_0_CS, HIGH);
  digitalWrite(STEPPER_0_STP, HIGH);
  delayMicroseconds(10);
  digitalWrite(STEPPER_0_STP, LOW);
  /* Serial.print("Status: "); */
  /* Serial.println(stepper.getDrvStatus(), BIN); */
  /* Serial.print("enabled: "); */
  /* Serial.print(driver.isEnabled()); */
  /* Serial.print(" Stalled "); */
  /* Serial.println(stepper.stallStatus()); */

  
}


