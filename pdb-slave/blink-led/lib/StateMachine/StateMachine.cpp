#include "StateMachine.h"

// Constructor
StateMachine::StateMachine()
{
   this->currentState = Init;
   this->startUpTimer.start();
   this->onOffButtonLedState = false;
   this->keepPDBOn = false;
   this->LVon1 = false;
   this->LVon2 = false;
}

// Updates the current state based on the button inputs
void StateMachine::updateState(bool buttonState){
    int timeSinceStartupMs = startUpTimer.read_ms();
    switch(this->currentState){
        case Init:
            if(timeSinceStartupMs > this->onOffButtonTime){
                this->onOffButtonLedState = true;
                this->keepPDBOn = true;
            }
            if(!buttonState){
                if(timeSinceStartupMs < this->onOffButtonTime){
                    this->currentState = TurnOnReleasedTooShort;
                }
                else{
                    this->currentState = LVOn;
                }
            }
            break;
        case LVOn:
            this->LVon1 = true;
            this->LVon2 = true;
            // Wait for ethercat message from master saying it is operational
            // If so, go to MasterOn state
            break;
    }
}

std::string StateMachine::getState(){
    switch(this->currentState){
        case Init:
            return "Init";
        case TurnOnReleasedTooShort:
            return "Too short";
        case LVOn:
            return "LV On";
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
    return this->LVon1;
}

bool StateMachine::getLVOn2(){
    return this->LVon2;
}
