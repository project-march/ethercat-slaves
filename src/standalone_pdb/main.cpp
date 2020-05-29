#include <mbed.h>
#include "LPC1768_pindefs_M4.h"
#include "HVControl.h"
#include "HVOCTriggers.h"
#include "CurrentSensors.h"
#include "StateMachine.h"

Serial pc(USBTX, USBRX, 9600);  // Serial communication for debugging

// On/off-related inputs/outputs
DigitalOut onOffButtonLed(LPC_ONOFFBUTTON_LED, false);     // True means LED on
DigitalOut mbedLed1(LPC_LED1, false);                      // Shows same as button led
DigitalIn onOffButton(LPC_ONOFFBUTTON_PRESSED, PullDown);  // True means button pressed
DigitalOut keepPDBOn(LPC_KEEP_PDB_ON, false);              // True means keep PDB on
DigitalOut mbedLed4(LPC_LED4, false);                      // Shows if in Shutdown state

// Low voltage related inputs/outputs
DigitalOut LVOn1(LPC_LVON1, false);        // True means on
DigitalIn LVOkay1(LPC_LVOKAY1, PullDown);  // True means okay
DigitalOut LVOn2(LPC_LVON2, false);        // True means on
DigitalIn LVOkay2(LPC_LVOKAY2, PullDown);  // True means okay

// Master communication related inputs/outputs
DigitalOut mbedLed2(LPC_LED2, false);  // Shows if in MasterOk state

// High voltage related inputs/outputs
DigitalOut mbedLed3(LPC_LED3, false);  // Shows if any HV is on
HVControl hvControl(LPC_I2C_SDA, LPC_I2C_SCL);
HVOCTriggers hvOCTriggers(LPC_I2C_SDA, LPC_I2C_SCL);

// Emergency button related inputs/outputs
DigitalIn emergencyButton(LPC_EMERGENCY_SWITCH_STATUS, PullDown);  // False means disconnected HV
DigitalOut emergencyButtonControl(LPC_EMERGENCY_SWITCH, false);    // False means disconnect HV

// Current sensing related inputs/outputs
CurrentSensors currentSensors(LPC_I2C_SDA, LPC_I2C_SCL);

union bit32
{
  float f;
  uint32_t ui;
};

StateMachine stateMachine;  // State machine instance
Timer printTimer;           // Timer to print debug statements only once per second

int main()
{
  pc.printf("\r\nMBED gets power now!");

  bit32 PDBCurrent, LV1Current, LV2Current, HVCurrent;

  printTimer.start();  // Start print timer right before entering infinite loop

  while (1)
  {
    // Get inputs from digitalIns and I2C bus
    bool onOffButtonState = onOffButton.read();
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

    // Update system state
    stateMachine.updateState(onOffButtonState, true, false);

    // Debug prints (Take care: these may take a lot of time and fuck up the masterOk timer!)
    if (printTimer.read_ms() > 1000)
    {  // Print once every x ms
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
    onOffButtonLed = stateMachine.getOnOffButtonLedState();
    mbedLed1 = stateMachine.getOnOffButtonLedState();      // LED on if button LED is on
    mbedLed2 = (stateMachine.getState() == "MasterOk_s");  // LED on if in MasterOk state
    mbedLed3 = (hvOnStates != 0) && emergencyButtonState;  // LED on if any HV is on and not disabled by SSR
    mbedLed4 = (stateMachine.getState() == "Shutdown_s");  // LED on if in Shutdown state
    keepPDBOn = stateMachine.getKeepPDBOn();
    LVOn1 = stateMachine.getLVOn();
    LVOn2 = stateMachine.getLVOn();

    // Control HV
    if (stateMachine.getState() == "MasterOk_s" || stateMachine.getState() == "ShutdownInit_s")
    {
      // In an allowed state to have HV on
      emergencyButtonControl = true;         // Enable HV, at least from software
      uint8_t desiredHVStates = 0b11111111;  // All joints on
      if (emergencyButtonState)
      {  // HV enabled
        if (hvOnStates != desiredHVStates)
        {                                                    // HV states not yet as desired
          hvControl.setAllHVStagedStartup(desiredHVStates);  // Staged startup to minimize inrush currents
        }
      }
      else
      {                                  // HV disabled
        hvControl.setAllHV(0b00000000);  // Reset all HV nets to off, even though already disabled
      }
    }
    else
    {
      // Not in an allowed state to have any HV on
      emergencyButtonControl = false;  // Disable HV
      hvControl.setAllHV(0b00000000);  // Reset all HV nets to off, even though already disabled
    }
  }
}
