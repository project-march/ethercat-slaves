#include "Thermistor.h"

Thermistor::Thermistor(PinName pin) : thermistorPin(pin)
{
}

bool Thermistor::read()
{
    bool temperatureTooHigh = false;
    float thermistorReadout = thermistorPin.read();

    if(thermistorReadout > cuttoffVoltageRatio)
    {
        temperatureTooHigh = true;
    }
    return temperatureTooHigh;
}