#include <mbed.h>
#include <LPC1768_pindefs.h>
#include "StateMachine.h"

Serial pc(USBTX, USBRX, 9600);

DigitalOut buttonLed(LPC_BUTTON_LED);
DigitalOut mbedLed1(LPC_LED1); // Shows same as button led
DigitalIn button(LPC_BUTTON_PRESSED, PullDown);
DigitalOut keepPDBOn(LPC_KEEP_PDB_ON, PullDown);

DigitalOut LVOn1(LPC_LVON1, PullDown);
DigitalOut LVOn2(LPC_LVON2, PullDown);
DigitalIn LVOkay1(LPC_LVOKAY1, PullDown);
DigitalIn LVOkay2(LPC_LVOKAY2, PullDown);

DigitalOut mbedLed2(LPC_LED2); // Shows if in MasterOk state
// Temporary logical signals to simulate master signals
DigitalIn masterOk(p14, PullDown);
DigitalOut masterShutdown(p15, PullDown);
DigitalIn masterShutdownAllowed(p16, PullDown);

DigitalOut mbedLed3(LPC_LED3); // Shows if any HV is on

DigitalOut mbedLed4(LPC_LED4); // Shows if in Shutdown state



StateMachine stateMachine;
Timer printTimer;

int main(){
    pc.printf("MBED gets power now!\r\n");

    // Set initial outputs
    buttonLed = false;
    mbedLed1 = false;
    mbedLed2 = false;
    mbedLed3 = false;
    mbedLed4 = false;
    keepPDBOn = false;
    LVOn1 = false;
    LVOn2 = false;
    masterShutdown = false;

    printTimer.start();

    while(1){
        // Get state of 'button'
        bool buttonstate = button.read();
        bool LVOkay1State = LVOkay1.read();
        bool LVOkay2State = LVOkay2.read();
        bool masterOkState = masterOk.read();
        bool masterShutdownAllowedState = masterShutdownAllowed.read();

        // Update EtherCAT

        // Update system state
        stateMachine.updateState(buttonstate, masterOkState, masterShutdownAllowedState);

        // Debug prints
        if(printTimer.read_ms() > 1000){
            pc.printf("\r\nState: %s", stateMachine.getState().c_str());
            // pc.printf("\tKeepPDBOn: %d", stateMachine.getKeepPDBOn());
            // if((!LVOkay1State) && (stateMachine.getState() == "LVOn_s")){
            //     pc.printf("LV1 not okay!!!\r\n");
            // }
            // pc.printf("\tPin 16: %d", masterShutdownAllowedState);
            printTimer.reset();
        }

        // Take action: set LED, set KeepPDBOn, etc.
        buttonLed = stateMachine.getOnOffButtonLedState();
        mbedLed1 = stateMachine.getOnOffButtonLedState();
        mbedLed2 = (stateMachine.getState() == "MasterOk_s");
        // Todo: make mbedLed3 true if any HV on
        mbedLed4 = (stateMachine.getState() == "Shutdown_s");
        keepPDBOn = stateMachine.getKeepPDBOn();
        LVOn1 = stateMachine.getLVOn1();
        LVOn2 = stateMachine.getLVOn2();
        masterShutdown = stateMachine.getMasterShutdown();
        

        // Set logging stuff in EtherCAT buffers

    }
    
}