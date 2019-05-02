#ifndef CURRENTSENSORS_H
#define CURRENTSENSORS_H

#include <mbed.h>

class CurrentSensors {
private:
    I2C bus;
    const uint8_t ADS1015_address = 0x48; // Depending on Address pin

public:
    CurrentSensors(PinName SDA_PIN, PinName SCL_PIN);

    float readLV1Current();
    float readLV2Current();
    float readHVCurrent();
    float readPDBCurrent();
};

#endif // CURRENTSENSORS_H