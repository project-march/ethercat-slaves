#include "HVControl.h"

HVControl::HVControl(PinName SDA_PIN, PinName SCL_PIN) : bus(SDA_PIN, SCL_PIN){
    this->PCA8575D_read = (PCA8575D_address << 1) | 0x01; // Shift left and set LSB to one
    this->PCA8575D_write = (PCA8575D_address << 1) & 0xFE; // Shift left and set LSB to zero
    this->HVControlPins = 0xFF;
    
    // Initial reset
    this->resetAllHV();
}

void HVControl::write(){
    uint16_t write_data = this->HVControlPins;
    bus.write(this->PCA8575D_write, (char*) &write_data, 2);
}

uint16_t HVControl::read(){
    uint16_t read_data = 0;
    bus.read(this->PCA8575D_read, (char*) &read_data, 2);
    return read_data;
}

bool HVControl::readHV(HVControlPinNames pin){
    uint16_t read_data = this->read();
    return ((read_data >> pin) & 0x01); // Return the bit specified by the pin argument
}

void HVControl::turnOnHV(HVControlPinNames pin){
    this->HVControlPins &= (~(1 << pin)); // Clear bit (logical 0 means HV on)
    this->write();
}

void HVControl::turnOffHV(HVControlPinNames pin){
    this->HVControlPins |= (1 << pin); // Set bit (logical 1 means HV off)
    this->write();
}

void HVControl::resetHV(HVControlPinNames pin){
    // Clear the reset bit
    this->HVControlPins &= (~(1 << pin));
    this->write();
    wait_ms(20);
    // Set the reset bit
    this->HVControlPins |= (1 << pin);
    this->write();
}

uint16_t HVControl::readAllHV(){
    uint16_t read_data = this->read();
    // Todo: update HVControlPins based on result?
    return read_data;
}

void HVControl::turnOffAllHV(){
    // Set all on bits (logical 1 means HV off)
    for(int i = 0; i < sizeof(this->onPins)/sizeof(this->onPins[0]); i++){
        this->HVControlPins |= (1 << this->onPins[i]);
    }
    this->write();
}

void HVControl::turnOnAllHV(){
    // Clear all on bits (logical 0 means HV on)
    for(int i = 0; i < sizeof(this->onPins)/sizeof(this->onPins[0]); i++){
        this->HVControlPins &= (~(1 << this->onPins[i]));
    }
    this->write();
}

void HVControl::resetAllHV(){
    // Clear all reset bits
    for(int i = 0; i < sizeof(this->resetPins)/sizeof(this->resetPins[0]); i++){
        this->HVControlPins &= (~(1 << this->resetPins[i]));
    }
    this->write();
    wait_ms(20);
    // Set all reset bits
    for(int i = 0; i < sizeof(this->resetPins)/sizeof(this->resetPins[0]); i++){
        this->HVControlPins |= (1 << this->resetPins[i]);
    }
    this->write();
}

void HVControl::setAllHV(uint8_t code){
    
}