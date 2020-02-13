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

#include "Temperature.h"

#ifdef DEBUG_IMP
    #undef DEBUG_IMP
#endif
#ifdef DEBUG_PRN
    #undef DEBUG_PRN
#endif
#ifdef DEBUG_ARG
    #undef DEBUG_ARG
#endif

#define DEBUG_IMP Serial * pc, const int debugLevel
#define DEBUG_PRN(fmt) if((pc != NULL) & (debugLevel > 0)) pc->printf fmt
#define DEBUG_ARG pc, debugLevel - 1

//Constructors
Temperature::Temperature(PinName pin, DEBUG_IMP)
{
    DEBUG_PRN(("\r\nTemperature constructor called with PinName argument %i.", pin));
    m_device = new DS18B20(true, true, false, pin);
    DEBUG_PRN(("\r\nDevice class constructed @ 0x%08X.", m_device));
    
    this->connected = true;
    this->numberOfFails = 0;
    this->disconnectTimerRunning = false;
    this->previousResult = 0;

    if(this->init(8, DEBUG_ARG))
    {
        DEBUG_PRN(("\r\nInitialization failed."));
    }
}

//public member functions
float Temperature::read(DEBUG_IMP)
{
    DEBUG_PRN(("\r\nReading temperature."));
    float result = 0;
    if(this->connected)
    {
        DEBUG_PRN(("\r\nDevice connected."));
        result = this->m_device->readTemperature(DEBUG_ARG);
        if(result == -999.0)
        {
            //CRC check failed, measurement invalid
            this->numberOfFails++;
            result = this->previousResult;
            DEBUG_PRN(("\r\nMeasurement failed. Number of fails = %i", this->numberOfFails));
            if(this->numberOfFails >= this->maxNumberOfFails)
            {
                //The device is probably disconnected. 
                //Try to reconnect/reset the device one more time.
                DEBUG_PRN(("\r\nMeasurement failed too many times. "
                    "Device is probably disconnected."));
                int status = this->init(1, DEBUG_ARG);
                DEBUG_PRN(("\r\nReconnect status = %i", status));
                if(status)
                {
                    DEBUG_PRN(("\r\nCouldn't reconnect. Assuming disconnected."));
                    //Reconnecting didn't work. Device is gone.
                    this->connected = false;
                    this->disconnectTimer.start();
                    this->disconnectTimer.reset();
                    this->disconnectTimerRunning = true;
                    this->numberOfFails = 0;
                    result = -1.0;
                }
                else
                {
                    DEBUG_PRN(("\r\nManaged to reconnect. Continuing as normal."));
                    this->connected = true;
                    this->numberOfFails = 0;
                    this->disconnectTimerRunning = false;
                }
            }
        }
        else
        {
            // CRC check succeeded, measurement valid
            this->numberOfFails = 0;
            this->previousResult = result;
        }
        DEBUG_PRN(("\r\nResult = %f", result));
        return result;
    }
    else
    {
        DEBUG_PRN(("\r\nDevice disconnected."));
        if(this->disconnectTimerRunning && this->disconnectTimer.read() > this->reconnectTime)
        {
            if(this->init(1, DEBUG_ARG))
            {
                DEBUG_PRN(("\r\nCouldn't reconnect the device."));
                this->disconnectTimer.reset();
            }
            else
            {
                //Device is now connected.
                DEBUG_PRN(("\r\nReconnected the device."));
                this->connected = true;
                this->disconnectTimer.stop();
                this->disconnectTimer.reset();
                this->disconnectTimerRunning = false;
                return this->read();
            }
        }
        else if(!this->disconnectTimerRunning)
        {
            this->disconnectTimer.reset();    
            this->disconnectTimer.start();
            this->disconnectTimerRunning = true;
        }
        DEBUG_PRN(("\r\nReturning -1.0"));
        this->previousResult = -1.0;
        return -1.0;
    }
}

int Temperature::init(int tries, DEBUG_IMP)
{
    int status = -1; // Fail
    for (int i = 0; i < tries; i++)
    {
        DEBUG_PRN(("\r\nTrying to initialize. Try %i.", i+1));
        status = this->m_device->initialize(DEBUG_ARG);
        if(status == 0)
        {
            DEBUG_PRN(("\r\nInitialized successfully."));
            break;
        }

    }
    if(status != 0)
    {
        DEBUG_PRN(("\r\nInitialization failed. Status = %i.", status));
        this->connected = false;
        return status;
    }
    this->m_device->setResolution(nineBit, DEBUG_ARG);
    return 0;
}   //Temperature::init(int tries)