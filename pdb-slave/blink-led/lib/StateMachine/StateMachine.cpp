#include "StateMachine.h"

// Constructor
StateMachine::StateMachine()
{
   this->currentState = init;
   this->startUpTimer.start();
   this->onOffButtonLedState = false;
   this->keepPDBOn = false;
}

// Updates the current state based on the button inputs
void StateMachine::updateState(bool buttonState){
    int timeSinceStartupMs = startUpTimer.read_ms();
    switch(this->currentState){
        case init:
            if(timeSinceStartupMs > this->onOffButtonTime){
                this->onOffButtonLedState = true;
                this->keepPDBOn = true;
            }
            if(!buttonState){
                if(timeSinceStartupMs < this->onOffButtonTime){
                    this->currentState = OnOffReleased_short;
                }
                else{
                    this->currentState = OnOffReleased_long;
                }
            }
            break;
    }
}

std::string StateMachine::getState(){
    switch(this->currentState){
        case init:
            return "Init";
        case OnOffReleased_short:
            return "Short";
        case OnOffReleased_long:
            return "Long";
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

