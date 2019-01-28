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
      uint32_t A;
      uint32_t B;
      uint32_t C;
      uint32_t D;
      uint32_t E;
      uint32_t F;
      uint32_t G;
      uint32_t H;
      uint32_t I;
      uint32_t J;
   } CC_PACKED mosi;
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
      uint32_t a;
      uint32_t b;
      uint32_t c;
      uint32_t d;
      uint32_t e;
      uint32_t f;
      uint32_t g;
      uint32_t h;
      uint32_t i;
      uint32_t j;
   } CC_PACKED miso;
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
