#ifndef SOILMOISTURESENSOR_H
#define SOILMOISTURESENSOR_H

#include <Arduino.h>

class SoilMoistureSensor {
public:
    SoilMoistureSensor(int pin, int airValue, int waterValue)
        : pin(pin), airValue(airValue), waterValue(waterValue) {
        intervals = (airValue - waterValue) / 3;
    }

    int read() {
        value = analogRead(pin);
        return value;
    }

    String getStatus() const {
        if (value > waterValue && value < (waterValue + intervals)) {
            return "VW"; // Very Wet
        } else if (value >= (waterValue + intervals) && value < (airValue - intervals)) {
            return "WET"; // Wet
        } else if (value >= (airValue - intervals) && value < airValue) {
            return "DRY"; // Dry
        } else {
            return "ERR"; // Error
        }
    }

private:
    int pin;
    int airValue;
    int waterValue;
    int intervals;
    int value = 0;
};

#endif // SOILMOISTURESENSOR_H
