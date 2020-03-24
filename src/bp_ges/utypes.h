#ifndef __UTYPES_H__
#define __UTYPES_H__

#include "cc.h"

/* Object dictionary storage */

typedef struct
{
   /* Inputs */

   struct
   {
      int32_t TemperatureLHAA;
      int32_t TemperatureRHAA;
      uint8_t OverTemperatureTriggerBP;
   } miso;

} _Objects;

extern _Objects Obj;

#endif /* __UTYPES_H__ */
