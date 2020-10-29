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
#define HEATER_PWM1_PIN 5


// #define MOSFET_0_PIN                          10
// #define MOSFET_1_PIN                          11
// #define MOSFET_2_PIN                          12
// #define MOSFET_3_PIN                          13
//
//
// #define IN_b0                                 25
// #define IN_b1                                 26
// #define IN_b2                                 27
// #define IN_b3                                 28
// #define IN_b4                                 29
// #define IN_b5                                 30
// #define IN_b6                                 31
// #define IN_b7                                 32
// #define IN_b8                                 33
// #define IN_b9                                 34
// #define IN_b10                                35
// #define IN_b11                                36
// #define IN_b12                                37
// #define IN_b13                                38
// #define IN_b14                                39
// #define IN_b15                                40
//
//
// #define OUT_b0                                44
// #define OUT_b1                                45
// #define OUT_b2                                42
// #define OUT_b3                                43
// #define OUT_b4                                49
// #define OUT_b5                                48
// #define OUT_b6                                46
// #define OUT_b7                                47
//
//
// #define MOT_STEP_PIN                          22
// #define MOT_DIR_PIN                           24
// #define MOT_ENBL_PIN                          23
//
//
// #define MISO                                  50
// #define SCK                                   52
//
//
// #define T1_CS                                 19
// #define T2_CS                                 18
//
// #define HEADER_D_0                            0
// #define HEADER_D_1                            1
// #define HEADER_D_2                            2
// #define HEADER_D_3                            3
// #define HEADER_D_4                            4
// #define HEADER_D_5                            5
// #define HEADER_A_0                            A0
// #define HEADER_A_1                            A1
// #define HEADER_A_2                            A2
// #define HEADER_A_3                            A3
// #define HEADER_A_4                            A4
// #define HEADER_A_5                            A5

#endif /* end of include guard: PINS_H_RSOALORP */
