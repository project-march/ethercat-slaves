#ifndef PDBI2C_H
#define PDBI2C_H

#include <mbed.h>

class PDBI2C 
{
private:
    I2C bus;
    const uint8_t PCA8575D_address = 0x20; // Depending on Address pins
    uint8_t PCA8575D_read;
    uint8_t PCA8575D_write;
    uint16_t HVControlPins;

public:
    PDBI2C(PinName SDA_PIN, PinName SCL_PIN, Serial * pc);
    void setHVControlPin(uint8_t pin, bool value, Serial * pc);
    bool getHVControlPin(uint8_t pin, Serial * pc);
};


#endif // PDB_I2C_H
