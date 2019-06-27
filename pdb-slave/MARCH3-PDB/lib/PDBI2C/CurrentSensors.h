#ifndef CURRENTSENSORS_H
#define CURRENTSENSORS_H

#include <mbed.h>

// See the AD1015 datasheet for more information

class CurrentSensors {
private:
    I2C bus;
    const uint8_t ADS1015_address = 0x4A; // Depending on Address pin
    uint8_t ADS1015_read;
    uint8_t ADS1015_write;

    enum ADS1015RegAddressPtr {
        convReg = 0b00,
        confReg = 0b01,
        loThReg = 0b10,
        hiThReg = 0b11,
    };

    enum ADS1015MuxConfigs {
        PDBCS   = 0b100,
        LVCS1   = 0b101,
        LVCS2   = 0b110,
        HVCS    = 0b111,
    };

    enum ADS1015FSRConfigs {
        fsr6144 = 0b000,
        fsr4096 = 0b001,
        fsr2048 = 0b010,
        fsr1024 = 0b011,
        fsr0512 = 0b100,
        fsr0256 = 0b101,
    };
    ADS1015FSRConfigs FSR;

    enum ADS1015DataRateConfigs {
        dr128sps = 0b000,
        dr250sps = 0b001,
        dr490sps = 0b010,
        dr920sps = 0b011,
        dr1600sps = 0b100,
        dr2400sps = 0b101,
        dr3300sps = 0b110,
    };
    ADS1015DataRateConfigs dataRate;
    

    const float ACS723SensorSensitivity = 400; // mV per Ampere
    const float ACS723SensorOffset = 500; // mV

    const float ACS780SensorSensitivity = 20; // mV per Ampere
    const float ACS780SensorOffset = 0; // mV

    float getLSBSize();
    uint16_t swapBytes(uint16_t data);

    uint16_t readReg();
    bool writeAddrReg(ADS1015RegAddressPtr reg);
    bool writeConfReg(ADS1015MuxConfigs muxConf);

public:
    CurrentSensors(PinName SDA_PIN, PinName SCL_PIN);

    float readPDBCurrent();
    float readLV1Current();
    float readLV2Current();
    float readHVCurrent();

};

#endif // CURRENTSENSORS_H
