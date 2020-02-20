#ifndef HVCONTROL_H
#define HVCONTROL_H

#include <mbed.h>

enum HVOnPinNames
{
  HVOn_1 = 1,   // P01  lT2
  HVOn_2 = 3,   // P03  lT4
  HVOn_3 = 5,   // P05  lT6
  HVOn_4 = 7,   // P07  lT8
  HVOn_5 = 15,  // P17  lT9
  HVOn_6 = 13,  // P15  lT11
  HVOn_7 = 11,  // P13  lT13
  HVOn_8 = 9,   // P11  lT15
};

enum HVResetPinNames
{
  HVReset_1 = 0,   // P00  lT1
  HVReset_2 = 2,   // P02  lT3
  HVReset_3 = 4,   // P04  lT5
  HVReset_4 = 6,   // P06  lT7
  HVReset_5 = 14,  // P16  lT10
  HVReset_6 = 12,  // P14  lT12
  HVReset_7 = 10,  // P12  lT14
  HVReset_8 = 8,   // P10  lT16
};

class HVControl
{
private:
  I2C bus;
  const uint8_t PCA8575D_address = 0x20;  // Depending on Address pins
  uint8_t PCA8575D_read;
  uint8_t PCA8575D_write;
  uint16_t HVControlPins;

  HVResetPinNames resetPins[8] = { HVReset_1, HVReset_2, HVReset_3, HVReset_4,
                                   HVReset_5, HVReset_6, HVReset_7, HVReset_8 };
  HVOnPinNames onPins[8] = { HVOn_1, HVOn_2, HVOn_3, HVOn_4, HVOn_5, HVOn_6, HVOn_7, HVOn_8 };

  void setBit(uint8_t index);
  void clearBit(uint8_t index);
  bool getBit(uint16_t word, uint8_t index);

  void write();
  uint16_t read();

public:
  HVControl(PinName SDA_PIN, PinName SCL_PIN);

  bool readHV(HVOnPinNames pin);
  void turnOnHV(HVOnPinNames pin);
  void turnOffHV(HVOnPinNames pin);
  void resetHV(HVResetPinNames pin);

  void turnOnAllHV();
  void turnOffAllHV();
  void resetAllHV();

  uint8_t readAllReset();
  uint8_t readAllOn();
  void setAllHV(uint8_t code);
  void setAllHVStagedStartup(uint8_t code);
};

#endif  // HVCONTROL_H
