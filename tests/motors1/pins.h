#ifndef PINS_H_RSOALORP
#define PINS_H_RSOALORP
#include <Arduino.h>


#define THERMISTOR                            A0
#define THERMISTOR_ON                         A1

#define OPTO_PORT PORTA
#define OPTO_INPUT_BITMASK 0x55
#define OPTO_I0 22
#define OPTO_I1 24
#define OPTO_I2 26
#define OPTO_I3 28
#define HEATER_DISABLE_PIN OPTO_I0
#define STEPPER_DISABLE_PIN OPTO_I3

#define PWM0 0
#define PWM1 1
#define PWM2 2
#define PWM3 3
#define PWM4 4
#define PWM5 5
#define PWM6 6
#define PWM7 7
#define PWM8 8
#define PWM9 9

#define HEATER_PWM0_PIN 5
#define HEATER_PWM1_PIN 6


#define STEPPER_0_CS      (11)
#define STEPPER_0_EN      (10)
#define STEPPER_0_STP     (25)
#define STEPPER_0_DIR     (23)

#define STEPPER_1_CS      (13)
#define STEPPER_1_EN      (12)
#define STEPPER_1_STP     (27)
#define STEPPER_1_DIR     (29)

#define EXTRUDER_CS  STEPPER_1_CS
#define EXTRUDER_EN  STEPPER_1_EN
#define EXTRUDER_STP STEPPER_1_STP
#define EXTRUDER_DIR STEPPER_1_DIR

void __attribute__((weak)) setupPinModes() {
    // Handled by TMC2130Stepper::begin()
    pinMode(STEPPER_0_CS, OUTPUT);
    pinMode(STEPPER_1_CS, OUTPUT);
    pinMode(STEPPER_0_EN, OUTPUT);
    pinMode(STEPPER_1_EN, OUTPUT);
    pinMode(STEPPER_0_STP, OUTPUT);
    pinMode(STEPPER_1_STP, OUTPUT);
    pinMode(STEPPER_0_DIR, OUTPUT);
    pinMode(STEPPER_1_DIR, OUTPUT);
    pinMode(PWM0, OUTPUT);
    pinMode(PWM1, OUTPUT);
    pinMode(PWM2, OUTPUT);
    pinMode(PWM3, OUTPUT);
    pinMode(PWM4, OUTPUT);
    pinMode(PWM5, OUTPUT);
    pinMode(PWM6, OUTPUT);
    pinMode(PWM7, OUTPUT);
    pinMode(PWM8, OUTPUT);
    pinMode(PWM9, OUTPUT);

    pinMode(THERMISTOR_ON, OUTPUT);
    pinMode(OPTO_I0, INPUT);
    pinMode(OPTO_I1, INPUT);
    pinMode(OPTO_I2, INPUT);
    pinMode(OPTO_I3, INPUT);
    pinMode(THERMISTOR, INPUT);
}

#endif /* end of include guard: PINS_H_RSOALORP */
