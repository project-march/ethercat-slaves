#ifndef __UTYPES_H__
#define __UTYPES_H__

#include <cc.h>

/* Inputs */
CC_PACKED_BEGIN
typedef struct
{
   CC_PACKED_BEGIN   
   struct
   {
      uint8_t masterShutdown;
      uint8_t emergencyButtonState;
      uint8_t HVOCTriggers;
      uint8_t LVStates;
      uint8_t HVStates;
      uint32_t PDBCurrent;
      uint32_t LV1Current;
      uint32_t LV2Current;
      uint32_t HVCurrent;
   } CC_PACKED miso;
   CC_PACKED_END
} CC_PACKED _Rbuffer;
CC_PACKED_END

/* Outputs */
CC_PACKED_BEGIN
typedef struct
{
   CC_PACKED_BEGIN
   struct
   {
      uint8_t masterOk;
      uint8_t masterShutdownAllowed;
      uint8_t emergencyButtonControl;
      uint8_t HVControl;
      uint8_t LVControl;
   } CC_PACKED mosi;
   CC_PACKED_END
} CC_PACKED _Wbuffer;
CC_PACKED_END

/* Parameters */
CC_PACKED_BEGIN
typedef struct
{
} CC_PACKED _Cbuffer;
CC_PACKED_END

/* Manufacturer specific data */
CC_PACKED_BEGIN
typedef struct
{
} CC_PACKED _Mbuffer;
CC_PACKED_END

extern _Rbuffer Rb;
extern _Wbuffer Wb;
extern _Cbuffer Cb;
extern _Mbuffer Mb;

#endif /* __UTYPES_H__ */
