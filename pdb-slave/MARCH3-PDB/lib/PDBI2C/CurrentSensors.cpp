#include "CurrentSensors.h"

CurrentSensors::CurrentSensors(PinName SDA_PIN, PinName SCL_PIN) : bus(SDA_PIN, SCL_PIN){
    this->ADS1015_read = (ADS1015_address << 1) | 0x01; // Shift left and set LSB to one
    this->ADS1015_write = (ADS1015_address << 1) & 0xFE; // Shift left and set LSB to zero
    this->FSR = 0b010; // Set default Full Scale Range
    this->dataRate = 0b100; // Set default data rate
}

float CurrentSensors::getLSBSize(){
    switch(this->FSR){
        case 0b000: // 6.144 V
            return 3.0f;
        case 0b001: // 4.096 V
            return 2.0f;
        case 0b010: // 2.048 V
            return 1.0f;
        case 0b011: // 1.024 V
            return 0.5f;
        case 0b100: // 0.512 V
            return 0.25f;
        case 0b101: // 0.256 V
            return 0.125f;
        case 0b110: // 0.256 V
            return 0.125f;
        case 0b111: // 0.256 V
            return 0.125f;
        default:
            return 0.0f;
    }
}

uint16_t CurrentSensors::readReg(){
    uint16_t read_data = 0;
    this->bus.read(this->ADS1015_read, (char*) &read_data, 2);
    return read_data;
}

bool CurrentSensors::writeAddrReg(AD1015RegAddressPtr reg){
    uint8_t addressPtrReg = reg;
    return this->bus.write(this->ADS1015_write, (char*) &addressPtrReg, 1);
}
bool CurrentSensors::writeConfReg(AD1015MuxConfigs muxConf){
    uint8_t write_data[3] = {0, 0, 0};
    
    // Set Address Pointer Register
    write_data[0] = confReg; // Tell ADS1015 we're writing to the configuration register
    
    // MSB of the Config Register
    write_data[1] |= (1 << 7); // Set OS bit
    write_data[1] |= (muxConf << 4); // Set multiplexer configuration
    write_data[1] |= (this->FSR << 1); // Set Full Scale Range

    // LSB of the Config Register
    write_data[2] |= (this->dataRate << 5); // Set data rate
    write_data[2] |= 0b11; // Disable comparator

    // Write all three bytes to the ADS1015
    return this->bus.write(this->ADS1015_write, (char*) &write_data, 3);
}

float CurrentSensors::readPDBCurrent(){
    // Write to config register to get correct settings for reading
    this->writeConfReg(PDBCS);
    // First write to Address Pointer Register indicating we want to read the conversion register
    this->writeAddrReg(convReg);
    // Read the two bytes that are transmitted by the slave
    uint16_t read_data = this->readReg();
    // Convert the read data to a float
    // First swap the two bytes, because the LSB and MSB are received in the wrong order
    uint8_t MSB, LSB;
    LSB = ((read_data >> 8) & 0xFF); // The MSB of the received data should be the LSB...
    MSB = (read_data & 0xFF); // ...and vice versa
    read_data = (MSB << 8) | LSB; // Stitch the two bytes back together
    // Throw away the last four bits because they are always 0
    read_data = read_data >> 4;
    // Convert to a (float) voltage using the FSR
    float convertedVoltage = read_data * this->getLSBSize() / 1000; // In Volt
    // Convert from measured voltage to the actual current through the current sensor sensitivity and offset
    float convertedCurrent = (convertedVoltage * 1000 - this->sensorOffset) / this->sensorSensitivity; // In Ampere
    return convertedCurrent;
}

float CurrentSensors::readLV1Current(){
    return 0;
}

float CurrentSensors::readLV2Current(){
    return 0;
}

float CurrentSensors::readHVCurrent(){
    return 0;
}

