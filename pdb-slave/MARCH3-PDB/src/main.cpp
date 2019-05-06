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
union bit32 {
    float    f;
    uint32_t ui;
};

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
    miso.emergencyButtonState = 0; // Not supported on M3 PDB
    miso.HVOCTriggers = hvOCTriggers.readOCTriggers();
    miso.LVStates = LVOkay.read();
    miso.HVStates = hvControl.readAllOn();
    miso.PDBCurrent = currentSensors.readPDBCurrent();
    miso.LV1Current = currentSensors.readLV1Current();
    miso.LV2Current = currentSensors.readLV2Current();
    miso.HVCurrent = currentSensors.readHVCurrent();

    bit32 PDBCurrent, LV1Current, LV2Current, HVCurrent;

    printTimer.start(); // Start print timer right before entering infinite loop

    while(1){
        // Update EtherCAT variables
        ecat.update();
        
        // Get inputs from digitalIns and I2C bus
        bool buttonstate = button.read();
        bool LVOkayState = LVOkay.read();
        uint8_t hvOCTriggerStates = hvOCTriggers.readOCTriggers();
        PDBCurrent.f = currentSensors.readPDBCurrent();
        LV1Current.f = currentSensors.readLV1Current();
        LV2Current.f = currentSensors.readLV2Current();
        HVCurrent.f = currentSensors.readHVCurrent();
        uint8_t hvOnStates = hvControl.readAllOn();
        uint8_t hvResetStates = hvControl.readAllReset();

        // Update system state
        stateMachine.updateState(buttonstate, (bool) mosi.masterOk, (bool) mosi.masterShutdownAllowed);

        // Debug prints
        if(printTimer.read_ms() > 1000){
            pc.printf("\r\nState: %s", stateMachine.getState().c_str());
            // pc.printf("\tKeepPDBOn: %d", stateMachine.getKeepPDBOn());
            // if((!LVOkayState) && (stateMachine.getState() == "LVOn_s")){
            //     pc.printf("LV not okay");
            // }
            pc.printf("\r\n HV reset: %x, HV on: %x", hvResetStates, hvOnStates);
            pc.printf("\r\n HV OC trigger: %x", hvOCTriggerStates);
            pc.printf("\r\n PDB current: %f", PDBCurrent.f);
            pc.printf("\r\n LV current: %f", LV1Current.f);
            printTimer.reset();
        }

        // Set LEDs and digitalOuts
        buttonLed = !stateMachine.getOnOffButtonLedState(); // Behaviour is logically inverted
        mbedLed1 = stateMachine.getOnOffButtonLedState(); // LED on if button LED is on
        mbedLed2 = (stateMachine.getState() == "MasterOk_s"); // LED on if in MasterOk state
        mbedLed3 = (hvControl.readAllOn() != 0); // LED on if any HV is on
        mbedLed4 = (stateMachine.getState() == "Shutdown_s"); // LED on if in Shutdown state
        keepPDBOn = stateMachine.getKeepPDBOn();
        LVOn = (stateMachine.getLVOn() && (mosi.LVControl & 1)); // If both the statemachine and master say LV should be on

        // Control HV
        if(stateMachine.getState() == "MasterOk_s" || stateMachine.getState() == "ShutdownInit_s"){
            // In an allowed state to have HV on
            hvControl.setAllHV(mosi.HVControl);
            // hvControl.turnOnAllHV();
            miso.emergencyButtonState = mosi.emergencyButtonControl; // Not supported on M3 PDB
        }
        else{
            // Not in an allowed state to have any HV on
            hvControl.turnOffAllHV();
            // Turn on emergency button control
            miso.emergencyButtonState = 1; // Not supported on M3 PDB
        }
        
        // Set miso's in EtherCAT buffers
        miso.masterShutdown = stateMachine.getMasterShutdown();
        miso.HVOCTriggers = hvOCTriggerStates;
        miso.LVStates = LVOkayState;
        miso.HVStates = hvOnStates;
        miso.PDBCurrent = PDBCurrent.ui;
        miso.LV1Current = LV1Current.ui;
        miso.LV2Current = LV2Current.ui;
        miso.HVCurrent = HVCurrent.ui;

    }
    
}