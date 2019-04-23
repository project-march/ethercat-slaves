#include "StateMachine.h"

// Constructor
StateMachine::StateMachine()
{
   this->currentState = Init_s;
   this->onOffButtonTimer.reset();
   this->onOffButtonTimer.start();
   this->ledTimer.reset();
   this->onOffButtonLedState = false;
   this->keepPDBOn = false;
   this->LVon[0] = false;
   this->LVon[1] = false;
   this->masterShutdown = false;
}

// Updates the current state based on the button inputs
void StateMachine::updateState(bool buttonState, bool masterOkState, bool shutdownAllowedState){
    int buttonTimeMs = this->onOffButtonTimer.read_ms();
    int ledTimeMs = this->ledTimer.read_ms();
    switch(this->currentState){
        case Init_s:
            if(buttonTimeMs > this->onOffButtonTimeShort){
                this->onOffButtonLedState = true;
                this->keepPDBOn = true;
            }
            if(!buttonState){
                if(buttonTimeMs <= this->onOffButtonTimeShort){
                    this->currentState = TurnOnReleasedTooShort_s;
                }
                else{
                    this->currentState = LVOn_s;
                }
                this->onOffButtonTimer.stop();
                this->onOffButtonTimer.reset();
            }
            break;
        case LVOn_s:
            this->onOffButtonLedState = true;
            this->LVon[0] = true;
            this->LVon[1] = true;
            // Handling the on/off button
            if(buttonState){
                // Start shutdown timer when the button is pressed
                this->onOffButtonTimer.start();
            }
            else{
                // Released too early, so stop and reset timer
                this->onOffButtonTimer.stop();
                this->onOffButtonTimer.reset();
            }
            if(buttonTimeMs > this->onOffButtonTimeLong){
                // Button held long enough, so forced shutdown
                this->currentState = TurnOff_s;
            }
            // Perhaps toggle led after short button time? To be decided
            // Wait for ethercat message from master saying it is operational to go to MasterOk
            // Temporary solution: fake with digitalIn signal
            if(masterOkState){
                this->currentState = MasterOk_s;
            }
            break;
        case MasterOk_s:
            this->onOffButtonLedState = true;
            // Handling the on/off button
            if(buttonState){
                // Start shutdown timer when the button is pressed
                this->onOffButtonTimer.start();
            }
            else{
                // Released too early, so stop and reset timer
                this->onOffButtonTimer.stop();
                this->onOffButtonTimer.reset();
            }
            if(buttonTimeMs > this->onOffButtonTimeShort){
                // Button held long enough, so go to ShutdownInit
                this->currentState = ShutdownInit_s;
                this->onOffButtonLedState = false;
                this->ledTimer.start();
            }
            break;
        case ShutdownInit_s:
            this->masterShutdown = true;
            if(buttonTimeMs > this->onOffButtonTimeLong){
                if(buttonState){
                    // Forced shutdown
                    this->currentState = TurnOff_s;
                }
                else if(shutdownAllowedState){
                    // Master has allowed shutdown
                    this->currentState = Shutdown_s;
                    this->onOffButtonTimer.stop();
                    this->onOffButtonTimer.reset();
                }
                else{
                    // Return to MasterOk if master doesn't allow shutdown
                    this->currentState = MasterOk_s;
                    this->onOffButtonTimer.stop();
                    this->onOffButtonTimer.reset();
                }
            }
            // Let button led blink by toggling every *blinkTime* ms 
            if(((ledTimeMs % (2*this->blinkTime)) <= this->blinkTime) && ledTimeMs > 0){
                this->onOffButtonLedState = true;
            }
            else
            {
                this->onOffButtonLedState = false;
            }
            break;
        case Shutdown_s:
            // Todo: turn off HV
            // Handling the on/off button
            if(buttonState){
                // Start shutdown timer when the button is pressed
                this->onOffButtonTimer.start();
            }
            else{
                // Released too early, so stop and reset timer
                this->onOffButtonTimer.stop();
                this->onOffButtonTimer.reset();
            }
            if(buttonTimeMs > this->onOffButtonTimeLong){
                // Button held long enough, so forced shutdown
                this->currentState = TurnOff_s;
            }
            if(!masterOkState){
                // Master has indicated it is done, so now turn off
                this->currentState = TurnOff_s;
            }
            // Let button led blink by toggling every *blinkTime* ms
            if(((ledTimeMs % (2*this->blinkTime)) <= this->blinkTime) && ledTimeMs > 0){
                this->onOffButtonLedState = true;
            }
            else
            {
                this->onOffButtonLedState = false;
            }
            break;
        case TurnOff_s:
            // Todo: turn off HV
            this->LVon[0] = false;
            this->LVon[1] = false;
            this->keepPDBOn = false;
            this->onOffButtonLedState = false;

    }
}

std::string StateMachine::getState(){
    switch(this->currentState){
        case Init_s:
            return "Init_s";
        case TurnOnReleasedTooShort_s:
            return "TurnOnReleasedTooShort_s";
        case LVOn_s:
            return "LVOn_s";
        case MasterOk_s:
            return "MasterOk_s";
        case ShutdownInit_s:
            return "ShutdownInit_s";
        case Shutdown_s:
            return "Shutdown_s";
        case TurnOff_s:
            return "TurnOff_s";
        default:
            return "Default";
    }
}

bool StateMachine::getOnOffButtonLedState(){
    return this->onOffButtonLedState;
}

bool StateMachine::getKeepPDBOn(){
    return this->keepPDBOn;
}

bool StateMachine::getLVOn1(){
    return this->LVon[0];
}

bool StateMachine::getLVOn2(){
    return this->LVon[1];
}

bool StateMachine::getMasterShutdown(){
    return this->masterShutdown;
}
