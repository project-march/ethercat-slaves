#include <mbed.h>
#include "DBS_pindefs.h"
#include "objectlist.h"
#include "Ethercat.h"

// LED on DieBieSlave, for testing communication
DigitalOut statusLed(DBS_LED);
// Serial communication with the pc for debugging
Serial pc(DBS_UART_USB_TX, DBS_UART_USB_RX, 128000);
// Ethercat communication with master
Ethercat ecat(DBS_ECAT_MOSI, DBS_ECAT_MISO, DBS_ECAT_SCK, DBS_ECAT_NCS);

int main() {
  wait(5);
  pc.printf("\f\r\nTime compiled = %s.\r\nDate = %s.\r\nBlink LED GES.", __TIME__, __DATE__);
  statusLed = 1;
  wait(5);
  statusLed = 0;

  miso_LED_ack = 0;
  while(1) {
    // Update the EtherCAT buffer
    ecat.update();

    // Set led if ordered to from EtherCAT master
    if(mosi_LED_command == 1){
      statusLed = 1;
    }
    else{
      statusLed = 0;
    }

    // Set testdata to be sent back to the master
    miso_LED_ack = miso_LED_ack + 1;

  }
}