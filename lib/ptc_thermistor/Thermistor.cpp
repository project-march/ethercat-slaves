#include "Thermistor.h"

Thermistor::Thermistor(PinName pin) : thermistorPin(pin)
{
}

uint8_t Thermistor::read()
{
    uint8_t temperatureTooHigh = 0;
    float thermistorReadout = thermistorPin.read();

    if(thermistorReadout > cuttoffVoltageRatio)
    {
        temperatureTooHigh = 1;
    }
    return temperatureTooHigh;
}