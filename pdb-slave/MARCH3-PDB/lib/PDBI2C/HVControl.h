#ifndef HVCONTROL_H
#define HVCONTROL_H

#include <mbed.h>

enum HVControlPinNames{
    HVOn_1          = 0,        // P00
    HVReset_1       = 1,        // P01
    HVOn_2          = 2,        // P02
    HVReset_2       = 3,        // P03
    HVOn_3          = 4,        // P04
    HVReset_3       = 5,        // P05
    HVOn_4          = 13,       // P15
    HVReset_4       = 12,       // P14
    HVOn_5          = 11,       // P13
    HVReset_5       = 10,       // P12
    HVOn_6          = 9,        // P11
    HVReset_6       = 8,        // P10
};

class HVControl 
{
private:
    I2C bus;
    const uint8_t PCA8575D_address = 0x20; // Depending on Address pins
    uint8_t PCA8575D_read;
    uint8_t PCA8575D_write;
    uint16_t HVControlPins;
    HVControlPinNames resetPins[6] = {HVReset_1, HVReset_2, HVReset_3, HVReset_4, HVReset_5, HVReset_5};
    HVControlPinNames onPins[6] = {HVOn_1, HVOn_2, HVOn_3, HVOn_4, HVOn_5, HVOn_6};

    void write();
    uint16_t read();

    void setBit(uint8_t index);
    void clearBit(uint8_t index);
    bool getBit(uint16_t word, uint8_t index);

public:
    HVControl(PinName SDA_PIN, PinName SCL_PIN);

    bool readHV(HVControlPinNames pin);
    void turnOnHV(HVControlPinNames pin);
    void turnOffHV(HVControlPinNames pin);
    void resetHV(HVControlPinNames pin);

    uint16_t readAllHV();
    void turnOffAllHV();
    void turnOnAllHV();
    void resetAllHV();

    uint8_t readAllReset();
    uint8_t readAllOn();
    void setAllHV(uint8_t code);
};


#endif // HVCONTROL_H
