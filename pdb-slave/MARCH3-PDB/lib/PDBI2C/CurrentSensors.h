#ifndef CURRENTSENSORS_H
#define CURRENTSENSORS_H

#include <mbed.h>

// See the AD1015 datasheet for more information

class CurrentSensors {
private:
    I2C bus;
    const uint8_t ADS1015_address = 0x4A; // Depending on Address pin // Todo: Change for M4 PDB!
    uint8_t ADS1015_read;
    uint8_t ADS1015_write;

    enum AD1015RegAddressPtr {
        convReg = 0b00,
        confReg = 0b01,
        loThReg = 0b10,
        hiThReg = 0b11,
    };

    enum AD1015MuxConfigs {
        PDBCS   = 0b100,
        LVCS1   = 0b101,
        LVCS2   = 0b110,
        HVCS    = 0b111,
    };

    uint8_t FSR;
    uint8_t dataRate;
    const float sensorSensitivity = 400; // mV per Ampere
    const float sensorOffset = 500; // mV

    float getLSBSize();

    uint16_t readReg();
    bool writeAddrReg(AD1015RegAddressPtr reg);
    bool writeConfReg(AD1015MuxConfigs muxConf);

public:
    CurrentSensors(PinName SDA_PIN, PinName SCL_PIN);

    float readPDBCurrent();
    float readLV1Current();
    float readLV2Current();
    float readHVCurrent();

};

#endif // CURRENTSENSORS_H