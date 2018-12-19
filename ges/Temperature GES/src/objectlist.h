//First Define Custom PDO

typedef struct
{
	uint8_t	miso_LED_ack;
}_PDO_In_Struct;

typedef struct 
{
	uint8_t	mosi_LED_command;
}_PDO_Out_Struct;

//Next EasyCAT essential Buffer
typedef union _BUFFI{
    _PDO_In_Struct _PDO_In;
    unsigned long Long[8];
    char Byte[64];
}BUFFIN;

typedef union _BUFFO
{
    _PDO_Out_Struct _PDO_Out;
    unsigned long Long[8];
    char Byte[64];
}BUFFOUT;

//External Definations for Main

BUFFOUT Out;
BUFFIN In;

//Final Definations for Using PDO objects
#define miso_LED_ack In._PDO_In.miso_LED_ack
#define mosi_LED_command Out._PDO_Out.mosi_LED_command