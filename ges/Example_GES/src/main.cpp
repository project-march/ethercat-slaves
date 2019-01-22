#include <mbed.h>
#include "DBS_pindefs.h"
#include "utypes.h"
#include "Ethercat.h"

#define WAIT_TIME (2) // seconds
#define APP_TITLE "Example GES" // Application name to be printed to terminal
#define PC_BAUDRATE (128000) // per second

// Easy access to PDOs. Needs to be changed if different PDOs are used
#define miso            Ethercat::pdoTx.Struct.miso
#define mosi            Ethercat::pdoRx.Struct.mosi

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
  miso.LED_ack = 0;

  while(1) {
    // Update the EtherCAT buffer
    ecat.update();

    // Set led if ordered to from EtherCAT master
    statusLed = (mosi.LED_command == 1);

    // Set acknowledge to be sent back to the master
    miso.LED_ack = mosi.LED_command;

  }
}