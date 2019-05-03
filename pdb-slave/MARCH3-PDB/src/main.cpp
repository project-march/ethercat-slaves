#include <mbed.h>
#include "LPC1768_pindefs_M3.h"
#include "HVControl.h"
#include "HVOCTriggers.h"
#include "CurrentSensors.h"
#include "utypes.h"
#include "Ethercat.h"
#include "StateMachine.h"

Serial pc(USBTX, USBRX, 9600); // Serial communication for debugging

// On/off-related inputs/outputs
DigitalOut buttonLed(LPC_BUTTON_LED, true); // Behaviour is logically inverted
DigitalOut mbedLed1(LPC_LED1, false); // Shows same as button led
DigitalIn button(LPC_BUTTON_PRESSED, PullDown);
DigitalOut keepPDBOn(LPC_KEEP_PDB_ON, PullDown);
DigitalOut mbedLed4(LPC_LED4, false); // Shows if in Shutdown state

// Low voltage related inputs/outputs
DigitalOut LVOn(LPC_LVON, PullDown);
DigitalIn LVOkay(LPC_LVOKAY, PullDown);

// Master communication related inputs/outputs
DigitalOut mbedLed2(LPC_LED2, false); // Shows if in MasterOk state

// High voltage related inputs/outputs
DigitalOut mbedLed3(LPC_LED3, false); // Shows if any HV is on
HVControl hvControl(LPC_I2C_SDA, LPC_I2C_SCL);
HVOCTriggers hvOCTriggers(LPC_I2C_SDA, LPC_I2C_SCL);

// Current sensing related inputs/outputs
CurrentSensors currentSensors(LPC_I2C_SDA, LPC_I2C_SCL);

// EtherCAT
// Set PDO sizes
const int PDORX_size = 32;
const int PDOTX_size = 32;
// EtherCAT object
Ethercat ecat(LPC_ECAT_MOSI, LPC_ECAT_MISO, LPC_ECAT_SCK, LPC_ECAT_SCS, PDORX_size, PDOTX_size);
// Easy access to PDOs
#define miso            Ethercat::pdoTx.Struct.miso
#define mosi            Ethercat::pdoRx.Struct.mosi

StateMachine stateMachine; // State machine instance
Timer printTimer; // Timer to print debug statements only once per second

int main(){
    pc.printf("\r\nMBED gets power now!");

    // Set initial DigitalOut outputs
    buttonLed = true; // Behaviour is logically inverted
    mbedLed1 = false;
    mbedLed2 = false;
    mbedLed3 = false;
    mbedLed4 = false;
    keepPDBOn = false;
    LVOn = false;

    // Set initial EtherCAT outputs
    miso.masterShutdown = 0;

    printTimer.start(); // Start print timer right before entering infinite loop

    while(1){
        // Update EtherCAT variables
        ecat.update();
        
        // Get inputs from digitalIns, I2C bus and EtherCAT
        bool buttonstate = button.read();
        bool LVOkayState = LVOkay.read();
        uint8_t hvOCTriggerStates = hvOCTriggers.readOCTriggers();
        float PDBCurrent = currentSensors.readPDBCurrent();
        bool masterOkState = mosi.masterOk;
        bool masterShutdownAllowedState = mosi.masterShutdownAllowed;

        // Update system state
        stateMachine.updateState(buttonstate, masterOkState, masterShutdownAllowedState);

        // Debug prints
        if(printTimer.read_ms() > 1000){
            pc.printf("\r\nState: %s", stateMachine.getState().c_str());
            // pc.printf("\tKeepPDBOn: %d", stateMachine.getKeepPDBOn());
            // if((!LVOkayState) && (stateMachine.getState() == "LVOn_s")){
            //     pc.printf("LV not okay");
            // }
            pc.printf("\r\n HV reset: %x, HV on: %x", hvControl.readAllReset(), hvControl.readAllOn());
            pc.printf("\r\n HV OC trigger: %x", hvOCTriggerStates);
            pc.printf("\r\n PDB current: %f", PDBCurrent);
            printTimer.reset();
        }

        // Set LEDs and digitalOuts
        buttonLed = !stateMachine.getOnOffButtonLedState(); // Behaviour is logically inverted
        mbedLed1 = stateMachine.getOnOffButtonLedState(); // Later change to getKeepPDBOn()?
        mbedLed2 = (stateMachine.getState() == "MasterOk_s"); // LED on if in MasterOk state
        mbedLed3 = (hvControl.readAllOn() != 0); // LED on if any HV is on
        mbedLed4 = (stateMachine.getState() == "Shutdown_s"); // LED on if in Shutdown state
        keepPDBOn = stateMachine.getKeepPDBOn();
        LVOn = stateMachine.getLVOn();
        miso.masterShutdown = stateMachine.getMasterShutdown();

        // Control HV
        if(stateMachine.getState() == "MasterOk_s" || stateMachine.getState() == "ShutdownInit_s"){
            // In an allowed state to have HV on
            hvControl.setAllHV(0xFF); // Todo: make this an EtherCAT variable
            // hvControl.turnOnAllHV();
        }
        else{
            // Not in an allowed state to have any HV on
            hvControl.turnOffAllHV();
        }
        
        // Set logging stuff in EtherCAT buffers
        // miso.LVState = 0;
        // miso.LVState |= stateMachine.getLVOn();
        // miso.LVState |= (LVOkayState << 1);

    }
    
}