#ifndef SHT3XSENSOR_H
#define SHT3XSENSOR_H

#include <ArtronShop_SHT3x.h>
#include <Wire.h>

class SHT3xSensor {
public:
    SHT3xSensor(uint8_t address) : sensor(address, &Wire) {}

    bool begin() {
        return sensor.begin();
    }

    bool measure() {
        if (sensor.measure()) {
            temperature = sensor.temperature();
            humidity = sensor.humidity();
            dewPoint = calculateDewPoint(temperature, humidity);
            return true;
        }
        return false;
    }

    float getTemperature() const {
        return temperature;
    }

    float getHumidity() const {
        return humidity;
    }

    float getDewPoint() const {
        return dewPoint;
    }

private:
    ArtronShop_SHT3x sensor;
    float temperature = 0;
    float humidity = 0;
    float dewPoint = 0;

    float calculateDewPoint(float temp, float hum) {
        const float a = 17.27;
        const float b = 237.7;
        float alpha = ((a * temp) / (b + temp)) + log(hum / 100.0);
        return (b * alpha) / (a - alpha);
    }
};

#endif // SHT3XSENSOR_H
