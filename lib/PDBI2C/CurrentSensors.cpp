#include "CurrentSensors.h"

// Constructor
CurrentSensors::CurrentSensors(PinName SDA_PIN, PinName SCL_PIN) : bus(SDA_PIN, SCL_PIN)
{
  this->ADC128D818_read = (ADC128D818_address << 1) | 0x01;   // Shift left and set LSB to one
  this->ADC128D818_write = (ADC128D818_address << 1) & 0xFE;  // Shift left and set LSB to zero
}

// This helper function swaps the two bytes of the input argument
uint16_t CurrentSensors::swapBytes(uint16_t data)
{
  uint8_t MSB, LSB;
  LSB = ((data >> 8) & 0xFF);           // The MSB of the data should be the LSB...
  MSB = (data & 0xFF);                  // ...and vice versa
  uint16_t swapped = (MSB << 8) | LSB;  // Stitch the two bytes back together
  return swapped;
}

// This function reads two bytes from the ADS1015
uint16_t CurrentSensors::readRegister()
{
  uint16_t read_data = 0;
  this->bus.read(this->ADC128D818_read, (char*)&read_data, 2);
  return read_data;
}

// This function writes a byte to the ADS1015 which sets which register to read/write from/to
bool CurrentSensors::writeRegisterAddress(ADC128D818RegisterAddresses registerAddress)
{
  uint8_t registerAddressPtr = registerAddress;
  return this->bus.write(this->ADC128D81_write, (char*)&addressPtrReg, 1);
}

// This function writes the Configuration register of the ADS1015
bool CurrentSensors::writeConfReg(ADS1015MuxConfigs muxConf)
{
  uint8_t write_data[3] = { 0, 0, 0 };

  // Set Address Pointer Register
  write_data[0] = confReg;  // Tell ADS1015 we're writing to the configuration register

  // MSB of the Config Register
  write_data[1] |= (1 << 7);          // Set OS bit
  write_data[1] |= (muxConf << 4);    // Set multiplexer configuration
  write_data[1] |= (this->FSR << 1);  // Set Full Scale Range

  // LSB of the Config Register
  write_data[2] |= (this->dataRate << 5);  // Set data rate
  write_data[2] |= 0b11;                   // Disable comparator

  // Write all three bytes to the ADS1015
  bool ack = this->bus.write(this->ADS1015_write, (char*)&write_data, 3);
  wait_us(1000);  // Give the ADS some time to adjust settings
  return ack;
}

// ---------------- Public functions ----------------

// This function reads the PDB current and returns the current as a float
float CurrentSensors::readPDBCurrent()
{
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
  float convertedVoltage = read_data * this->getLSBSize() / 1000;  // In Volt
  // Convert from measured voltage to the actual current through the current sensor sensitivity and offset
  // Todo: fix this additional offset (now based on empirical test data from power supply delivered power without
  // peripherals)
  float convertedCurrent =
      (convertedVoltage * 1000 - this->ACS723SensorOffset - 40) / this->ACS723SensorSensitivity;  // In Ampere
  return convertedCurrent;
}

// This function reads the LV current of net 1 and returns the current as a float
float CurrentSensors::readLV1Current()
{
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
  float convertedVoltage = read_data * this->getLSBSize() / 1000;  // In Volt
  // Convert from measured voltage to the actual current through the current sensor sensitivity and offset
  // Todo: fix this additional offset (now based on empirical test data from different loads on LV connector)
  float convertedCurrent =
      (convertedVoltage * 1000 - this->ACS723SensorOffset - 177) / this->ACS723SensorSensitivity;  // In Ampere
  return convertedCurrent;
}

// This function reads the LV current of net 2 and returns the current as a float
float CurrentSensors::readLV2Current()
{
  // Set Full Scale Range to correct value for the PDB current sensor
  this->FSR = fsr2048;
  // Write to config register to get correct settings for reading
  this->writeConfReg(LVCS2);
  // First write to Address Pointer Register indicating we want to read the conversion register
  this->writeAddrReg(convReg);
  // Read the two bytes that are transmitted by the slave
  uint16_t read_data = this->readReg();
  // First swap the two bytes, because the LSB and MSB are received in the wrong order
  read_data = this->swapBytes(read_data);
  // Throw away the last four bits because they are always 0
  read_data = read_data >> 4;
  // Convert to a (float) voltage using the FSR
  float convertedVoltage = read_data * this->getLSBSize() / 1000;  // In Volt
  // Convert from measured voltage to the actual current through the current sensor sensitivity and offset
  // Todo: fix this additional offset (now based on empirical test data from different loads on LV connector)
  float convertedCurrent =
      (convertedVoltage * 1000 - this->ACS723SensorOffset - 177) / this->ACS723SensorSensitivity;  // In Ampere
  return convertedCurrent;
}

// This function reads the total HV current and returns the current as a float
float CurrentSensors::readHVCurrent()
{
  // Set Full Scale Range to correct value for the PDB current sensor
  this->FSR = fsr2048;
  // Write to config register to get correct settings for reading
  this->writeConfReg(HVCS);
  // First write to Address Pointer Register indicating we want to read the conversion register
  this->writeAddrReg(convReg);
  // Read the two bytes that are transmitted by the slave
  uint16_t read_data = this->readReg();
  // First swap the two bytes, because the LSB and MSB are received in the wrong order
  read_data = this->swapBytes(read_data);
  // Throw away the last four bits because they are always 0
  read_data = read_data >> 4;
  // Convert to a (float) voltage using the FSR
  float convertedVoltage = read_data * this->getLSBSize() / 1000;  // In Volt
  // Convert from measured voltage to the actual current through the current sensor sensitivity and offset
  // Todo: fix this additional offset (now based on empirical test data from different loads on LV connector)
  float convertedCurrent =
      (convertedVoltage * 1000 - this->ACS780SensorOffset) / this->ACS780SensorSensitivity;  // In Ampere
  return convertedCurrent;
}
