#include <mbed.h>
#include "objectlist.h"
#include "Ethercat.h"
#include "DBS_pindefs.h"

void main_init();
void main_loop();
void finish_loop();

// int set_bit(int reg, int bitNumber, bool value);
// void operate_status_led();
// void toggle_status_led();

DigitalOut statusLed(DBS_LED,0);
Serial pc(DBS_UART_USB_TX, DBS_UART_USB_RX, 128000);
Ethercat ecat(DBS_ECAT_MOSI, DBS_ECAT_MISO, DBS_ECAT_SCK, DBS_ECAT_NCS, &pc, 10);

Timer cycleTimer;
Timer totalTimer;

// Ticker ledToggleLong;
// Ticker ledToggleShort;

const int ethercatFrequency = 1000;
// const float statusPeriod = 10;
// int statusNumber;

int main() {
  wait(5);
  pc.printf("\f\r\nTime compiled = %s.\r\nDate = %s.\r\nBlink LED GES.", __TIME__, __DATE__);
  wait(5);
  main_init();
  while(1) {
    main_loop();
  }
}

void main_init(){
  //Starts both timers at 0.
  totalTimer.reset();
  totalTimer.start();
  cycleTimer.start();
  cycleTimer.reset();
  // ledToggleLong.attach(&operate_status_led, statusPeriod/2);
  return;
}

void main_loop(){
  ecat.update();

  if(mosi_LED_command == 1){
    statusLed = 1;
  }
  miso_LED_ack = mosi_LED_command;

  finish_loop();
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
    cycleCounter = 0;
    tooSlowCounter = 0;
    totalTimer.reset();
  }
}