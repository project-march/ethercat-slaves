#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <mbed.h>
#include <string>
#include <States.h>

class StateMachine
{
private:
    State currentState;
    Timer onOffButtonTimer;
    Timer ledTimer;
    bool onOffButtonLedState;
    bool keepPDBOn;
    bool LVon[2];
    bool HVon[8]; // {8,7,6,5,1,2,3,4}
    bool masterShutdown;
    const int onOffButtonTimeShort = 3000; // time on/off button needs to be pressed to turn on or off in milliseconds
    const int onOffButtonTimeLong = 10000; // time on/off button needs to be pressed to force shutdown in milliseconds
    const int blinkTime = 250; // time that led is on/off when blinking in milliseconds
    
public:
    StateMachine();
    void updateState(bool buttonState, bool masterOkState, bool shutdownAllowedState);
    std::string getState();
    bool getOnOffButtonLedState();
    bool getKeepPDBOn();
    bool getLVOn1();
    bool getLVOn2();
    unsigned char getHVOn();
    bool getMasterShutdown();
};

#endif // STATEMACHINE_H