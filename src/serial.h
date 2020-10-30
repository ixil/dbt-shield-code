#pragma once

#define BUF_LENGTH 128
static bool do_echo = true;
extern double temperature;
extern bool heaterOn;
extern double setPoint;

void exec(char);
void processCom();