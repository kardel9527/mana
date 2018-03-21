#ifndef __I_TIMER_MGR_H_
#define __I_TIMER_MGR_H_
#include "udt.h"
#include "macros.h"

NMS_BEGIN(kevent)

class IHandler;

class ITimerMgr {
public:
	ITimerMgr() {}

	virtual ~ITimerMgr() {}

	// deleay in s, interval in micro seconds, interval requal 0 means not repeat
	virtual int32 add(uint64 start, uint32 itv, IHandler *handler) { return 0; }

	virtual void remove(int32 id) {}

	virtual int32 expire_single(uint64 now) { return 0; }

	virtual int32 expire_all(uint64 now) { return 0; }

	virtual uint32 expire_time(uint64 now, uint32 itv) { return itv; }
};

NMS_END // end namespace kevent

#endif // __I_TIMER_MGR_H_

