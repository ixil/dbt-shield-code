#include "stepper.h"
#include "interrupts.h"

void setup() {
  // put your setup code here, to run once:
  setupStepperPins(); // Make stepper pins Outputs

  // Steppers are active low
  digitalWrite(STEPPER_0_EN, HIGH);
  digitalWrite(STEPPER_1_EN, HIGH);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);

  Serial.begin(230400);
  while(!Serial);
  Serial.println("\nStart...");

  SPI.begin();
  // tmc2130_init(STEPPER, current, steps_per_mm); 
  // 1500 ma
  tmc2130_init(STEPPER0, 80, 300.0);
  tmc2130_init(STEPPER1, 80, 300.0);

  digitalWrite(STEPPER_0_EN, HIGH);
  digitalWrite(STEPPER_1_EN, HIGH);

  timerSetup();
}

void loop() {
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

