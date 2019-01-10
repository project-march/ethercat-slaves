#ifndef OBJECTLIST_
#define OBJECTLIST_
#include <mbed.h>
//First Define Custom PDO

typedef struct _pdo_miso_struct{
    uint8_t miso_LED_ack;
}pdo_miso_struct;

typedef struct _pdo_mosi_struct{
    uint8_t mosi_LED_command;      
}pdo_mosi_struct;

//Next essential Buffer
typedef union _bufferMiso{
    pdo_miso_struct Struct;
    unsigned long Long[8];
    char Byte[32];
}bufferMiso;

typedef union _bufferMosi
{
    pdo_mosi_struct Struct;
    unsigned long Long[8];
    char Byte[32];
}bufferMosi;      


//Final Definations for Using PDO objects

#define miso_LED_ack        Ethercat::pdoTx.Struct.miso_LED_ack

#define mosi_LED_command    Ethercat::pdoRx.Struct.mosi_LED_command

#endif //OBJECTLIST_