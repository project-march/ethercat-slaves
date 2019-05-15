#ifndef HVOCTRIGGERS_H
#define HVOCTRIGGERS_H

#include <mbed.h>

class HVOCTriggers
{
private:
    I2C bus;
    const uint8_t PCA9534_address = 0x21; // Depending on Address pins
    uint8_t PCA9534_read;
    uint8_t PCA9534_write;

    enum HVOCtriggerPins {
        oct_1 = 0,      // P00 lT17
        oct_2 = 1,      // P01 lT18
        oct_3 = 2,      // P02 lT19
        oct_4 = 3,      // P03 lT20
        oct_5 = 7,      // P07 lT21
        oct_6 = 6,      // P06 lT22
        oct_7 = 5,      // P05 lT23
        oct_8 = 4,      // P04 lT24
    };

    HVOCtriggerPins ocTriggerPins[8] = {oct_1, oct_2, oct_3, oct_4, oct_5, oct_6, oct_7, oct_8};

    bool getBit(uint8_t byte, uint8_t index);
    uint8_t read();
    
public:
    HVOCTriggers(PinName SDA_PIN, PinName SCL_PIN);

    uint8_t readOCTriggers();
};

#endif // HVOCTRIGGER_H