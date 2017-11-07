#ifndef __REACTOR_H_
#define __REACTOR_H_
#include "macros.h"
#include "udt.h"
#include "ireactor.h"

NMS_BEGIN(network)

class CommandHandler;
class NetIoHandler;
class NetMgrBase;

class Reactor {
public:
	Reactor();
	Reactor(kevent::IReactor *reactor);
	~Reactor();

	void impl(kevent::IReactor *reactor) { _impl = reactor; }
	kevent::IReactor* impl() const { return _impl; }

	void net_mgr(NetMgrBase *mgr) { _net_mgr = mgr; }
	NetMgrBase* net_mgr() const { return _net_mgr; }

	int32 begin_event_loop();
	void end_event_loop();

	void add_cmd(int32 type, NetIoHandler *handler);

	bool active() const { return _impl->active(); }

	// io handler
	int32 register_handler(kevent::IHandler *handler) { return _impl->register_handler(handler); }
	int32 modify_handler(kevent::IHandler *handler) { return _impl->modify_handler(handler); }
	void unregister_handler(int32 hid) { return _impl->unregister_handler(hid); }

	// register a sig handler.
	int32 register_handler(kevent::IHandler *handler, int32 sig) { return _impl->register_handler(handler, sig); }

	// register a timer handler.
	int32 register_timer_handler(kevent::IHandler *handler, uint64 start_time, uint32 itv) { return _impl->register_timer_handler(handler, start_time, itv); }
	void unregister_timer_handler(int32 tid) { return _impl->unregister_timer_handler(tid); }

private:
	static void* io_routin(void *arg);

private:
	uint64 _thread_id;
	CommandHandler *_cmd_handler;
	kevent::IReactor *_impl;
	NetMgrBase *_net_mgr;
};

NMS_END // end namespace network

#endif // __REACTOR_H_

