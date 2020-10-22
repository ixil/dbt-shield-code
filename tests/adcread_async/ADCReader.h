#ifndef ADCREADER_H
#define ADCREADER_H

class ADCReader
{
private:
    volatile int adcMeasurement;
    volatile bool adcReading;
    bool started;
    

public:
    ADCReader();
    virtual ~ADCReader();
    void run();
};

#endif /* ADCREADER_H */

