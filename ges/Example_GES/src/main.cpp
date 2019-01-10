#include <mbed.h>
#include "DBS_pindefs.h"
#include "objectlist.h"
#include "Ethercat.h"

void finish_loop();

// int set_bit(int reg, int bitNumber, bool value);
// void operate_status_led();
// void toggle_status_led();

// LED on DieBieSlave, for testing communication
DigitalOut statusLed(DBS_LED);
// Serial communication with the pc for debugging
Serial pc(DBS_UART_USB_TX, DBS_UART_USB_RX, 128000);

// Ethercat communication with master
Ethercat ecat(DBS_ECAT_MOSI, DBS_ECAT_MISO, DBS_ECAT_SCK, DBS_ECAT_NCS, &pc, 1);

//Timers for debugging
Timer cycleTimer;
Timer totalTimer;

// Ticker ledToggleLong;
// Ticker ledToggleShort;

// EtherCAT frequency
const int ethercatFrequency = 1;
// const float statusPeriod = 10;
// int statusNumber;

int main() {
  // wait(5);
  // pc.printf("\f\r\nTime compiled = %s.\r\nDate = %s.\r\nBlink LED GES.", __TIME__, __DATE__);
  // statusLed = 1;
  // wait(5);
  totalTimer.reset();
  totalTimer.start();
  cycleTimer.reset();
  cycleTimer.start();
  miso_LED_ack = 0;
  statusLed = 0;
  while(1) {
    // Update the EtherCAT buffer
    ecat.update(&pc, 2);

    // Set led if ordered to from EtherCAT master
    if(mosi_LED_command == 1){
      statusLed = 1;
    }
    else{
      statusLed = 0;
    }

    // Set testdata to be sent back to the master
    miso_LED_ack = miso_LED_ack + 1;

    // finish_loop();
  }
}

void finish_loop(){
  static int cycleCounter = 0;
  //EtherCAT cycle period in us.
  static const float ethercatPeriod =1.0/((float) ethercatFrequency);

  //Check whether the cycle took longer than the specified EtherCAT cycle time
  //  increments tooSlowCounter if it did.
  //  Waits if it didn't.
  static int tooSlowCounter = 0;
  if(cycleTimer.read() < ethercatPeriod)
  {
    while(cycleTimer.read() < ethercatPeriod)
    {
      //Wait for the etherCAT cycle
    }
  }
  else
  {
    tooSlowCounter++;
  }

  //Starts the Timer over. This marks the end of the official cycle.
  cycleTimer.reset();
  cycleCounter++;

  //Once every 5 seconds, dump some information over the serial
  if(totalTimer.read() > 5)
  {
    float totalTime = totalTimer.read();
    pc.printf("\r\n\n\n%i cycles performed. Took %f seconds for an average "
    "of %f per cycle. Was too slow %i times.",
    cycleCounter, totalTime, totalTime /( (float) cycleCounter), tooSlowCounter);
    // pc.printf("\nled command: %i",mosi_LED_command);
    cycleCounter = 0;
    tooSlowCounter = 0;
    totalTimer.reset();
  }
}