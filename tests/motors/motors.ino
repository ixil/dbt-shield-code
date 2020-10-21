#include <stepper.h>
#include <fast

#include <AccelStepper.h>
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);
st.setMaxSpeed(spmm*maxspeed);
st.setAcceleration(spmm*maxaccel);

shaft = true; // Direction of motor

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
  digitalWrite(STEP0_EN, HIGH);
  digitalWrite(STEP1_EN, HIGH);

  Serial.begin(250000);
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
}

}

void loop() {
  unsigned long currentMicros;
  currentMicros = micros();

  // Serial control
  /* // Print out Diagnostics */
  /* while(Serial.available() > 0) { */
  /*   int8_t read_byte = Serial.read(); */
  /*   if (read_byte == '0')      { digitalWrite( EN_PIN, HIGH ); } */
  /*   else if (read_byte == '1') { digitalWrite( EN_PIN,  LOW ); } */
  /*   else if (read_byte == '+') { if (OCR1A > MAX_SPEED) OCR1A -= 20; } */
  /*   else if (read_byte == '-') { if (OCR1A < MIN_SPEED) OCR1A += 20; } */
  /* } */


  delay(1000);
  dir = !dir;
  driver.shaft(dir);

  if((ms-last_time) > 100) { //run every 0.1s
    last_time = ms;

    DRV_STATUS_t drv_status[] = {0, 0};
    drv_status[0].sr = STEPPER0.DRV_STATUS();
    drv_status[1].sr = STEPPER1.DRV_STATUS();

    for (const int i_step=0; i_step <2; ++i_step; ) {
      Serial.print("%d ", i);
      Serial.print(drv_status[i_step].sg_result, DEC);
      Serial.print(" ");
      Serial.println(steppers[i_step].cs2rms(drv_status[i_step].cs_actual), DEC);
    }
  }

/* elapsedMicros = currentMicros - nextMicros;  */
/* if (elapsedMicros >= duration){ */
/* nextMicros = nextMicros + duration;

  } // end time check
} // end loop
}
