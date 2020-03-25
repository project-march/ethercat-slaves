#ifndef THERMISTOR_
#define THERMISTOR_

#include <mbed.h>

class Thermistor{
public:
    Thermistor(PinName pin);
    uint8_t read();
private:
    AnalogIn thermistorPin;

    const float referenceResistance = 100;
    const float cutoffResistance = 570;
    const float cuttoffVoltageRatio = cutoffResistance/(cutoffResistance + referenceResistance);
};

#endif  //THERMISTOR_