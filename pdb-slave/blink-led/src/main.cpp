#include <mbed.h>
#include <LPC1768_pindefs.h>
#include "StateMachine.h"

Serial pc(USBTX, USBRX, 9600);

DigitalOut buttonLed(LPC_BUTTON_LED);
DigitalOut mbedLed1(LPC_LED1);
DigitalIn pin(p6, PullDown);
DigitalOut keepPDBOn(LPC_KEEP_PDB_ON, PullDown);

StateMachine stateMachine;
Timer printTimer;

int main(){
    pc.printf("MBED gets power now!\r\n");

    buttonLed = false;
    mbedLed1 = false;
    keepPDBOn = false;
    printTimer.start();

    while(1){
        // Get state of 'button'
        bool buttonstate = pin.read();

        // Update system state
        stateMachine.updateState(buttonstate);

        // Debug prints
        if(printTimer.read_ms() > 1000){
            pc.printf("State: %s\t\tKeepPDBOn: %d\r\n", stateMachine.getState().c_str(), stateMachine.getKeepPDBOn());
            printTimer.reset();
        }

        // Take action: set LED, set KeepPDBOn, etc.
        buttonLed = stateMachine.getOnOffButtonLedState();
        mbedLed1 = stateMachine.getOnOffButtonLedState();
        keepPDBOn = stateMachine.getKeepPDBOn();

    }
    
}