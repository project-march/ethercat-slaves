#include "PDBI2C.h"

PDBI2C::PDBI2C(PinName SDA_PIN, PinName SCL_PIN, Serial * pc) : bus(SDA_PIN, SCL_PIN){
    this->PCA8575D_read = (PCA8575D_address << 1) | 0x01; // Shift left and set LSB to one
    this->PCA8575D_write = (PCA8575D_address << 1) & 0xFE; // Shift left and set LSB to zero
    this->HVControlPins = 0xFF;
    
    // Initial write
    uint16_t write_data = this->HVControlPins;
    uint8_t ack = bus.write(this->PCA8575D_write, (char*) &write_data, 2);
    pc->printf("\r\nData read = %d, address = %d, ack = %d", read_data, this->PCA8575D_read, ack);

}

void PDBI2C::setHVControlPin(uint8_t pin, bool value, Serial * pc){
    if(value){
        // Set bit
        this->HVControlPins |= (1 << pin);
    }
    else{
        // Clear bit
        this->HVControlPins &= (~(1 << pin));
    }
    uint16_t write_data = this->HVControlPins;
    uint8_t ack = bus.write(this->PCA8575D_write, (char*) &write_data, 2);
    pc->printf("\r\nData written = %d, address = %d, ack = %d", write_data, this->PCA8575D_write, ack);
}

bool PDBI2C::getHVControlPin(uint8_t pin, Serial * pc){
    uint16_t read_data = 0;
    uint8_t ack = bus.read(this->PCA8575D_read, (char*) &read_data, 2);
    pc->printf("\r\nData read = %d, address = %d, ack = %d", read_data, this->PCA8575D_read, ack);
}

