#pragma once
#include <avr/pgmspace.h>

#define BUF_LENGTH 128
static bool do_echo = true;
extern double temperature;
extern bool heaterOn;
extern double setPoint;
extern volatile double extruderTargetSpeed;
extern bool speedUpdate;
extern bool motorsEnabled;

void exec(char);
void processCom();
