#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <mbed.h>
#include <string>
#include <States.h>

class StateMachine
{
private:
    State currentState;
    Timer startUpTimer;
    bool onOffButtonLedState;
    bool keepPDBOn;
    bool LVon1;
    bool LVon2;
    bool HVon[8]; // {8,7,6,5,1,2,3,4}
    const int onOffButtonTime = 3000;
    
public:
    StateMachine();
    void updateState(bool buttonState);
    std::string getState();
    bool getOnOffButtonLedState();
    bool getKeepPDBOn();
    bool getLVOn1();
    bool getLVOn2();
};

#endif // STATEMACHINE_H