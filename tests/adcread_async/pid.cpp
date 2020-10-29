#include "pid.h"

AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

void runPID(){
    if(heaterOn){
    myPID.run(); //call every loop, updates automatically at certain time interval
    digitalWrite(6, outputVal);
    digitalWrite(5, myPID.atSetPoint(1));
    }
}

void initPID(){
    myPID.setBangBang(4);
    //set PID update interval to 4000ms
    myPID.setTimeStep(4000);
}

