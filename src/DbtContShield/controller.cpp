#include "controller.h"
#include <Arduino.h>
#include <util/atomic.h>
#include "pins.h"
#include "interrupts.h"
#include "TimerInterrupt.h"

/* ISR(ADC_vect){ ADCAveragingFilter::AveragingISRHandler(); }; */

namespace Controller {

/*! In mm^3/s */
double calculateFlowRate(float speed) {
    // TODO, do some fancy calculations
    return speed;
}

/*! In mm^3/s */
float calculateExtrudeSpeed(double flowrate) {
    // TODO, do some inverse fancy calculations
    return flowrate;
}

/*! In mm/s */
void setExtruderTargetSpeed(float target) {
  Serial.print("Setting target speed:");
  Serial.println(target);
  cli();
  ::extruderTargetSpeed = target;
  sei();
  ::speedUpdate = true;
}

void enableSteppers(){
    ::motorsEnabled = true;
    const uint8_t port = digitalPinToPort(STEPPER_0_EN);
    const uint8_t portBits = digitalPinToBitMask(STEPPER_0_EN) | digitalPinToBitMask(STEPPER_1_EN);
    volatile uint8_t *out = portOutputRegister(port);
    *out &= ~portBits; // motors are ACTIVE LOW
}

void disableSteppers(){
    ::motorsEnabled = false;
    const uint8_t port = digitalPinToPort(STEPPER_0_EN);
    const uint8_t portBits = digitalPinToBitMask(STEPPER_0_EN) | digitalPinToBitMask(STEPPER_1_EN);
    volatile uint8_t *out = portOutputRegister(port);
    *out |= portBits; // motors are ACTIVE LOW
    Serial.print("Steppers disabled");
}

void enableHeater(){
    ::heaterOn = true;
    digitalWrite(PWM6, HIGH);
}

void disableHeater(){
    digitalWrite(PWM5, LOW);
    //digitalWrite(PWM6, LOW);
    ::heaterOn = false;
}

void enableExtruder(){
    ::motorsEnabled = true;
    const uint8_t port = digitalPinToPort(EXTRUDER_EN);
    const uint8_t portBit = digitalPinToBitMask(EXTRUDER_EN);
    volatile uint8_t *out = portOutputRegister(port);
    *out |= portBit; // Extruders are ACTIVE LOW
}

void disableExtruder(){
    const uint8_t port = digitalPinToPort(EXTRUDER_EN);
    const uint8_t portBit = digitalPinToBitMask(EXTRUDER_EN);
    volatile uint8_t *out = portOutputRegister(port);
    *out |= portBit; // Extruders are ACTIVE LOW
    ::motorsEnabled = false;
    Serial.print("Disabling extruder");
    ::speedUpdate = true;
}

void disableAllPWMs() {
    for (int i=0; i < 9; ++i){
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
    ::motorsEnabled = false;
}


void setupKillPins(){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        pinMode(PWM5, OUTPUT);
        pinMode(PWM6, OUTPUT);
        disableExtruder();
        disableHeater();
        disableAllPWMs();
        pinMode(OPTO_I0, INPUT);
        pinMode(OPTO_I1, INPUT);
        pinMode(OPTO_I2, INPUT);
        pinMode(OPTO_I3, INPUT);
    }
}

uint8_t readOptoInputs(){
  return  0xF & ~(((PINJ & 0x03)<<2) | (PINH & 0x03));
}

bool readHeaterBit(const uint8_t& optobyte){
  //return optobyte & HEATER_DISABLE_BIT;

}

uint8_t readSpeedMode(const uint8_t& optobyte){
  return (optobyte) & SPEED_BITS_MASK;
}

bool readStepperBit(const uint8_t& optobyte){
  return optobyte & STEPPER_DISABLE_BIT;
}

/*! Disables both steppers, blocking interrupts, if the HEATER_DISABLE pin is high.*/
void pollStepperDisable(){
  const uint8_t inputs = readOptoInputs();
  if (!readStepperBit(inputs)){
    disableSteppers();
  }
}
/*! Disables the heater, blocking interrupts, if the HEATER_DISABLE pin is high.*/
void pollHeaterDisable(){
/*       const uint8_t inputs = readOptoInputs();
      if (!readHeaterBit(inputs)){
        //disableHeater();
    } */
}



/*! Disables the heater, blocking interrupts, if the HEATER_DISABLE pin is high.*/
void pollDigitalInputs(){
    uint8_t bitCount, speedMode;
    const uint8_t inputs = readOptoInputs();
      // Opto outputs are active low, so flip the bits
        //bool stepperdisable = readStepperBit(inputs);
        //bool heaterdisable = readHeaterBit(inputs);
        //if (heaterdisable) { disableHeater(); }
        //if (stepperdisable) { disableExtruder(); }
        //if (stepperdisable || heaterdisable) { return; }
    speedMode = readSpeedMode(inputs);
    switch (speedMode) {
        case 1:
            setExtruderTargetSpeed(5);
            break;
        case 2:
            setExtruderTargetSpeed(15);
            break;
        case 3:
            setExtruderTargetSpeed(30);
            break;
        default:
            break;
    }
}

void timerISR(void){
    pollDigitalInputs();
}

void setupControlTimers(){
    /* ITimer3.init(); */
    if (ITimer3.attachInterruptInterval(5, timerISR)){
    } else{
        Serial.println("Timer3 failed to setup!");
        Serial.flush();
        abort();
    }
}

void pollControlPins() {
  pollDigitalInputs();
// #ifdef NDEBUG
// #endif
}
}
