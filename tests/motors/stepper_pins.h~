#ifndef STEPPER_PINS_H
#define STEPPER_PINS_H
#include <Arduino.h>

// Stepper enable and Chip select pins
#define STEPPER_0_CS      (D13)
#define STEPPER_0_EN      (D12)
#define STEPPER_0_STP     (D23)
#define STEPPER_1_CS      (D13)
#define STEPPER_1_EN      (D10)
#define STEPPER_1_STP     (D25)

// Sense Resistor value
#ifndef STEPPER_R_SENSE
#define STEPPER_R_SENSE (0.11f)
#endif
#define STEPPER_0_R_SENSE STEPPER_R_SENSE
#define STEPPER_1_R_SENSE STEPPER_R_SENSE

    void setupStepperPins() {
        // Handled by TMC2130Stepper::begin()
        // pinMode(STEPPER_0_CS, OUTPUT);
        // pinMode(STEPPER_1_CS, OUTPUT);
        pinMode(STEPPER_0_EN, OUTPUT);
        pinMode(STEPPER_0_STP, OUTPUT);
        pinMode(STEPPER_1_EN, OUTPUT);
        pinMode(STEPPER_1_STP, OUTPUT);
    }

#endif /* STEPPER_PINS_H */
