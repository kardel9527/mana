#ifndef __EPOLL_REACTOR_H_
#define __EPOLL_REACTOR_H_
#include "udt.h"
#include "macros.h"
#include "ireactor.h"

struct epoll_event;

NMS_BEGIN(kevent)

class EpollReactor : public IReactor {
public:
	EpollReactor();
	virtual ~EpollReactor();

	virtual int32 open(ITimerMgr *timer, SigMgr *sig);
	virtual void close();

	virtual int32 handle_events(uint32 itv = 5);

	// io handler.
	virtual int32 register_handler(IHandler *handler);
	virtual int32 modify_handler(IHandler *handler);
	virtual void unregister_handler(int32 hid);

	// signal handler.
	virtual int32 register_handler(IHandler *handler, int32 sig);

	// timer handler
	virtual int32 register_timer_handler(IHandler *handler, uint64 start_time, int32 interval);
	virtual void unregister_timer_handler(int32 tid);

private:
	EpollReactor(const EpollReactor &other) {}
	EpollReactor& operator = (const EpollReactor &other) { return *this; }

private:
	int32 epoll_mask(int32 mask);

	int32 dispatch_io_event();

	int32 dispatch_timer_event();

private:
	int32 _epoll_fd;
	epoll_event *_event;
};

NMS_END // end namespace kevent

#endif // __EPOLL_REACTOR_H_
