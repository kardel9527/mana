#include <string.h>
#include "stringutil.h"

NMS_BEGIN(kcommon)

NMS_BEGIN(stringutil)

uint32 smemcpy(char *dst, const char *src, uint32 max) {
	uint32 len = ::strlen(src);
	len = len > max ? max : len;
	::memcpy(dst, src, len);
	return len;
}

NMS_END // end namespace stringutil

NMS_END // end namespace kcommon

