#ifndef __I_HANDLER_H_
#define __I_HANDLER_H_
#include "udt.h"
#include "macros.h"
#include "reactor.h"

NMS_BEGIN(kevent)

#define safe_close_handle(handler) \
	do { if (handler && handler->handle() != invalid_handle) { ::close(handler->handle()); handler->handle(invalid_handle); } } while (0)

class IHandler {
public:
	enum HandlerEventType { HET_READ = 0x0001, HET_WRITE = 0x0002, HET_EXCEPT = 0x0004 };

public:
	IHandler() : _handle(invalid_handle), _handle_flag(HET_READ|HET_EXCEPT), _reactor(0) {}
	virtual ~IHandler() {}

	void reactor(network::Reactor *reactor) { _reactor = reactor; }
	network::Reactor* reactor() const { return _reactor; }

	void handle(int32 hid) { _handle = hid; }
	int32 handle() const { return _handle; }

	void flag(int32 flag) { _handle_flag = flag; }
	int32 flag() const { return _handle_flag; }

	int32 register_io_event() { return reactor()->register_handler(this); }
	void unregister_io_event() { reactor()->unregister_handler(handle()); handle_close(); }

	int32 register_timer_event(int32 itv) { _handle = reactor()->register_timer_handler(this, 0, itv); return _handle > 0 ? 0 : -1; }
	void unregister_timer_event() { reactor()->unregister_timer_handler(handle()); handle_close(); _handle = invalid_handle; }

	virtual int32 handle_input() { return 0; }

	virtual int32 handle_output() { return 0; }

	virtual int32 handle_error() { return -1; }

	virtual int32 handle_close() { return 0; }

	virtual int32 handle_timeout() { return 0; }

	virtual int32 handle_sig() { return 0; }

private:
	// for timer, _handle=timer id, for socket, _handle=file descriptor, for sig, unused.s
	int32 _handle;
	// handle flag, specially for socket, event.
	int32 _handle_flag;
	// reactor impl.
	network::Reactor *_reactor;
};

NMS_END // end namespace kevent

#endif // __I_HANDLER_H_

