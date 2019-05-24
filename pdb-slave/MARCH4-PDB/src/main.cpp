#include <mbed.h>
#include "LPC1768_pindefs_M4.h"
#include "HVControl.h"
#include "HVOCTriggers.h"
#include "CurrentSensors.h"
#include "utypes.h"
#include "Ethercat.h"
#include "StateMachine.h"

Serial pc(USBTX, USBRX, 9600); // Serial communication for debugging

// On/off-related inputs/outputs
DigitalOut buttonLed(LPC_BUTTON_LED, false); // True means LED on
DigitalOut mbedLed1(LPC_LED1, false); // Shows same as button led
DigitalIn button(LPC_BUTTON_PRESSED, PullDown); // True means button pressed
DigitalOut keepPDBOn(LPC_KEEP_PDB_ON, false); // True means keep PDB on
DigitalOut mbedLed4(LPC_LED4, false); // Shows if in Shutdown state

// Low voltage related inputs/outputs
DigitalOut LVOn1(LPC_LVON1, false); // True means on
DigitalIn LVOkay1(LPC_LVOKAY1, PullDown); // True means okay
DigitalOut LVOn2(LPC_LVON2, false); // True means on
DigitalIn LVOkay2(LPC_LVOKAY2, PullDown); // True means okay

// Master communication related inputs/outputs
DigitalOut mbedLed2(LPC_LED2, false); // Shows if in MasterOk state

// High voltage related inputs/outputs
DigitalOut mbedLed3(LPC_LED3, false); // Shows if any HV is on
HVControl hvControl(LPC_I2C_SDA, LPC_I2C_SCL);
HVOCTriggers hvOCTriggers(LPC_I2C_SDA, LPC_I2C_SCL);

// Emergency button related inputs/outputs
DigitalIn emergencyButton(LPC_EMERGENCY_SWITCH_STATUS, PullDown); // False means disconnected HV
DigitalOut emergencyButtonControl(LPC_EMERGENCY_SWITCH, false); // False means disconnect HV

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
DigitalOut ecatReset(LPC_ECAT_RST, true);
DigitalOut ecatIRQ(LPC_ECAT_IRQ, false);
Ethercat ecat(LPC_ECAT_MOSI, LPC_ECAT_MISO, LPC_ECAT_SCK, LPC_ECAT_SCS, PDORX_size, PDOTX_size, &pc, 10);

// Easy access to PDOs
#define miso            Ethercat::pdoTx.Struct.miso
#define mosi            Ethercat::pdoRx.Struct.mosi

StateMachine stateMachine; // State machine instance
Timer printTimer; // Timer to print debug statements only once per second

