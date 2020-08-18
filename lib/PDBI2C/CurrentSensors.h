#ifndef CURRENTSENSORS_H
#define CURRENTSENSORS_H

#include <mbed.h>

// See the AD1015 datasheet for more information

class CurrentSensors
{
private:
  I2C bus;
  const uint8_t ADC128D818_address = 0x1D;  // Depending on Address pin
  uint8_t ADC128D818_read;
  uint8_t ADC128D818_write;

  enum ADC128D818RegisterAddresses
  {
    configurationRegister = 0x00,
    interruptStatusRegister = 0x01,
    interruptMaskRegister = 0x03,
    conversionRateRegister = 0x07,
    channelDisableRegister = 0x08,
    oneShotRegister = 0x09,
    deepShutdownRegister = 0x0A,
    advancedConfigurationRegister = 0x0B,
    busyChannelRegisters = 0x0C,

    channelReadingsRegisterIN0 = 0x20,
    channelReadingsRegisterIN1 = 0x21,
    channelReadingsRegisterIN2 = 0x22,
    channelReadingsRegisterIN3 = 0x23,
    channelReadingsRegisterIN4 = 0x24,
    channelReadingsRegisterIN5 = 0x25,
    channelReadingsRegisterIN6 = 0x26,
    channelReadingsRegisterIN7 = 0x27,

    limitRegisterIN0High = 0x2A,
    limitRegisterIN0Low = 0x2B,
    limitRegisterIN1High = 0x2C,
    limitRegisterIN1Low = 0x2D,
    limitRegisterIN2High = 0x2E,
    limitRegisterIN2Low = 0x2F,
    limitRegisterIN3High = 0x30,
    limitRegisterIN3Low = 0x30,
    limitRegisterIN4High = 0x30,
    limitRegisterIN4Low = 0x30,
    limitRegisterIN5High = 0x30,
    limitRegisterIN5Low = 0x30,
    limitRegisterIN6High = 0x30,
    limitRegisterIN6Low = 0x30,
    limitRegisterIN7High = 0x30,
    limitRegisterIN7Low = 0x30,

    manufacturerIDRegister = 0x3E,
    revisionIDRegister = 0x3F,
  };

  const enum ADC128D818ConfigurationSettings
  {
    advancedConfigurationSetting = 0x03,
    conversionRateSetting = 0x01,
    limitSettingLow = 0x00,
    limitSettingHigh = 0xFF,
    configurationSetting = 0x03,
  };

  const float ACS780SensorSensitivity = 40;  // mV per Ampere
  const float ACS780SensorOffset = 0;        // mV

  uint16_t swapBytes(uint16_t data);

  uint16_t readRegister();
  bool writeRegisterAddress(ADC128D818RegisterAddresses registerAddres);

  template <typename T>
  bool writeRegister(ADC128D818RegisterAddresses registerAddres, T);

public:
  CurrentSensors(PinName SDA_PIN, PinName SCL_PIN);

  float readPDBCurrent();
  float readLV1Current();
  float readLV2Current();
  float readHVCurrent();
};

#endif  // CURRENTSENSORS_H