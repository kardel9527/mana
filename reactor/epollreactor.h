#ifndef __EPOLL_REACTOR_H_
#define __EPOLL_REACTOR_H_
#include "udt.h"
#include "macros.h"
#include "ireactor.h"

NMS_BEGIN(kevent)

class EpollReactor : public IReactor {
	struct HandlerEntry { IHandler *_handler; int32 _mask; bool _suspend; HandlerEntry() : _handler(0), _mask(0), _suspend(false) {} };
	class HandlerRepository {
	public:
		HandlerRepository() : _size(0),_capacity(0), _tuple(0) {}
		~HandlerRepository();

		int32 open(uint32 max_handler_sz);

		int32 add(IHandler *hanlder, int32 mask);

		int32 remove(int32 hid);

		HandlerEntry* find(int32 hid);

	private:
		HandlerRepository(const HandlerRepository &other) {}
		HandlerRepository& operator = (const HandlerRepository& other) { return *this; }

	private:
		uint32 _size;
		uint32 _capacity;
		HandlerEntry *_tuple;
	};

public:
	EpollReactor();

	virtual ~EpollReactor();

	virtual int32 open(uint32 max_fd_sz, ITimerMgr *timer, SigMgr *sig);

	virtual void close();

	virtual int32 handle_events(timet interval = 0ull);

	virtual bool is_active();

	virtual void deactive(bool active = false);

	virtual int32 register_handler(IHandler *handler, int32 mask);

	// register a sig handler
	virtual int32 register_handler(int32 sig_num, IHandler *handler);

	virtual int32 remove_handler(int32 hid);

	virtual int32 suspend_handler(int32 hid);

	virtual int32 resume_handler(int32 hid);

	virtual int32 register_timer(IHandler *handler, timet start_time, timet interval);

	virtual int32 cancel_timer(int32 tid);

private:
	EpollReactor(const EpollReactor &other) {}
	EpollReactor& operator = (const EpollReactor &other) { return *this; }

private:
	int32 epoll_mask(int32 mask);

	int32 resume_handler_impl(HandlerEntry *entry, int32 op);

private:
	int32 _epoll_fd;
	bool _active;
	HandlerRepository _handler;
	ITimerMgr *_timer_queue;
};

NMS_END // end namespace kevent

#endif // __EPOLL_REACTOR_H_
