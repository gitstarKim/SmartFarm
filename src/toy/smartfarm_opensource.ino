#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SHT3xSensor.h"
#include "SoilMoistureSensor.h"

// LCD 설정: 주소 0x27, 16x2 디스플레이
LiquidCrystal_I2C lcd(0x27, 16, 2);

// SHT3x 센서 객체 생성: 주소 0x44
SHT3xSensor sht3xSensor(0x44);

// 토양 수분 센서 객체 생성: 핀 A0, 공기 중 센서 값 520, 물 속 센서 값 260
SoilMoistureSensor soilSensor(A0, 520, 260);

// pH 센서 설정
#define SensorPin A2          // pH meter Analog output to Arduino Analog Input A2
#define Offset 0.00           // Deviation compensate
#define ArrayLength  40       // Times of collection
int pHArray[ArrayLength];     // Store the average value of the sensor feedback
int pHArrayIndex = 0;

void setup() {
    Serial.begin(9600);      // 시리얼 통신을 9600bps로 설정
    Wire.begin();            // I2C 통신을 시작합니다

    // LCD 초기화
    lcd.init();
    lcd.backlight();

    // SHT3x 센서 초기화
    while (!sht3xSensor.begin()) {
        Serial.println("SHT3x not found !");
        delay(1000);
    }
}

void loop() {
    static float pHValue, voltage;

    // 토양 수분 값 읽기
    soilSensor.read();
    String soilStatus = soilSensor.getStatus();

    // SHT3x 센서 값 읽기
    if (sht3xSensor.measure()) {
        float temperature = sht3xSensor.getTemperature();
        float humidity = sht3xSensor.getHumidity();
        float dewPoint = sht3xSensor.getDewPoint();

        // pH 센서 값 읽기
        for (int i = 0; i < ArrayLength; i++) {
            pHArray[i] = analogRead(SensorPin);
            delay(20); // 각 측정 간의 짧은 지연
        }
        voltage = averageArray(pHArray, ArrayLength) * 5.0 / 1024.0;
        pHValue = 3.5 * voltage + Offset;

        // 데이터 출력
        Serial.print("Temperature: ");
        Serial.print(temperature, 1);
        Serial.print(" *C\tHumidity: ");
        Serial.print(humidity, 1);
        Serial.print(" %RH\tDew Point: ");
        Serial.print(dewPoint, 1);
        Serial.print(" *C\tpH value: ");
        Serial.println(pHValue, 2);

        // LCD에 출력
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Soil:" + soilStatus);
        lcd.setCursor(0, 1);
        lcd.print("T");
        lcd.print(temperature, 1);
        lcd.print("H");
        lcd.print(humidity, 1);
        lcd.setCursor(10, 1);
        lcd.print("D");
        lcd.print(dewPoint, 1);
        lcd.setCursor(9, 0);
        lcd.print("PH");
        lcd.print(pHValue, 2);
        
        // Raspberry Pi로 전송할 데이터 생성
        String data = String(temperature, 1) + "," + String(humidity, 1) + "," + String(dewPoint, 1) + "," + soilStatus + "," + String(pHValue, 2);
        Serial.println(data);
    } else {
        Serial.println("SHT3x read error");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SHT3x Error");
    }

    delay(1000); // 1초마다 측정 반복
}

// 배열의 평균 계산 함수
double averageArray(int* arr, int number) {
    int i;
    int max, min;
    double avg;
    long amount = 0;

    if (number <= 0) {
        Serial.println("Error number for the array to averaging!/n");
        return 0;
    }

    if (number < 5) { // 배열 길이가 5 미만인 경우 직접 평균 계산
        for (i = 0; i < number; i++) {
            amount += arr[i];
        }
        avg = amount / number;
        return avg;
    } else {
        // 배열 길이가 5 이상인 경우 최댓값과 최솟값을 제외한 평균 계산
        if (arr[0] < arr[1]) {
            min = arr[0]; max = arr[1];
        } else {
            min = arr[1]; max = arr[0];
        }

        for (i = 2; i < number; i++) {
            if (arr[i] < min) {
                amount += min;
                min = arr[i];
            } else if (arr[i] > max) {
                amount += max;
                max = arr[i];
            } else {
                amount += arr[i];
            }
        }
        avg = (double)amount / (number - 2);
    }
    return avg;
}
