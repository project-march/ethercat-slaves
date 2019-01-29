#include <mbed.h>

// Include pin definitions for the DieBieSlave
#include "DBS_pindefs.h"

// Include slave-specific utypes.h before Ethercat.h
#include "utypes.h"

// Include the EtherCAT library
#include "Ethercat.h"

#define WAIT_TIME (2) // seconds
#define APP_TITLE "Capacity Test GES" // Application name to be printed to terminal
#define PC_BAUDRATE (128000) // per second

// Easy access to PDOs. Needs to be changed if different PDOs are used
#define miso            Ethercat::pdoTx.Struct.miso
#define mosi            Ethercat::pdoRx.Struct.mosi

// Set PDO sizes
const int PDORX_size = 40;    // Because of 32bit mosi array with 10 elements = 40 bytes
const int PDOTX_size = 40;    // Because of 32bit miso array with 10 elements = 40 bytes

// LED on DieBieSlave, for testing communication
DigitalOut statusLed(DBS_LED);

// Serial communication with the pc for debugging
Serial pc(DBS_UART_USB_TX, DBS_UART_USB_RX, PC_BAUDRATE);

// Ethercat communication with master
Ethercat ecat(DBS_ECAT_MOSI, DBS_ECAT_MISO, DBS_ECAT_SCK, DBS_ECAT_NCS, PDORX_size, PDOTX_size);

int main() {
  wait(WAIT_TIME);
  // Print application title and compile information
  pc.printf("\f\r\n%s\r\n------------------\r\nTime compiled = %s.\r\nDate = %s.", APP_TITLE, __TIME__, __DATE__);
  statusLed = 1;
  wait(WAIT_TIME);
  statusLed = 0;

  // Set all initial misos
  for(int i = 0; i < (sizeof(miso)/sizeof(*miso)); i++)
  {
    miso[i] = i;
  }


  while(1) {
    // Update the EtherCAT buffer
    ecat.update();

    // Set led if all mosi messages are not zero
    statusLed = (mosi[0] && mosi[1] && mosi[2]);

    // Set all misos to be sent back to the master
    for(int i = 0; i < (sizeof(miso)/sizeof(*miso)); i++)
    {
      miso[i] = mosi[i];
    }
  }
}