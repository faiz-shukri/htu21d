#include "htu21d.h"

#define HTU21D_NO_HOLD_START_MEASURE_HUMIDITY 0xF5
#define HTU21D_NO_HOLD_START_MEASURE_TEMP 0xF3

#define HTU21D_HOLD_START_MEASURE_HUMIDITY 0xE5
#define HTU21D_HOLD_START_MEASURE_TEMP 0xE3

#define HTU21D_WRITE_USER_REGISTER 0xE6
#define HTU21D_READ_USER_REGISTER 0xE7

#define HTU21D_SOFT_RESET 0xFE

#define HTU21D_READ_DELAY 50
HTU21D::HTU21D(TwoWire *wire, uint8_t address)
{
    this->wire = wire;
    this->address = address;
}
void HTU21D::setup(uint32_t pollingRate)
{
    this->pollingRate = pollingRate;
    this->step = 0;
}

uint8_t HTU21D::tick()
{
    switch (step)
    {
    case 0:
    {
        step++;
        break;
    }
    case 1: // wait next sample cycle
    {
        if (timer >= pollingRate)
        {
            timer = 0;
            uint8_t res = requestTemperature();
            e = 0;
            if (res)
            {
                temperatureError = res;
                step = 0;
            }
            else
                step++;
        }

        break;
    }
    case 2: // wait measurement completed
    {
        if (e >= HTU21D_READ_DELAY)
        {
            uint8_t res = readTemperature();
            temperatureError = res;
            if (res)
                step = 0;
            else
                step++;
        }
        break;
    }
    case 3: // wait measurement completed
    {
        uint8_t res = requestHumidity();
        e = 0;
        if (res)
        {
            humidityError = res;
            step = 0;
        }
        else
            step++;

        break;
    }
    case 4: // wait measurement completed
    {
        if (e >= HTU21D_READ_DELAY)
        {
            uint8_t res = readHumidity();
            humidityError = res;
            step = 0; // step++;
        }
        break;
    }
    default:
        step = 0;
        break;
    }

    if (step == 0)
    {
        error = temperatureError | humidityError;
        if (error && onReadError != nullptr)
            onReadError(this);
        if (!error && onDataUpdated != nullptr)
            onDataUpdated(this);
    }
    return step;
}
uint8_t HTU21D::requestHumidity()
{
    wire->beginTransmission(address);           // 7 bit address
    wire->write(HTU21D_NO_HOLD_START_MEASURE_HUMIDITY); // trigger humidity measurement
    return wire->endTransmission();
}
int HTU21D::readResult()
{
    wire->requestFrom(address, (uint8_t)2);

    uint16_t val = 0;
    int v = wire->read();
    if (v < 0)
        return -1;
    val = v << 8;

    v = wire->read();
    if (v < 0)
        return -1;

    val |= v;

    return val;
}
uint8_t HTU21D::readHumidity()
{
    int res = readResult();
    if (res < 0)
        return 1; // error

    humidity = 125 * (res / pow(2, 16)) - 6;
    return 0;
}
uint8_t HTU21D::requestTemperature()
{
    wire->beginTransmission(address);       // 7 bit address
    wire->write(HTU21D_NO_HOLD_START_MEASURE_TEMP); // trigger temp measurement
    return wire->endTransmission();
}

uint8_t HTU21D::readTemperature()
{
    int res = readResult();
    if (res < 0)
        return 1; // error

    temperature = 175.72 * (res / pow(2, 16)) - 46.85;
    return 0;
}

void HTU21D::setCallback(HTU21DCallback onDataUpdatedCallback, HTU21DCallback onReadErrorCallback)
{
    onDataUpdated = onDataUpdatedCallback;
    onReadError = onReadErrorCallback;
}