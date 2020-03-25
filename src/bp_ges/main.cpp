#include <mbed.h>
// Include pin definitions for the DieBieSlave
#include "DBS_pindefs.h"
// Include slave-specific utypes.h before Ethercat.h
#include "utypes.h"
// Include the EtherCAT library
#include "Ethercat.h"
// Include Temperature sensor library
#include "Temperature.h"
// Include Thermistor library
#include "Thermistor.h"

#define WAIT_TIME (2000000)               // micro-seconds
#define APP_TITLE "MARCH Backpack GES"    // Application name to be printed to terminal
#define PC_BAUDRATE (9600)                // per second

// Easy access to PDOs. Needs to be changed if different PDOs are used
#define miso Ethercat::pdoTx.Struct.miso
#define mosi Ethercat::pdoRx.Struct.mosi

// Set PDO sizes
const int PDORX_size = 32;
const int PDOTX_size = 32;

union bit32
{
  float f;
  int32_t i;
};

// LED for showing status
DigitalOut statusLed(DBS_LED);  // DieBieSlave

// Serial communication with the pc for debugging
Serial pc(DBS_UART_USB_TX, DBS_UART_USB_RX, PC_BAUDRATE);

// Ethercat communication with master
Ethercat ecat(DBS_ECAT_MOSI, DBS_ECAT_MISO, DBS_ECAT_SCK, DBS_ECAT_NCS, PDORX_size, PDOTX_size);

// Temperature sensor
Temperature temperatureSensorLHAA(DBS_P04);
Temperature temperatureSensorRHAA(DBS_P23);

// PTC Thermistor
Thermistor ptcThermistorLHAA(DBS_P05);
Thermistor ptcThermistorRHAA(DBS_P22);

int main()
{
  wait_us(WAIT_TIME);
  // Print application title and compile information
  pc.printf("\f\r\n%s\r\n------------------\r\nTime compiled = %s.\r\nDate = %s.", APP_TITLE, __TIME__, __DATE__);
  statusLed = true;
  wait_us(WAIT_TIME);
  statusLed = false;

  // Set all initial misos
  miso.TemperatureLHAA = 0;
  miso.TemperatureRHAA = 0;
  miso.OverTemperatureTriggerBP = 0;

  while (1)
  {
    // Update the EtherCAT buffer
    ecat.update();

    // Get temperature data
    bit32 temperatureLHAA, temperatureRHAA;
    uint8_t thermistorOverTemperatureLHAA, thermistorOverTemperatureRHAA;

    temperatureLHAA.f = temperatureSensorLHAA.read();
    temperatureRHAA.f = temperatureSensorRHAA.read();
    thermistorOverTemperatureLHAA = ptcThermistorLHAA.read();
    thermistorOverTemperatureRHAA = ptcThermistorRHAA.read();

    // Set status LED if any temperature data invalid
    statusLed = (temperatureLHAA.f < 0) || (temperatureRHAA.f < 0);

    // Set all misos to be sent back to the master
    miso.TemperatureLHAA = temperatureLHAA.i;
    miso.TemperatureRHAA = temperatureRHAA.i;

    miso.overTemperatureTriggerBP = thermistorOverTemperatureLHAA | (thermistorOverTemperatureRHAA << 1);
}