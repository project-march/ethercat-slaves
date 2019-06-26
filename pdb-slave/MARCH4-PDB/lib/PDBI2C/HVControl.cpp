#include "HVControl.h"

// Constructor
HVControl::HVControl(PinName SDA_PIN, PinName SCL_PIN) : bus(SDA_PIN, SCL_PIN){
    this->PCA8575D_read = (PCA8575D_address << 1) | 0x01; // Shift left and set LSB to one
    this->PCA8575D_write = (PCA8575D_address << 1) & 0xFE; // Shift left and set LSB to zero
    
    // Write initial values
    this->HVControlPins = 0xFF; // HV off and not resetting
    this->write();
    
    // Initial reset
    this->resetAllHV();
}

// This helper function sets a bit with given index in the HVControlPins
void HVControl::setBit(uint8_t index){
    this->HVControlPins |= (1 << index);
}

// This helper function clears a bit with given index in the HVControlPins
void HVControl::clearBit(uint8_t index){
    this->HVControlPins &= (~(1 << index));
}

// This helper function gets and returns a bit with given index from a given word
bool HVControl::getBit(uint16_t word, uint8_t index){
    return ((word >> index) & 0x01);
}

// This function writes the two bytes in HVControlPins to the PCA8575
void HVControl::write(){
    uint16_t write_data = this->HVControlPins;
    bus.write(this->PCA8575D_write, (char*) &write_data, 2);
}

// This function reads and returns two bytes from the PCA8575
uint16_t HVControl::read(){
    uint16_t read_data = 0;
    bus.read(this->PCA8575D_read, (char*) &read_data, 2);
    return read_data;
}

// ---------------- Public functions ----------------

// This functions reads and returns one specific HV status from the PCA8575
bool HVControl::readHV(HVOnPinNames pin){
    uint16_t read_data = this->read();
    return !this->getBit(read_data, pin);
}

// This function turns on one specific HV via the PCA8575
void HVControl::turnOnHV(HVOnPinNames pin){
    this->clearBit(pin); // Logical 0 means HV on
    this->write();
}

// This function turns off one specific HV via the PCA8575
void HVControl::turnOffHV(HVOnPinNames pin){
    this->setBit(pin); // Logical 1 means HV off
    this->write();
}

// This function resets one specific HV via the PCA8575
void HVControl::resetHV(HVResetPinNames pin){
    this->clearBit(pin);
    this->write();
    wait_ms(20);
    this->setBit(pin);
    this->write();
}

// This function turns on all HVs via the PCA8575
void HVControl::turnOnAllHV(){
    // Clear all on bits (logical 0 means HV on)
    for(int i = 0; i < sizeof(this->onPins)/sizeof(this->onPins[0]); i++){
        this->clearBit(this->onPins[i]);
    }
    this->write();
}

// This function turns off all HVs via the PCA8575
void HVControl::turnOffAllHV(){
    // Set all on bits (logical 1 means HV off)
    for(int i = 0; i < sizeof(this->onPins)/sizeof(this->onPins[0]); i++){
        this->setBit(this->onPins[i]);
    }
    this->write();
}

// This function resets all HVs via the PCA8575
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

// This function reads and returns all HVreset states from the PCA8575
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

// This function reads and returns all HVon states from the PCA8575
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

// This function sets all HVs based on the code given via the PCA8575
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

void HVControl::setAllHVStagedStartup(uint8_t code){
    // Loop through all HVOn pins and turn on HV depending on code
    // Staged startup is meant to turn on all HV nets seperately, to minimize inrush currents
    // Debug: turn on LED 4 if staged startup is called
    DigitalOut debugLED(LED4, false);
    debugLED = true;
    for(int i = 0; i < sizeof(this->onPins)/sizeof(this->onPins[0]); i++){
        if(this->getBit(code, i)){
            // Turn on HV
            this->clearBit(this->onPins[i]);
            wait_ms(100); // Wait for a while before turning on a new net
        }
        else{
            // Turn off HV
            this->setBit(this->onPins[i]);
        }
        this->write();
    }
    debugLED = false;
}