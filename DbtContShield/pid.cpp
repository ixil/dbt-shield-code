#include "pid.h"
#include "pins.h"

AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

void runPID(){
    motorsEnabled = motorsEnabled && myPID.atSetPoint(SETPOINT_THRESHOLD);
    if(heaterOn){
        myPID.run(); //call every loop, updates automatically at certain time interval
        digitalWrite(PWM6, outputVal);
        Serial.print("OUTPUTVAL:");
        Serial.println(outputVal);
    }
}

void initPID(){
    myPID.setBangBang(4);
    //set PID update interval to 4000ms
    myPID.setTimeStep(PID_TIMEINTERVAL);
}

