#include "macros.h"
#include "udt.h"

NMS_BEGIN(kcommon)

struct NetAddr {

	uint16 _port;
	const char *_addr[64];
};

NMS_END // end namespace kcommon

