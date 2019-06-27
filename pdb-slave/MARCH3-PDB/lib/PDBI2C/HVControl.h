#ifndef HVCONTROL_H
#define HVCONTROL_H

#include <mbed.h>

enum HVOnPinNames { // Todo: Change based on M4 PDB! Give names like LKFE_HVon
    HVOn_1          = 0,        // P00 IT1
    HVOn_2          = 2,        // P02 IT3
    HVOn_3          = 4,        // P04 IT5
    HVOn_4          = 13,       // P15 IT7
    HVOn_5          = 11,       // P13 IT9
    HVOn_6          = 9,        // P11 IT11
};

enum HVResetPinNames {
    HVReset_1       = 1,        // P01 IT2
    HVReset_2       = 3,        // P03 IT4
    HVReset_3       = 5,        // P05 IT6
    HVReset_4       = 12,       // P14 IT8
    HVReset_5       = 10,       // P12 IT10
    HVReset_6       = 8,        // P10 IT12
};

class HVControl 
{
private:
    I2C bus;
    const uint8_t PCA8575D_address = 0x20; // Depending on Address pins
    uint8_t PCA8575D_read;
    uint8_t PCA8575D_write;
    uint16_t HVControlPins;

    HVResetPinNames resetPins[6] = {HVReset_1, HVReset_2, HVReset_3, HVReset_4, HVReset_5, HVReset_6}; // Todo: Change based on joint order
    HVOnPinNames onPins[6] = {HVOn_1, HVOn_2, HVOn_3, HVOn_4, HVOn_5, HVOn_6}; // Todo: Change based on joint order

    void setBit(uint8_t index);
    void clearBit(uint8_t index);
    bool getBit(uint16_t word, uint8_t index);

    void write();
    uint16_t read();

public:
    HVControl(PinName SDA_PIN, PinName SCL_PIN);

    bool readHV(HVOnPinNames pin);
    void turnOnHV(HVOnPinNames pin);
    void turnOffHV(HVOnPinNames pin);
    void resetHV(HVResetPinNames pin);

    void turnOnAllHV();
    void turnOffAllHV();
    void resetAllHV();

    uint8_t readAllReset();
    uint8_t readAllOn();
    void setAllHV(uint8_t code);
};


#endif // HVCONTROL_H
