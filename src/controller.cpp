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
    ::extruderTargetSpeed = target;
}

void enableSteppers(){
    ::motorsEnabled = true;
    const uint8_t port = digitalPinToPort(STEPPER_0_EN);
    const uint8_t portBits = digitalPinToBitMask(STEPPER_0_EN) | digitalPinToBitMask(STEPPER_1_EN);
    volatile uint8_t *out = portOutputRegister(port);
    *out &= ~portBits; // motors are ACTIVE LOW
}

void disableSteppers(){
    ::motorsEnabled = true;
    const uint8_t port = digitalPinToPort(STEPPER_0_EN);
    const uint8_t portBits = digitalPinToBitMask(STEPPER_0_EN) | digitalPinToBitMask(STEPPER_1_EN);
    volatile uint8_t *out = portOutputRegister(port);
    *out |= portBits; // motors are ACTIVE LOW
}

void enableHeater(){
    ::heaterOn = true;
}

void disableHeater(){
    digitalWrite(PWM5, LOW);
    digitalWrite(PWM6, LOW);
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
    ::extruderTargetSpeed = 0.0;
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
        pinMode(STEPPER_DISABLE_PIN, INPUT);
        pinMode(HEATER_DISABLE_PIN, INPUT);
    }
}


/*! Disables both steppers, blocking interrupts, if the HEATER_DISABLE pin is high.*/
void pollStepperDisable(){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        const uint8_t bit = digitalPinToBitMask(STEPPER_DISABLE_PIN);
        const uint8_t port = digitalPinToPort(STEPPER_DISABLE_PIN);
        if ( *portInputRegister(port) & bit ) {
            disableSteppers();
        }
    }
}
/*! Disables the heater, blocking interrupts, if the HEATER_DISABLE pin is high.*/
void pollHeaterDisable(){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        const uint8_t bit = digitalPinToBitMask(HEATER_DISABLE_PIN);
        const uint8_t port = digitalPinToPort(HEATER_DISABLE_PIN);
        if ( *portInputRegister(port) & bit ) {
            disableHeater();
        }
    }
}

/*! Disables the heater, blocking interrupts, if the HEATER_DISABLE pin is high.*/
void pollDigitalInputs(){
    uint8_t bitCount, speedBits;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        const uint8_t inputs = (OPTO_PORT & OPTO_INPUT_BITMASK);
        bool quit = inputs & (digitalPinToBitMask(HEATER_DISABLE_PIN) |
                digitalPinToBitMask(STEPPER_DISABLE_PIN));
        if (quit) {
            if ( inputs & digitalPinToBitMask(HEATER_DISABLE_PIN) ) {
                disableHeater();
            }
            if ( inputs & digitalPinToBitMask(STEPPER_DISABLE_PIN) ) {
                disableExtruder();
            }
            return;
        }
    speedBits = inputs & (digitalPinToBitMask(OPTO_I1) | digitalPinToBitMask(OPTO_I2));
    } // else  // Exit Atomicblock here,
    for (bitCount=0; speedBits; ++bitCount){
        speedBits &= speedBits - 1;
    }
    switch (bitCount) {
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
        abort();
    }
}


void pollControlPins() {
  Serial.flush();
  pollDigitalInputs();
// #ifdef NDEBUG
  Serial.println("Inputs");
  Serial.println(OPTO_PORT & 0x55, BIN);
  Serial.print(" ExtruderTargetSpeed set to: ");
  Serial.print(::extruderTargetSpeed);
  Serial.println("mm/s");
  Serial.print(" Heaters : ");
  Serial.print(::heaterOn);
  Serial.print(" Motors: ");
  Serial.println(::motorsEnabled);
  Serial.println();
// #endif
}
}
