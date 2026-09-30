
// author: somnathkarmakar1203@gmail.com
// ta/user_ta_header_defines.h

#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

#include <pilike.h>

#define TA_UUID PILIKE_UUID
#define TA_FLAGS (TA_FLAG_SINGLE_INSTANCE | TA_FLAG_INSTANCE_KEEP_ALIVE)
#define TA_STACK_SIZE (4*1024)
#define TA_DATA_SIZE (64*1024)

#endif /* USER_TA_HEADER_DEFINES_H */
