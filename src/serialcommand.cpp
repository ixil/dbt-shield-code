#include <Arduino.h>
#include "serialcommand.h"
#include "controller.h"

#ifndef MAX_EVEL
#define MAX_EVEL 99999
#endif
void exec(char *cmdline)
{
    char *command = strsep(&cmdline, " ");

    if (strcmp_P(command, PSTR("help")) == 0) {
        Serial.println(F(
            "mode <pin> <mode>: pinMode()\r\n"
            "read <pin>: digitalRead()\r\n"
            "aread <pin>: analogRead()\r\n"
            "write <pin> <value>: digitalWrite()\r\n"
            "awrite <pin> <value>: analogWrite()\r\n"
            "echo <value>: set echo off (0) or on (1)\r\n\n"
            "heater <number> <value>: 0/1 on/off   /   0-250C\r\n"
            "extrude <number> <value>: 0/1 on/off  /  -3000-3000\r\n"
            "evel <number> <value>: 0/1 on/off    /   -300-300\r\n"
            "load: load filament\r\n"
            "unload: unload filament\r\n"
            "temp: extruder temperature\r\n"
            "stat: \r\n"
            ));
    } else if (strcmp_P(command, PSTR("mode")) == 0) {
        int pin = atoi(strsep(&cmdline, " "));
        int mode = atoi(cmdline);
        pinMode(pin, mode);
    } else if (strcmp_P(command, PSTR("read")) == 0) {
        int pin = atoi(cmdline);
        Serial.println(digitalRead(pin));
    } else if (strcmp_P(command, PSTR("aread")) == 0) {
        int pin = atoi(cmdline);
        Serial.println(analogRead(pin));
    } else if (strcmp_P(command, PSTR("write")) == 0) {
        int pin = atoi(strsep(&cmdline, " "));
        int value = atoi(cmdline);
        digitalWrite(pin, value);
    } else if (strcmp_P(command, PSTR("awrite")) == 0) {
        int pin = atoi(strsep(&cmdline, " "));
        int value = atoi(cmdline);
        analogWrite(pin, value);
    } else if (strcmp_P(command, PSTR("echo")) == 0) {
        do_echo = atoi(cmdline);
    } else if (strcmp_P(command, PSTR("heater")) == 0){
        int number = atoi(strsep(&cmdline, " "));
        int value = atoi(cmdline);
        setPoint = value;
        heaterOn = !(bool)number;
    } else if (strcmp_P(command, PSTR("evel")) == 0){
        int number = atoi(strsep(&cmdline, " "));
        float value = atof(cmdline);
        // TODO guard on the values that won't fit inside a uint16_t
        extruderTargetSpeed = value;
        speedUpdate = true;
        if (number) {Controller::enableSteppers();}
    } else if(strcmp_P(command, PSTR("load")) == 0){
        //TODO load filament
    } else if(strcmp_P(command, PSTR("unload")) == 0){
        //TODO unload filamnent
    } else if (strcmp_P(command, PSTR("temp")) == 0){
        Serial.println(temperature);
    } else if (strcmp_P(command, PSTR("stat")) == 0){
        ::statusCheck=true;
    } else {
        Serial.print(F("Error: Unknown command: "));
        Serial.println(command);
    }
}

void processCom()
{
    /* Process incoming commands. */
    while (Serial.available()) {
        static char buffer[BUF_LENGTH];
        static int length = 0;

        int data = Serial.read();
        if (data == '\b' || data == '\177') {  // BS and DEL
            if (length) {
                length--;
                if (do_echo) Serial.write("\b \b");
            }
        }
        else if (data == '\r') {
            if (do_echo) Serial.write("\r\n");    // output CRLF
            buffer[length] = '\0';
            if (length) exec(buffer);
            length = 0;
        }
        else if (length < BUF_LENGTH - 1) {
            buffer[length++] = data;
            if (do_echo) Serial.write(data);
        }
    }
}
