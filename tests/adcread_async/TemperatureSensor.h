#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

class TemperatureSensor {
protected:
    float lastValue;
    bool busy;
    bool tempRead;
    bool newValue;

public:
    TemperatureSensor();
    virtual ~TemperatureSensor();

    virtual void init() = 0;
    virtual bool isBusy() = 0;
    virtual float readLastValue() = 0;
    virtual void startMeasurement() = 0;
    virtual bool newMeasurement() = 0;
};

#endif /* TEMPERATURESENSOR_H */
