#ifndef STEPPER_PINS_H
#define STEPPER_PINS_H
#include <Arduino.h>

// Sense Resistor value
#ifndef STEPPER_R_SENSE
#define STEPPER_R_SENSE (0.11f)
#endif
#define STEPPER_0_R_SENSE STEPPER_R_SENSE
#define STEPPER_1_R_SENSE STEPPER_R_SENSE

#endif /* STEPPER_PINS_H */
