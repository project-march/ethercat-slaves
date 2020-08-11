#ifndef HVCONTROL_H
#define HVCONTROL_H

#include <mbed.h>

enum HVOnPinNames
{
  HVOn_1 = 0,  // P00  lT1
  HVOn_2 = 1,  // P01  lT2
  HVOn_3 = 2,  // P02  lT3
  HVOn_4 = 3,  // P03  lT4
  HVOn_5 = 4,  // P04  lT5
  HVOn_6 = 5,  // P05  lT6
  HVOn_7 = 6,  // P06  lT7
  HVOn_8 = 7,  // P07  lT8
};

class HVControl
{
private:
  I2C bus;
  const uint8_t PCA9534PW_address = 0x21;  // Depending on Address pins
  uint8_t PCA9534PW_read;
  uint8_t PCA9534PW_write;
  const uint8_t PCA9534PW_input_port = 0x00;          // Command byte for input port
  const uint8_t PCA9534PW_output_port = 0x01;         // Command byte for output
  const uint8_t PCA9534PW_polarity_inversion = 0x02;  // Command byte for polarity inversion
  const uint8_t PCA9534PW_configuration = 0x03;       // Command byte for configuration
  uint8_t HVControlPins;

  HVOnPinNames onPins[8] = { HVOn_1, HVOn_2, HVOn_3, HVOn_4, HVOn_5, HVOn_6, HVOn_7, HVOn_8 };

  void setBit(uint8_t index);
  void clearBit(uint8_t index);
  bool getBit(uint8_t word, uint8_t index);

  void write();
  uint8_t read();

public:
  HVControl(PinName SDA_PIN, PinName SCL_PIN);

  bool readHV(HVOnPinNames pin);
  void turnOnHV(HVOnPinNames pin);
  void turnOffHV(HVOnPinNames pin);

  void turnOnAllHV();
  void turnOffAllHV();

  uint8_t readAllOn();
  void setAllHV(uint8_t code);
  void setAllHVStagedStartup(uint8_t code);
};

#endif  // HVCONTROL_H
