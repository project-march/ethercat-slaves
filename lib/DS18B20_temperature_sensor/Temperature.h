/* * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * * * * * * * * * * * * *\
 *  Library implementing the software interface for a DS18B20+ temperature sensor.      *
 *  Hardware consists of a TO-92 three-pin device with a round side and a flat side.    *
 *      Pin "GND" should be connected to ground voltage.                                *
 *      Pin "DQ" should be connected to a digital input/output pin.                     *
 *      Pin "VDD" should be connected to a voltage between 3.0V and 5.5V.               *
 *          ┌──────────┐                                                                *
 *          │         O┼────────── 1 GND              /───────\                         *
 *          │         O┼────────── 2 DQ              /         \                        *
 *          │         O┼────────── 3 VDD            │  3  2  1  │                       *
 *          └──────────┘                            └───────────┘                       *
 *   CURVED SIDE VIEW (flat side on the bottom)     PIN SIDE VIEW                       *
 *                                                                                      *
 * Created by P. Verton for the MARCH 3 team of Project MARCH                           *
 * Date: 09-APR-2018                                                                    *
\* * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * * * * * * * * * * * * */

#ifndef TEMPERATURE_
#define TEMPERATURE_
#include <mbed.h>
#include "DS18B20.h"

#ifdef DEBUG_DEC
    #undef DEBUG_DEC
#endif
#define DEBUG_DEC Serial * pc = NULL, int debugLevel = 0

class Temperature
{
public:
//Constructors
    Temperature(PinName pin, DEBUG_DEC);

//Public member functions
    //Reads the current sensor temperature
    //  float   return      Temperature value in celsius.
    //                      Has value -999 if measurement is invalid.
    //                      Has value -1000 if device is disconnected.
    float read(DEBUG_DEC);

    //Initializes the device and makes sure all settings are correct.
    //  int     tries       Number of times it should try to init the device.
    //          default = 1
    //  int     return      Indicates whether it initialized succesfully.
    //          0           Indicates it initialized succesfully
    //          -1          Indicates the device couldn't be found
    //          -2          indicates the address CRC is incorrect
    //          -3          Indicates the deviceID is incorrect
    int init(int tries = 1, DEBUG_DEC);

private:
//Private member functions

//Class members
    //Temperature sensor peripheral
    DS18B20 *   m_device;

    //Timer to see how long it has been since the last correct measurement
    Timer       disconnectTimer;

    //Whether the disconnect Timer is running
    bool        disconnectTimerRunning;

    //Whether the device is currently connected
    bool        connected;

    //Previous (valid) measurement
    float       previousResult;

    //Number of failed measurements in a row
    int         numberOfFails;

    //Number of failed measurements in a row needed to assume the device is disconnected
    static const int maxNumberOfFails = 10;

    //Amount of time in seconds to wait before attempting to reconnect to the device when disconnected
    static constexpr float reconnectTime = 0.5;
};

#endif //TEMPERATURE