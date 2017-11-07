#ifndef __I_REACTOR_H_
#define __I_REACTOR_H_
#include "macros.h"
#include "udt.h"
#include "itimermgr.h"

NMS_BEGIN(kevent)
class ITimerMgr;
class IHandler;
class SigMgr;

class IReactor {
public:
	IReactor() : _active(false), _timer_mgr(0), _sig_mgr(0) {}
	virtual ~IReactor() { sdelete(_timer_mgr); }

	bool active() const { return _active; }
	void active(bool active) { _active = active; }

	void timer_mgr(ITimerMgr *mgr) { _timer_mgr = mgr; }
	ITimerMgr* timer_mgr() const { return _timer_mgr; }

	void sig_mgr(SigMgr *mgr) { _sig_mgr = mgr; }
	SigMgr* sig_mgr() const { return _sig_mgr; }

	virtual int32 open(ITimerMgr *timer, SigMgr *sig) { return -1; }
	virtual void close() {}

	virtual int32 handle_events(uint32 itv = 5) { return -1; }

	// io handler
	virtual int32 register_handler(IHandler *handler) { return invalid_handle; }
	virtual int32 modify_handler(IHandler *handler) { return invalid_handle; }
	virtual void unregister_handler(int32 hid) { }

	// register a sig handler.
	virtual int32 register_handler(IHandler *handler, int32 sig) { return invalid_handle; }

	// register a timer handler.
	virtual int32 register_timer_handler(IHandler *handler, uint64 start_time, int32 itv) { return invalid_handle; }
	virtual void unregister_timer_handler(int32 tid) { }

private:
	volatile bool _active;
	ITimerMgr *_timer_mgr;
	SigMgr *_sig_mgr;
};

NMS_END // end namespace kevent

#endif // __I_REACTOR_H_

