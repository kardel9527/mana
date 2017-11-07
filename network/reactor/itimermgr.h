#ifndef __I_TIMER_MGR_H_
#define __I_TIMER_MGR_H_
#include "udt.h"
#include "macros.h"

NMS_BEGIN(kevent)

class IHandler;

class ITimerMgr {
public:
	ITimerMgr() : _seed(0) {}

	virtual ~ITimerMgr() {}

	// deleay in s, interval in micro seconds, interval requal 0 means not repeat
	virtual int32 add(int32 delay, int32 itv, IHandler *handler) { return 0; }

	virtual void remove(int32 id) {}

	virtual int32 expire_single() { return 0; }

	virtual int32 expire_all() { return 0; }

	virtual int32 expire_time(int32 itv) { return itv; }

protected:
	int32 gen_id() { return ++_seed; }

private:
	int32 _seed;
};

NMS_END // end namespace kevent

#endif // __I_TIMER_MGR_H_

