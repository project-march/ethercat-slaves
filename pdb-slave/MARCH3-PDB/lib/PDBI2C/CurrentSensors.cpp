#include "CurrentSensors.h"

CurrentSensors::CurrentSensors(PinName SDA_PIN, PinName SCL_PIN) : bus(SDA_PIN, SCL_PIN){

}

float CurrentSensors::readPDBCurrent(){
    return 0;
}

float CurrentSensors::readLV1Current(){
    return 0;
}

float CurrentSensors::readLV2Current(){
    return 0;
}

float CurrentSensors::readHVCurrent(){
    return 0;
}

