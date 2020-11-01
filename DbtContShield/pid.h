#pragma once
#include <AutoPID.h>

#define SETPOINT_THRESHOLD 8

//pid settings and gains

#define KP .12
#define KI .0003
#define KD 0
#define PID_TIMEINTERVAL 1000
#define OUTPUT_MIN  0
#define OUTPUT_MAX  1

extern double temperature, setPoint, outputVal;
extern bool heaterOn;
extern bool motorsEnabled;



void initPID();
void runPID();

