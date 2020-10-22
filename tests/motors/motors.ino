#include "stepper.h"


bool shaft = true; // Direction of motor
unsigned long last_micro = 0;
unsigned long last_time = 0;
unsigned long currentMillis;
unsigned long currentMicros;
uint32_t drv_status[] = {0, 0};

// Using direct register manipulation can reach faster stepping times
#define STEP_PORT     PORTF // Match with STEP_PIN
#define STEP_BIT_POS      0 // Match with STEP_PIN

/* ISR(TIMER1_COMPA_vect){ */
/*   //STEP_PORT ^= 1 << STEP_BIT_POS; */
/*   digitalWrite(STEP_PIN, !digitalRead(STEP_PIN)); */
/* } */

void setup() {
  // put your setup code here, to run once:
  setupStepperPins(); // Make stepper pins Outputs

  // Steppers are active low
  digitalWrite(STEPPER_0_EN, HIGH);
  digitalWrite(STEPPER_1_EN, HIGH);

  Serial.begin(115200);
  while(!Serial);
  Serial.println("\nStart...");

  SPI.begin();
  // tmc2130_init(STEPPER, current, steps_per_mm); 
  // 1500 ma
  tmc2130_init(STEPPER0, 800, 20.0/8.0);
  tmc2130_init(STEPPER1, 800, 20.0/8.0);

  // Set stepper interrupt
  /* { */
  /*   cli();//stop interrupts */
  /*   sei();//allow interrupts */
  /* } */
  digitalWrite(STEPPER_0_EN, LOW);
  digitalWrite(STEPPER_1_EN, LOW);
  last_time = millis();
  last_micro = micros();
  delay(2000);

  // Serial control
  // Print out Diagnostics
}

void loop() {
  currentMicros = micros(); currentMillis = millis();
  /* Serial.print("."); */


  if((currentMillis - last_time) > 5000) { //run every 0.1s
    /* drv_status[0] = STEPPER0.DRV_STATUS(); */
    /* drv_status[1] = STEPPER1.DRV_STATUS(); */
    Serial.println("\nlets reverse...");
    shaft = !shaft;
    STEPPER0.shaft(shaft);
    STEPPER1.shaft(shaft);
    digitalWrite(STEPPER_0_EN, HIGH);
    digitalWrite(STEPPER_1_EN, HIGH);
    delay(1000);
    digitalWrite(STEPPER_0_EN, LOW);
    digitalWrite(STEPPER_1_EN, LOW);
    delayMicroseconds(10);
    last_time = millis();
    /* if((currentMicros- last_time) > 100) { //run every 0.1s */
    /*   last_time = ms; */
    /*   for (const int i_step=0; i_step <2; ++i_step; ) { */
    /*     Serial.print("%d ", i); */
    /*     Serial.print(drv_status[i_step].sg_result, DEC); */
    /*     Serial.print(" "); */
    /*     Serial.println(steppers[i_step].cs2rms(drv_status[i_step].cs_actual), DEC); */
    /*   } */

    }


  /* elapsedMicros = currentMicros - nextMicros;  */
  if (micros() - currentMicros >= 10){
    digitalWrite(STEPPER_0_STP, HIGH);
    digitalWrite(STEPPER_1_STP, HIGH);
    delayMicroseconds(10);
    digitalWrite(STEPPER_0_STP, LOW);
    digitalWrite(STEPPER_1_STP, LOW);
  }
}

