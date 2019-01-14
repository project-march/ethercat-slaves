#include <mbed.h>
#include "DBS_pindefs.h"
#include "utypes.h" // include utypes.h before Ethercat.h
#include "Ethercat.h"

#define WAIT_TIME (2) // seconds
#define APP_TITLE "Example GES"

#define PC_BAUDRATE (128000) // baudrate of serial pc communication for debugging

// Define EtherCAT inputs/outputs here for easy access
#define miso_LED_ack              Ethercat::pdoTx.Struct.miso_LED_ack
#define mosi_LED_command          Ethercat::pdoRx.Struct.mosi_LED_command

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

  miso_LED_ack = 0;
  while(1) {
    // Update the EtherCAT buffer
    ecat.update();

    // Set LED if ordered to from EtherCAT master
    statusLed = (mosi_LED_command == 1);

    // Set acknowledge to be sent back to the master
    miso_LED_ack = mosi_LED_command;

  }
}