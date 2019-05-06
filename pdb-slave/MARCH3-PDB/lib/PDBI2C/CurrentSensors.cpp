#include "CurrentSensors.h"

// Constructor
CurrentSensors::CurrentSensors(PinName SDA_PIN, PinName SCL_PIN) : bus(SDA_PIN, SCL_PIN){
    this->ADS1015_read = (ADS1015_address << 1) | 0x01; // Shift left and set LSB to one
    this->ADS1015_write = (ADS1015_address << 1) & 0xFE; // Shift left and set LSB to zero
    this->FSR = fsr2048; // Set default Full Scale Range
    this->dataRate = dr1600sps; // Set default data rate
}

// This helper function returns the size (in Volt) of the least significant bit of the read data based on the set Full Scale Range
float CurrentSensors::getLSBSize(){
    switch(this->FSR){
        case fsr6144:
            return 3.0f;
        case fsr4096:
            return 2.0f;
        case fsr2048:
            return 1.0f;
        case fsr1024:
            return 0.5f;
        case fsr0512:
            return 0.25f;
        case fsr0256:
            return 0.125f;
        default:
            return 0.0f;
    }
}

// This helper function swaps the two bytes of the input argument
uint16_t CurrentSensors::swapBytes(uint16_t data){
    uint8_t MSB, LSB;
    LSB = ((data >> 8) & 0xFF); // The MSB of the data should be the LSB...
    MSB = (data & 0xFF); // ...and vice versa
    uint16_t swapped = (MSB << 8) | LSB; // Stitch the two bytes back together
    return swapped;
}

// This function reads two bytes from the ADS1015
uint16_t CurrentSensors::readReg(){
    uint16_t read_data = 0;
    this->bus.read(this->ADS1015_read, (char*) &read_data, 2);
    return read_data;
}

// This function writes a byte to the ADS1015 which sets which register to read/write from/to
bool CurrentSensors::writeAddrReg(ADS1015RegAddressPtr reg){
    uint8_t addressPtrReg = reg;
    return this->bus.write(this->ADS1015_write, (char*) &addressPtrReg, 1);
}

// This function writes the Configuration register of the ADS1015
bool CurrentSensors::writeConfReg(ADS1015MuxConfigs muxConf){
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
    bool ack = this->bus.write(this->ADS1015_write, (char*) &write_data, 3);
    wait_ms(1); // Give the ADS some time to adjust settings
    return ack;
}

// ---------------- Public functions ----------------

// This function reads the PDB current and returns the current as a float
float CurrentSensors::readPDBCurrent(){
    // Set Full Scale Range to correct value for the PDB current sensor
    this->FSR = fsr2048;
    // Write to config register to get correct settings for reading
    this->writeConfReg(PDBCS);
    // First write to Address Pointer Register indicating we want to read the conversion register
    this->writeAddrReg(convReg);
    // Read the two bytes that are transmitted by the slave
    uint16_t read_data = this->readReg();
    // First swap the two bytes, because the LSB and MSB are received in the wrong order
    read_data = this->swapBytes(read_data);
    // Throw away the last four bits because they are always 0
    read_data = read_data >> 4;
    // Convert to a (float) voltage using the FSR
    float convertedVoltage = read_data * this->getLSBSize() / 1000; // In Volt
    // Convert from measured voltage to the actual current through the current sensor sensitivity and offset
    // Todo: fix this additional offset (now based on empirical test data from power supply delivered power without peripherals)
    float convertedCurrent = (convertedVoltage * 1000 - this->ACS723SensorOffset - 40) / this->ACS723SensorSensitivity; // In Ampere
    return convertedCurrent;
}

// This function reads the LV current of net 1 and returns the current as a float
float CurrentSensors::readLV1Current(){
    // Set Full Scale Range to correct value for the PDB current sensor
    this->FSR = fsr2048;
    // Write to config register to get correct settings for reading
    this->writeConfReg(LVCS1);
    // First write to Address Pointer Register indicating we want to read the conversion register
    this->writeAddrReg(convReg);
    // Read the two bytes that are transmitted by the slave
    uint16_t read_data = this->readReg();
    // First swap the two bytes, because the LSB and MSB are received in the wrong order
    read_data = this->swapBytes(read_data);
    // Throw away the last four bits because they are always 0
    read_data = read_data >> 4;
    // Convert to a (float) voltage using the FSR
    float convertedVoltage = read_data * this->getLSBSize() / 1000; // In Volt
    // Convert from measured voltage to the actual current through the current sensor sensitivity and offset
    // Todo: fix this additional offset (now based on empirical test data from different loads on LV connector)
    float convertedCurrent = (convertedVoltage * 1000 - this->ACS723SensorOffset - 180) / this->ACS723SensorSensitivity; // In Ampere
    return convertedCurrent;
}

// This function reads the LV current of net 2 and returns the current as a float
float CurrentSensors::readLV2Current(){
    return 0; // Does not exist on M3 PDB
}

// This function reads the total HV current and returns the current as a float
float CurrentSensors::readHVCurrent(){
    return 0; // Does not exist on M3 PDB
}

