#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <mbed.h>
#include <string>
#include <States.h>

class StateMachine
{
private:
    systemState currentState;
    Timer startUpTimer;
    bool onOffButtonLedState;
    bool keepPDBOn;
    const int onOffButtonTime = 3000;
    
public:
    StateMachine();
    void updateState(bool buttonState);
    std::string getState();
    bool getOnOffButtonLedState();
    bool getKeepPDBOn();
};

#endif // STATEMACHINE_H