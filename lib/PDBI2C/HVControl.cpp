#include "HVControl.h"

// Constructor
HVControl::HVControl(PinName SDA_PIN, PinName SCL_PIN) : bus(SDA_PIN, SCL_PIN)
{
  this->PCA9534PW_read = (PCA9534PW_address << 1) | 0x01;   // Shift left and set LSB to one
  this->PCA9534PW_write = (PCA9534PW_address << 1) & 0xFE;  // Shift left and set LSB to zero

  // Write initial values
  while (this->read() != 0x00)
  {
    this->HVControlPins = 0x00;  // HV off and not resetting
    this->write();
  }

  uint16_t write_configuration = (PCA9534PW_configuration << 8) | 0x00;  // Configure all IO pins as output
  bus.write(this->PCA9534PW_write, (char*)&write_configuration, 2);
}

// This helper function sets a bit with given index in the HVControlPins
void HVControl::setBit(uint8_t index)
{
  this->HVControlPins |= (1 << index);
}

// This helper function clears a bit with given index in the HVControlPins
void HVControl::clearBit(uint8_t index)
{
  this->HVControlPins &= (~(1 << index));
}

// This helper function gets and returns a bit with given index from a given word
bool HVControl::getBit(uint8_t word, uint8_t index)
{
  return ((word >> index) & 0x01);
}

// This function writes the two bytes in HVControlPins to the PCA8575
void HVControl::write()
{
  uint16_t write_data = (this->HVControlPins << 8) | PCA9534PW_output_port;
  ;
  bus.write(this->PCA9534PW_write, (char*)&write_data, 2);
}

// This function reads and returns two bytes from the PCA8575
uint8_t HVControl::read()
{
  uint8_t command = this->PCA9534PW_output_port;
  bus.write(this->PCA9534PW_read, (char*)&command, 1);

  uint8_t read_data = 0;
  bus.read(this->PCA9534PW_read, (char*)&read_data, 1);
  return read_data;
}

// ---------------- Public functions ----------------

// This functions reads and returns one specific HV status from the PCA8575
bool HVControl::readHV(HVOnPinNames pin)
{
  uint8_t read_data = this->read();
  return !this->getBit(read_data, pin);
}

// This function turns on one specific HV via the PCA8575
void HVControl::turnOnHV(HVOnPinNames pin)
{
  this->setBit(pin);  // Logical 1 means HV on
  this->write();
}

// This function turns off one specific HV via the PCA8575
void HVControl::turnOffHV(HVOnPinNames pin)
{
  this->clearBit(pin);  // Logical 0 means HV off
  this->write();
}

// This function turns on all HVs via the PCA8575
void HVControl::turnOnAllHV()
{
  // Set all on bits (logical 1 means HV on)
  for (int i = 0; i < sizeof(this->onPins) / sizeof(this->onPins[0]); i++)
  {
    this->setBit(this->onPins[i]);
  }
  this->write();
}

// This function turns off all HVs via the PCA8575
void HVControl::turnOffAllHV()
{
  // Clear all on bits (logical 0 means HV off)
  for (int i = 0; i < sizeof(this->onPins) / sizeof(this->onPins[0]); i++)
  {
    this->clearBit(this->onPins[i]);
  }
  this->write();
}

// This function reads and returns all HVon states from the PCA8575
uint8_t HVControl::readAllOn()
{
  uint8_t read_data = this->read();
  uint8_t onStates = 0;
  // Loop through all HVOn pins
  for (int i = 0; i < sizeof(this->onPins) / sizeof(this->onPins[0]); i++)
  {
    if (this->getBit(read_data, this->onPins[i]))
    {                        // Check if bit is 1 (= HV on)
      onStates |= (1 << i);  // Set bit in onStates
    }
  }
  return onStates;
}

// This function sets all HVs based on the code given via the PCA8575
void HVControl::setAllHV(uint8_t code)
{
  // Loop through all HVOn pins and turn on HV depending on code
  for (int i = 0; i < sizeof(this->onPins) / sizeof(this->onPins[0]); i++)
  {
    if (this->getBit(code, i))
    {
      // Turn on HV
      this->setBit(this->onPins[i]);
    }
    else
    {
      // Turn off HV
      this->clearBit(this->onPins[i]);
    }
  }
  this->write();
}

// This function does the same as setAllHV, but turns on all HV nets seperately
// Staged startup is designed with the purpose of minimizing inrush currents
void HVControl::setAllHVStagedStartup(uint8_t code)
{
  // Turn on LED 4 when staged startup is called
  DigitalOut stagedStartupLED(LED4, false);
  stagedStartupLED = true;
  // Loop through all HVOn pins and turn on HV depending on code
  for (int i = 0; i < sizeof(this->onPins) / sizeof(this->onPins[0]); i++)
  {
    if (this->getBit(code, i))
    {
      // Turn on HV
      this->setBit(this->onPins[i]);
      wait_us(100000);  // Wait for a while before turning on a new net
    }
    else
    {
      // Turn off HV
      this->clearBit(this->onPins[i]);
    }
    this->write();
  }
  // Turn off LED 4 after staged startup has finished
  stagedStartupLED = false;
}