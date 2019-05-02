#ifndef CURRENTSENSORS_H
#define CURRENTSENSORS_H

#include <mbed.h>

class CurrentSensors {
private:
    I2C bus;
    const uint8_t ADS1015_address = 0x4A; // Depending on Address pin // Todo: Change for M4 PDB!

public:
    CurrentSensors(PinName SDA_PIN, PinName SCL_PIN);

    float readPDBCurrent();
    float readLV1Current();
    float readLV2Current();
    float readHVCurrent();

};

#endif // CURRENTSENSORS_H