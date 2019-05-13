#include "HVOCTriggers.h"

// Constructor
HVOCTriggers::HVOCTriggers(PinName SDA_PIN, PinName SCL_PIN) : bus(SDA_PIN, SCL_PIN){
    this->PCA9534_read = (PCA9534_address << 1) | 0x01; // Shift left and set LSB to one
    this->PCA9534_write = (PCA9534_address << 1) & 0xFE; // Shift left and set LSB to zero
}

// This function reads the status of the 8 pins of the PCA9534
uint8_t HVOCTriggers::read(){
    uint8_t command = 0; // Read command
    this->bus.write(this->PCA9534_read, (char*) &command, 1);
    uint8_t read_data = 0;
    this->bus.read(this->PCA9534_read, (char*) &read_data, 1);
    return read_data;
}

// ---------------- Public functions ----------------

// This function reads and returns the status of the 8 OCTriggers
uint8_t HVOCTriggers::readOCTriggers(){
    uint8_t read_data = this->read();
    // Todo: Decode based on arbitrarily decided joint order
    return read_data;
}