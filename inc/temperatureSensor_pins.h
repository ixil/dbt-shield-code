#ifndef TEMPERATURESENSOR_PINS_H
#define TEMPERATURESENSOR_PINS_H

#define TEMP_PLS A0
#define TEMP_SENS_PWR A1
#define TEMP_SENS_READ TEMP_PLS

class TemperatureSensor {

    public:
        void setup();
        static void initTemp();
        static void readTemp();
        static uint8_t getTemp();
        void setTargetTemp(float target);
        // void setTargetTemp(int16_t);

    private:
        float temperatureReading;
        float current_temperature = 0.0;
        int16_t current_temperature_raw =  0;
        int16_t target_temperature_raw =  0 ;
}

#endif /* TEMPERATURESENSOR_PINS_H */
