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

void HVControl::setBit(uint8_t index){
    this->HVControlPins |= (1 << index);
}

void HVControl::clearBit(uint8_t index){
    this->HVControlPins &= (~(1 << index));
}

bool HVControl::getBit(uint16_t word, uint8_t index){
    return ((word >> index) & 0x01);
}

bool HVControl::readHV(HVControlPinNames pin){
    uint16_t read_data = this->read();
    return this->getBit(read_data, pin); // Return the bit specified by the pin argument
}

void HVControl::turnOnHV(HVControlPinNames pin){
    this->clearBit(pin); // Logical 0 means HV on
    this->write();
}

void HVControl::turnOffHV(HVControlPinNames pin){
    this->setBit(pin); // Logical 1 means HV off
    this->write();
}

void HVControl::resetHV(HVControlPinNames pin){
    this->clearBit(pin);
    this->write();
    wait_ms(20);
    this->setBit(pin);
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
        this->setBit(this->onPins[i]);
    }
    this->write();
}

void HVControl::turnOnAllHV(){
    // Clear all on bits (logical 0 means HV on)
    for(int i = 0; i < sizeof(this->onPins)/sizeof(this->onPins[0]); i++){
        this->clearBit(this->onPins[i]);
    }
    this->write();
}

void HVControl::resetAllHV(){
    // Clear all reset bits
    for(int i = 0; i < sizeof(this->resetPins)/sizeof(this->resetPins[0]); i++){
        this->clearBit(this->resetPins[i]);
    }
    this->write();
    wait_ms(20);
    // Set all reset bits
    for(int i = 0; i < sizeof(this->resetPins)/sizeof(this->resetPins[0]); i++){
        this->setBit(this->resetPins[i]);
    }
    this->write();
}

uint8_t HVControl::readAllReset(){
    uint16_t read_data = this->read();
    uint8_t resetStates = 0;
    // Loop through all reset pins
    for(int i = 0; i < sizeof(this->resetPins)/sizeof(this->resetPins[0]); i++){
        if(!this->getBit(read_data, this->resetPins[i])){ // Check if bit is zero (= reset)
            resetStates |= (1 << i); // Set bit in resetStates
        }
    }
    return resetStates;
}

uint8_t HVControl::readAllOn(){
    uint16_t read_data = this->read();
    uint8_t onStates = 0;
    // Loop through all HVOn pins
    for(int i = 0; i < sizeof(this->onPins)/sizeof(this->onPins[0]); i++){
        if(!this->getBit(read_data, this->onPins[i])){ // Check if bit is zero (= HV on)
            onStates |= (1 << i); // Set bit in onStates
        }
    }
    return onStates;
}

void HVControl::setAllHV(uint8_t code){
    // Loop through all HVOn pins and turn on HV depending on code
    for(int i = 0; i < sizeof(this->onPins)/sizeof(this->onPins[0]); i++){
        if(this->getBit(code, i)){
            // Turn on HV
            this->clearBit(this->onPins[i]);
        }
        else{
            // Turn off HV
            this->setBit(this->onPins[i]);
        }
    }
    this->write();
}