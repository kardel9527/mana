#ifndef __I_TIMER_MGR_H_
#define __I_TIMER_MGR_H_
#include "udt.h"
#include "macros.h"

NMS_BEGIN(kevent)

class IHandler;

class ITimerMgr {
public:
	virtual ~ITimerMgr() {}

	// deleay in s, interval in micro seconds, interval requal 0 means not repeat
	virtual int32 add(timet delay, timet interval, IHandler *handler) { return 0; }

	virtual void remove(int32 id) {}

	virtual int32 expire_single() { return 0; }

	virtual int32 expire_all() { return 0; }
};

NMS_END // kcommon

#endif // __I_TIMER_MGR_H_
