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

    uint8_t read();
    
public:
    HVOCTriggers(PinName SDA_PIN, PinName SCL_PIN);

    uint8_t readOCTriggers();
};

#endif // HVOCTRIGGER_H