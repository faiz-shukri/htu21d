#ifndef _htu21d_h
#define _htu21d_h

#include <Arduino.h>
#include <Wire.h>
#include "elapsedMillis.h"
#define HTU21D_ADDRESS 0x40
class HTU21D;
typedef void (*HTU21DCallback)(HTU21D *);

class HTU21D
{
private:
    uint8_t step;
    elapsedMillis e;
    elapsedMillis timer;
    uint8_t requestHumidity();
    uint8_t readHumidity();
    uint8_t requestTemperature();
    uint8_t readTemperature();
    int readResult();

public:
    TwoWire *wire;
    uint8_t address;
    uint32_t pollingRate;
    HTU21D(TwoWire *wire, uint8_t address = HTU21D_ADDRESS);
    HTU21DCallback onReadError = nullptr;
    HTU21DCallback onDataUpdated = nullptr;

    float temperature;
    float humidity;
    uint8_t error = 1;
    uint8_t temperatureError = 1;
    uint8_t humidityError = 1;

    void setup(uint32_t pollingRate = 1000);
    void setCallback(HTU21DCallback onDataUpdatedCallback = nullptr, HTU21DCallback onReadErrorCallback = nullptr);
    uint8_t tick();
};

#endif