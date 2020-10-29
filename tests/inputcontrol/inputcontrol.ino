#include <Arduino.h>
#include <util/atomic.h>
#include "pins.h"
#include "Interrupts.h"
#include "stepper_pins.h"

#include "TimerInterrupt.h"
/* ISR(ADC_vect){ ADCAveragingFilter::AveragingISRHandler(); }; */

volatile float extruderTargetSpeed = 0.0; // mm/s
double extruderTargetFlowRate = 0.0; // mm^3/s

volatile bool heatersEnabled = false;
volatile bool motorsEnabled = false;

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
    extruderTargetSpeed = target;
}

void writeDigitalPinHigh(uint8_t pin){
    const uint8_t portBit = digitalPinToBitMask(pin);
    const uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *out = portOutputRegister(port);
    *out |= portBit;
}

void enableSteppers(){
    motorsEnabled = true;
    const uint8_t port = digitalPinToPort(STEPPER_0_EN);
    const uint8_t portBits = digitalPinToBitMask(STEPPER_0_EN) | digitalPinToBitMask(STEPPER_1_EN);
    volatile uint8_t *out = portOutputRegister(port);
    *out &= ~portBits; // motors are ACTIVE LOW
}

void disableSteppers(){
    motorsEnabled = true;
    const uint8_t port = digitalPinToPort(STEPPER_0_EN);
    const uint8_t portBits = digitalPinToBitMask(STEPPER_0_EN) | digitalPinToBitMask(STEPPER_1_EN);
    volatile uint8_t *out = portOutputRegister(port);
    *out |= portBits; // motors are ACTIVE LOW
}

void enableHeater(){
    heatersEnabled = true;
}

void disableHeater(){
    digitalWrite(PWM5, LOW);
    digitalWrite(PWM6, LOW);
    heatersEnabled = false;
}


void enableExtruder(){
    motorsEnabled = true;
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
    motorsEnabled = false;
    extruderTargetSpeed = 0.0;
}

void disableAllPWMs() {
    for (int i=0; i < 9; ++i){
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
    motorsEnabled = false;
}


void setup_killpins(){
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

void setupTimerInterrupts(){
    /* ITimer3.init(); */
    if (ITimer3.attachInterruptInterval(5, timerISR)){
        Serial.println("Timer3 interrupt setup");
    } else{
        Serial.println("Timer3 failed to setup!");
        abort();
    }
}

void updateTimers(){
}

void setupPins(){

    DDRA &= ~(OPTO_INPUT_BITMASK);
    /* pinMode(OPTO_I0, INPUT); */
    /* pinMode(OPTO_I1, INPUT); */
    /* pinMode(OPTO_I2, INPUT); */
    /* pinMode(OPTO_I3, INPUT); */

}

void setup () {
    setup_killpins();
    setupPins();
    /* setup_powerhold(); */
    /* disableStepperDrivers(); */
    enableExtruder();
    Serial.begin(230400);
    Serial.println();
    setupTimerInterrupts();
}  // end of setup


void loop () {
  Serial.flush();
  pollDigitalInputs();
  Serial.println("Inputs");
  Serial.println(OPTO_PORT & 0x55, BIN);
  Serial.print(" ExtruderTargetSpeed set to: ");
  Serial.print(extruderTargetSpeed);
  Serial.println("mm/s");
  Serial.print(" Heaters : ");
  Serial.print(heatersEnabled);
  Serial.print(" Motors: ");
  Serial.println(motorsEnabled);
  Serial.println();

}  // end of loop


/* 
 void setup() {
     thermalManager.init();    // Initialize temperature loop
     job_timer.init();   // Initial setup of print job timer
     steppers.init();           // Init stepper. This enables interrupts!
     lcd_init();
     lcd_reset_status();
 
 #if ENABLED(SHOW_BOOTSCREEN)
     lcd_bootscreen();
 #endif
 
 #if ENABLED(USE_WATCHDOG)
     watchdog_init();
 #endif
 }
 */