int main(){
    pc.printf("\r\nMBED gets power now!");

    // wait_ms(30);
    // ecatReset = false;
    // wait_us(300);
    // ecatReset = true;
    // wait_us(5);
    // ecat.init();

    bool masterUpToDate = false;
    uint8_t lastMasterOk = 0;
    int missedMasterCounter = 0;
    Timer masterOkTimer;

    // Set initial EtherCAT outputs
    miso.masterShutdown = 0;
    miso.emergencyButtonState = 0;
    miso.HVOCTriggers = hvOCTriggers.readOCTriggers();
    miso.LVStates = (LVOkay2.read() << 1) | LVOkay1.read();
    miso.HVStates = hvControl.readAllOn();
    miso.PDBCurrent = currentSensors.readPDBCurrent();
    miso.LV1Current = currentSensors.readLV1Current();
    miso.LV2Current = currentSensors.readLV2Current();
    miso.HVCurrent = currentSensors.readHVCurrent();

    bit32 PDBCurrent, LV1Current, LV2Current, HVCurrent;

    printTimer.start(); // Start print timer right before entering infinite loop
    masterOkTimer.start();

    while(1){
        // Update EtherCAT variables
        ecat.update();
        
        // Get inputs from digitalIns and I2C bus
        bool buttonstate = button.read();
        bool LVOkay1State = LVOkay1.read();
        bool LVOkay2State = LVOkay2.read();
        bool emergencyButtonState = emergencyButton.read();
        uint8_t hvOCTriggerStates = hvOCTriggers.readOCTriggers();
        PDBCurrent.f = currentSensors.readPDBCurrent();
        LV1Current.f = currentSensors.readLV1Current();
        LV2Current.f = currentSensors.readLV2Current();
        HVCurrent.f = currentSensors.readHVCurrent();
        uint8_t hvOnStates = hvControl.readAllOn();
        uint8_t hvResetStates = hvControl.readAllReset();

        // Keep track of whether an EtherCAT master is present
        if(mosi.masterOk != lastMasterOk){ // Master sent a masterOk signal
            masterOkTimer.reset();
            masterUpToDate = true;
            missedMasterCounter = 0;
        }
        else if(masterOkTimer.read_ms() > 100){ // More than 100 ms since last masterOk signal
            masterUpToDate = false;
            missedMasterCounter = true;
        }
        lastMasterOk = mosi.masterOk;
        if(!masterUpToDate){
            missedMasterCounter++;
        }

        // Update system state
        stateMachine.updateState(buttonstate, (bool) mosi.masterOk, (bool) mosi.masterShutdownAllowed); // Temporary: change back!

        // Debug prints (Take care: these may take a lot of time and fuck up the masterOk timer!)
        if(printTimer.read_ms() > 1000){ // Print once every x ms
            // pc.printf("\r\nState: %s", stateMachine.getState().c_str());
            // pc.printf("\tKeepPDBOn: %d", stateMachine.getKeepPDBOn());
            // if((!LVOkayState) && (stateMachine.getState() == "LVOn_s")){
            //     pc.printf("LV not okay");
            // }
            // pc.printf("\r\n HV reset: %x, HV on: %x", hvResetStates, hvOnStates);
            // pc.printf("\r\n HV OC trigger: %x", hvOCTriggerStates);
            // pc.printf("\r\n PDB current: %f", PDBCurrent.f);
            // pc.printf("\r\n LV current: %f", LV1Current.f);
            // pc.printf("\r\n counter: %d", missedMasterCounter);
            printTimer.reset();
        }

        // Set LEDs and digitalOuts
        buttonLed = stateMachine.getOnOffButtonLedState();
        mbedLed1 = stateMachine.getOnOffButtonLedState(); // LED on if button LED is on
        mbedLed2 = (stateMachine.getState() == "MasterOk_s"); // LED on if in MasterOk state
        mbedLed3 = (hvControl.readAllOn() != 0) && emergencyButtonState; // LED on if any HV is on and not disabled by SSR
        mbedLed4 = (stateMachine.getState() == "Shutdown_s"); // LED on if in Shutdown state
        keepPDBOn = stateMachine.getKeepPDBOn();
        // LVOn1 = stateMachine.getLVOn(); // Don't listen to what master says over EtherCAT: LV net 1 not controllable by master
        // LVOn2 = (stateMachine.getLVOn() && (mosi.LVControl >> 1)); // If both the statemachine and master say LV should be on
        // LVOn2 = stateMachine.getLVOn(); // Temporary, remove later!
        LVOn1 = true;
        LVOn2 = true;

        // Control HV
        if(stateMachine.getState() == "MasterOk_s" || stateMachine.getState() == "ShutdownInit_s"){
            // In an allowed state to have HV on
            // emergencyButtonControl = mosi.emergencyButtonControl;
            // hvControl.setAllHV(mosi.HVControl);
            // hvControl.setAllHV(0b11111111) ;// Temporary, remove later!
            hvControl.setAllHV(0b11101110); // Temporary, remove later!
            emergencyButtonControl = true; // Enable HV // Temporary, remove later!
        }
        else{
            // Not in an allowed state to have any HV on
            // hvControl.setAllHV(0b11111111); // Temporary, remove later!
            hvControl.setAllHV(0b11101110); // Temporary, remove later!
            // emergencyButtonControl = false; // Disconnect HV
            emergencyButtonControl = true; // Enable HV // Temporary, remove later!
        }
        
        // Set miso's in EtherCAT buffers
        miso.emergencyButtonState = emergencyButtonState;
        miso.masterShutdown = stateMachine.getMasterShutdown();
        miso.HVOCTriggers = hvOCTriggerStates;
        miso.LVStates = (LVOkay2State << 1) | LVOkay1State;
        miso.HVStates = hvOnStates;
        miso.PDBCurrent = PDBCurrent.ui;
        miso.LV1Current = LV1Current.ui;
        miso.LV2Current = LV2Current.ui;
        miso.HVCurrent = HVCurrent.ui;
    }
    
}