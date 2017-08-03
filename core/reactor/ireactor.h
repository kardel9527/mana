#ifndef __I_REACTOR_H_
#define __I_REACTOR_H_
#include "macros.h"
#include "udt.h"

NMS_BEGIN(kcommon)
class ITimerMgr;
class IHandler;
class SigMgr;
NMS_END // kcommon

NMS_BEGIN(kevent)
class ITimerMgr;
class IHandler;
class SigMgr;

class IReactor {
public:
	virtual ~IReactor() {}

	virtual int32 open(uint32 max_fd_sz, ITimerMgr *timer, SigMgr *sig) { return -1; }

	virtual void close() {}

	virtual int32 handle_events(timet interval = 0ull) { return -1; }

	virtual bool is_active() { return false; }

	virtual void deactive(bool active = false) {}

	virtual int32 register_handler(IHandler *handler, int32 mask) { return -1; }

	// register a sig handler
	virtual int32 register_handler(int32 sig_num, IHandler *handler) { return -1; }

	virtual int32 append_handler_mask_once(int32 hid, int32 mask) { return -1; }

	virtual int32 remove_handler(int32 hid) { return -1; }

	virtual int32 suspend_handler(int32 hid) { return -1; }

	virtual int32 resume_handler(int32 hid) { return -1; }

	virtual int32 register_timer(IHandler *handler, timet start_time, timet interval) { return -1; }

	virtual int32 cancel_timer(int32 tid) { return -1; }
};

NMS_END // end namespace kevent

#endif // __I_REACTOR_H_
