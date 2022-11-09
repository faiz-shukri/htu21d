#include <Arduino.h>
#include <Wire.h>
#include "htu21d.h"

#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22
#define WIRE_FREQUENCY 400000
#define WIRE_TIMEOUT 100

#define HTU21D_ADDRESS 0x40

HTU21D htu21d = HTU21D(&Wire, HTU21D_ADDRESS);
void sensorDataUpdated(HTU21D *sender);
void sensorReadError(HTU21D *sender);

void setup()
{
    Serial.begin(115200);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, WIRE_FREQUENCY);
    Wire.setTimeOut(WIRE_TIMEOUT);

    htu21d.setup(500); // poll every 500ms

    // attach event callbacks
    htu21d.setCallback(sensorDataUpdated, sensorReadError);
}

void loop()
{
    // run tick
    htu21d.tick();
}

void sensorDataUpdated(HTU21D *sender)
{
    // New data updated
    Serial.print(millis());
    Serial.print(" [Data Updated] Temp: ");
    Serial.print(sender->temperature, 2);
    Serial.print("*C   Humidity: ");
    Serial.print(sender->humidity, 2);
    Serial.println("%RH");
}
void sensorReadError(HTU21D *sender)
{
    // Sensor read error
    Serial.print(millis());
    Serial.println(" [Read Error]");
}