#ifndef __REACTOR_H_
#define __REACTOR_H_
#include "macros.h"
#include "udt.h"
#include "ireactor.h"

NMS_BEGIN(kevent)

class Reactor {
public:
	Reactor(IReactor *reactor, bool del) : _del(del), _impl(reactor) {}

	~Reactor() { if (_del) safe_delete(_reactor); }

	int32 handle_events(timeval interval = 0ull) { return -1; }

	bool is_active() { return _impl->is_active(); }

	void deactive(bool active = false) { _impl->deactive(active); }

	int32 register_handler(IHandler *handler, int32 mask) { return -1; }

	int32 register_handler(int32 sig_num, IHandler *handler) { return -1; }

	int32 remove_handler(int32 hid) { return -1; }

	int32 suspend_handler(int32 hid) { return -1; }

	int32 resume_handler(int32 hid) { return -1; }

	int32 register_timer(IHandler *handler, timeval start_time, timeval interval) { return -1; }

	int32 cancel_timer(int32 tid) { return -1; }

private:
	bool _del;
	IReactor *_impl;
};

NMS_END // end namespace kevent


#endif // __REACTOR_H_

