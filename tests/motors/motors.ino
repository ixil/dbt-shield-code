#include <Arduino.h>
#include "stepper.h"

unsigned long last_micro = 0;
unsigned long last_time = 0;
unsigned long currentMillis;
unsigned long currentMicros;
uint32_t drv_status[] = {0, 0};

// Using direct register manipulation can reach faster stepping times
#define STEP_PORT PORTA // Match with STEP_PIN
#define STEPPER0_BIT_POS      0 // Match with STEP_PIN
#define STEPPER1_BIT_POS      3 // Match with STEP_PIN

uint16_t stepperSpeedPulses;
volatile bool speedChange[2] = {false, false};
uint16_t targetPulse[2] = {10000,10000};
bool stepperDirections[2] = {false,false};

void changeStepper0Direction(){
  TIMSK5 &= ~bit(OCIE5A) & ~bit(TOIE5); // disable interrupt
  TIMSK5 |= bit(OCF5A); // clear interrupt flag
  STEPPER0.switchCSpin(LOW);
  STEPPER0.shaft(stepperDirections[0]); //TODO may have to wait for this to have happened?
  STEPPER0.switchCSpin(HIGH);
  TIFR5 |= bit(OCF5A) | bit(TOV5); // clear flags
  TIMSK5 |= bit(OCIE5A) | bit(TOIE5); // reenable interrupt
}

void changeStepper1Direction(){
  TIMSK4 &= ~bit(OCIE4B) & ~bit(TOIE4); // disable interrupt
  TIMSK4 |= bit(OCF4B); // disable interrupt
  STEPPER1.shaft(stepperDirections[1]); //TODO may have to wait for this to have happened?
  TIFR4 |= bit(OCF4B) | bit(TOV4); // clear flags
  TIMSK4 |= bit(OCIE4B) | bit(TOIE4); // reenable interrupt
}


#define MOTORSTEPS 200
/* https://e3d-online.dozuki.com/Guide/Extruder+steps+per+mm/96 */
#define GEAR_RATIO 3
#define HOB_DIAMETER 7.3
#define STEP_PRESCALER 256
#define TIMER_FREQ 16000000UL/STEP_PRESCALER*2
#define MAX_STEPPERIOD 16000000UL/STEP_PRESCALER*2
constexpr double STEPS_PER_MM = MOTORSTEPS*MICROSTEPPING*GEAR_RATIO / (HOB_DIAMETER * PI);

void setTargetStepperSpeed(uint8_t stepper, double speed){
  stepper = stepper % 2;
  targetPulse[stepper] = uint16_t(TIMER_FREQ / (speed * STEPS_PER_MM) - 0.5); // round, and do 0 count offset

}

void updateStepper0Speed(){
  // order matters, High byte write first
  OCR5AH = targetPulse[0] >> 8;
  OCR5AL = targetPulse[0] & 0xFF;
}

void updateStepper1Speed(){
  // order matters, High byte write first
  OCR4BH = targetPulse[1] >> 8;
  OCR4BL = targetPulse[1] & 0xFF;
}

/* 
 ISR(TIMER4_COMPA_vect){
   STEP_PORT ^= 1 << STEPPER1_BIT_POS; //Toggle the bit, using dedge
   if (speedChange[1]){
     updateStepper1Speed();
   }
 }
 */

ISR(TIMER5_COMPA_vect){
  STEP_PORT ^=  1 << STEPPER0_BIT_POS; //Toggle the bit, using dedge
  if (speedChange[0]){
    updateStepper0Speed();
  }
}

void setupStepper0Timer(){
  TCCR5A = bit(COM5A1);
  TCCR5B = bit(WGM52) | bit(CS42); //| bit(CS40); // CS42, CS41, CS40; // Prescaler 0, 1, 8, 64, 256, 1024, external
  // Force output compare, (for immediate compare match)
  TCCR5C =  bit(FOC5A) | bit(FOC5B) | bit(FOC5C); // CS42, CS41, CS40; // 0, 1, 8, 64, 256, 1024, external
  // interrupt mask (input / output compare channels and timer overflow)
  TIMSK5 = bit(OCIE5A) ;
}

void setupStepper1Timer(){
  // TODO This will clash with the PWM pin
  TCCR4A = bit(COM4A1);
  TCCR4B = bit(WGM52) | bit(CS42); //| bit(CS40); // CS42, CS41, CS40; // Prescaler 0, 1, 8, 64, 256, 1024, external
  // Force output compare, (for immediate compare match)
  TCCR4C =  bit(FOC4A) | bit(FOC4B) | bit(FOC4C);
  // interrupt mask (input / output compare channels and timer overflow)
  TIMSK4 = bit(OCIE4B) ;
}


void timerSetup(){
  setupStepper0Timer();
  /* setupStepper1Timer(): */
}


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
  STEPPER0.switchCSpin(LOW);
  tmc2130_init(STEPPER0, 80, 300.0);
  STEPPER0.switchCSpin(HIGH);
  STEPPER1.switchCSpin(LOW);
  tmc2130_init(STEPPER1, 80, 300.0);
  STEPPER1.switchCSpin(HIGH);

  digitalWrite(STEPPER_0_EN, HIGH);
  digitalWrite(STEPPER_1_EN, HIGH);

  timerSetup();
}

/* 
 template<class TMC, char AXIS_LETTER, char DRIVER_ID, AxisEnum AXIS_ID>
     bool test_stall_status() {
       this->switchCSpin(LOW);
       // read stallGuard flag from TMC library, will handle HW and SW SPI
       TMC2130_n::DRV_STATUS_t drv_status{0};
       drv_status.sr = this->DRV_STATUS();
 
       this->switchCSpin(HIGH);
       return drv_status.stallGuard;
     }
 */

void loop() {
  static uint16_t reverseTime = millis();
  static uint16_t speedChangeTime = millis();
  static uint16_t diagTime = millis();
  /* currentMicros = micros(); */
  currentMillis = millis();
  /* Serial.print("."); */

  if((currentMillis - speedChangeTime) > 500) {
    diagTime = millis();
    STEPPER0.switchCSpin(LOW);
    Serial.print("drv_err: "); Serial.println(STEPPER0.drv_err());
    Serial.print("uv_cp: "); Serial.println(STEPPER0.uv_cp());
    bool isEnabled();
    STEPPER0.push();
    Serial.print("is enabled: "); Serial.println(STEPPER0.isEnabled());
    Serial.print("DRV_STATUS: "); Serial.println(STEPPER0.DRV_STATUS(), HEX);
    STEPPER0.switchCSpin(HIGH);
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

