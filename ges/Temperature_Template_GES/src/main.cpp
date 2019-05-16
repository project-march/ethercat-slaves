#include <mbed.h>
// Include pin definitions for the DieBieSlave
#include "DBS_pindefs.h"
// Include slave-specific utypes.h before Ethercat.h
#include "utypes.h"
// Include the EtherCAT library
#include "Ethercat.h"
// Include Temperature sensor library
#include "Temperature.h"

#define WAIT_TIME (2) // seconds
#define APP_TITLE "Temperature Template GES" // Application name to be printed to terminal
#define PC_BAUDRATE (9600) // per second

// Easy access to PDOs. Needs to be changed if different PDOs are used
#define miso            Ethercat::pdoTx.Struct.miso
#define mosi            Ethercat::pdoRx.Struct.mosi

// Set PDO sizes
const int PDORX_size = 32;
const int PDOTX_size = 32;

union bit32 {
    float    f;
    int32_t  i;
};

// LED for showing status
DigitalOut statusLed(DBS_LED); // DieBieSlave
// DigitalOut statusLed(LED1); // Nucleo F303RE and LPC1768

// Serial communication with the pc for debugging
Serial pc(DBS_UART_USB_TX, DBS_UART_USB_RX, PC_BAUDRATE); // DieBieSlave
// Serial pc(USBTX, USBRX, PC_BAUDRATE); // Nucleo F303RE and LPC1768

// Ethercat communication with master
Ethercat ecat(DBS_ECAT_MOSI, DBS_ECAT_MISO, DBS_ECAT_SCK, DBS_ECAT_NCS, PDORX_size, PDOTX_size);

// Temperature sensor
Temperature tempSensor(DBS_P04, &pc, 10); // DieBieSlave
// Temperature tempSensor(PB_7, &pc, 2); // Nucleo F303RE
// Temperature tempSensor(p6, &pc, 2); // LPC1768

int main() {
  wait(WAIT_TIME);
  // Print application title and compile information
  pc.printf("\f\r\n%s\r\n------------------\r\nTime compiled = %s.\r\nDate = %s.", APP_TITLE, __TIME__, __DATE__);
  statusLed = true;
  wait(WAIT_TIME);
  statusLed = false;

  // Set all initial misos
  miso.temperature = 0;

  while(1) {
    // Update the EtherCAT buffer
    ecat.update();

    // Get temperature data
    bit32 temperature;
    temperature.f = tempSensor.read(&pc, 10);

    // Set status LED if temperature data invalid
    statusLed = (temperature.f < -998);

    // Set all misos to be sent back to the master
    miso.temperature = (int32_t) temperature.f; // if no conversion on the master side takes place
    // miso.temperature = temperature.i; // if conversion on the master size takes place

    wait_ms(500); // Optional
  }
}