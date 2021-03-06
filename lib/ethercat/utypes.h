#ifndef __UTYPES_H__
#define __UTYPES_H__

#include <cc.h>

// This file is a placeholder. Please include in your project a utypes.h file with the correct inputs and outputs.
// Make sure to include your own utypes.h file in your main file before including this file.

/* Inputs */
CC_PACKED_BEGIN
typedef struct
{
  uint8_t mosi_placeholder;
} CC_PACKED _Rbuffer;
CC_PACKED_END

/* Outputs */
CC_PACKED_BEGIN
typedef struct
{
  uint8_t miso_placeholder;
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
