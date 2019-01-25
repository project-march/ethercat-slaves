#include <mbed.h>
#include "DBS_pindefs.h"
// Include slave-specific utypes.h before Ethercat.h
#include "utypes.h"
#include "Ethercat.h"

#define WAIT_TIME (2) // seconds
#define APP_TITLE "Capacity Test GES" // Application name to be printed to terminal
#define PC_BAUDRATE (128000) // per second

// Easy access to PDOs. Needs to be changed if different PDOs are used
// #define miso            Ethercat::pdoTx.Struct.miso
// #define mosi            Ethercat::pdoRx.Struct.mosi

// LED on DieBieSlave, for testing communication
DigitalOut statusLed(DBS_LED);

// Serial communication with the pc for debugging
Serial pc(DBS_UART_USB_TX, DBS_UART_USB_RX, PC_BAUDRATE);

// Ethercat communication with master
Ethercat ecat(DBS_ECAT_MOSI, DBS_ECAT_MISO, DBS_ECAT_SCK, DBS_ECAT_NCS);

int main() {
  wait(WAIT_TIME);
  // Print application title and compile information
  pc.printf("\f\r\n%s\r\n------------------\r\nTime compiled = %s.\r\nDate = %s.", APP_TITLE, __TIME__, __DATE__);
  statusLed = 1;
  wait(WAIT_TIME);
  statusLed = 0;
  // Set all initial misos
  // miso.a = 1;
  // miso.b = 2;
  //etc.

  while(1) {
    // Update the EtherCAT buffer
    ecat.update();

    // Set led if all mosi messages arrive correctly
    // statusLed = (mosi.A && mosi.B); //etc.

    // Set all misos to be sent back to the master
    // miso.a = mosi.A;
    // miso.b = mosi.B;
    //etc.
  }
}