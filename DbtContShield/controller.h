#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <Arduino.h>

extern bool motorsEnabled;
extern double temperature;
extern double setPoint, outputVal;
extern bool heaterOn;
extern double extruderTargetSpeed;
extern volatile bool speedUpdate;

namespace Controller {

    float calculateExtrudeSpeed(double flowrate);
    double calculateFlowRate(float speed);

    void disableAllPWMs();
    void disableExtruder();
    void disableHeater();
    void disableSteppers();

    void enableExtruder();
    void enableHeater();
    void enableSteppers();

    void pollControlPins();
    void pollDigitalInputs();
    void pollHeaterDisable();
    void pollStepperDisable();

    void setExtruderTargetSpeed(float target);
    void setupControlTimers();

    void setupKillPins();
    void printControls();
    uint8_t readOptoInputs();

    void timerISR(void);
}

#endif /* CONTROLLER_H */
