#ifndef __STRING_UTIL_H_
#define __STRING_UTIL_H_
#include "udt.h"
#include "macros.h"

NMS_BEGIN(kcommon)

NMS_BEGIN(stringutil)

inline uint32 smemcpy(char *dst, const char *src, uint32 max);

NMS_END // end namespace stringutil

NMS_END // end namespace kcommon

#endif // __STRING_UTIL_H_

