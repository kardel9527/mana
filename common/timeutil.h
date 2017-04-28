#ifndef __TIME_UTIL_H_
#define __TIME_UTIL_H_
#include "macros.h"
#include "udt.h"

NMS_BEGIN(ktimeutil)

bool check_is_same_day(uint64 t1, uint64 t2);

NMS_END // ktimeUtil

#endif // __TIME_UTIL_H_

