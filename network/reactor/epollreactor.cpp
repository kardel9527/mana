#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "ihandler.h"
#include "petty.h"
#include "timerlist.h"
#include "timeutil.h"
#include "epollreactor.h"

NMS_BEGIN(kevent)

// EpollReactor
////////////////////////////////////////////////////////////////////////////////
EpollReactor::EpollReactor() : _epoll_fd(invalid_handle), _event(0) {
}

EpollReactor::~EpollReactor() {
	assert(!active() && _epoll_fd == -1 && "close first.");
	close();
}

int32 EpollReactor::open(ITimerMgr *timer, SigMgr *sig) {
	timer_mgr(timer);
	sig_mgr(sig);

	_event = new epoll_event;
	assert(_event && "memory alloc error!");
	_event->data.ptr = 0;
	_event->events = 0;

	_epoll_fd = ::epoll_create(256);

	return _epoll_fd == -1 ? -1 : 0;
}

void EpollReactor::close() {
	active(false);
	sclose(_epoll_fd);
	sdelete(_event);
}

int32 EpollReactor::handle_events(uint32 itv/* = 0ull*/) {
	if (!_event->data.ptr) {
		// TODO: handle more event here?
		itv = timer_mgr()->expire_time(ktimeutil::get_current_time(), itv);
		int32 ret = ::epoll_wait(_epoll_fd, _event, 1, itv);
		if (ret == -1 && errno != EINTR) {
			// TODO: error ocured.
			return -1;
		}
	}

	// if no timer event happened, dispatch an io event.
	if (!dispatch_timer_event())
		dispatch_io_event();

	return 0;
}

int32 EpollReactor::register_handler(IHandler *handler) {
	assert(handler && "empty handler!");
	assert(handler->handle() > 0 && "handler handle error!");

	int32 ep_mask = epoll_mask(handler->flag());

	epoll_event event;
	event.data.ptr = (void *)handler;
	event.events = ep_mask | EPOLLET | EPOLLONESHOT;

	return ::epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, handler->handle(), &event);
}

int32 EpollReactor::modify_handler(IHandler *handler) {
	assert(handler && "empty handler!");
	assert(handler->handle() > 0 && "handler handle error!");

	int32 ep_mask = epoll_mask(handler->flag());

	epoll_event event;
	event.data.ptr = handler;
	event.events = ep_mask | EPOLLET | EPOLLONESHOT;
	return ::epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, handler->handle(), &event);
}

void EpollReactor::unregister_handler(int32 hid) {
	epoll_event event;
	event.data.fd = hid;
	event.events = 0;

	::epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
}

int32 EpollReactor::register_handler(IHandler *handler, int32 sig_num) {
	// TODO:support signal handler.
	return -1;
}

int32 EpollReactor::register_timer_handler(IHandler *handler, uint64 start_time, int32 itv) {
	return timer_mgr()->add(start_time, itv, handler);
}

void EpollReactor::unregister_timer_handler(int32 tid) {
	timer_mgr()->remove(tid);
}

int32 EpollReactor::epoll_mask(int32 mask) {
	int32 ret = 0;
	if (mask & IHandler::HET_READ)
		ret |= EPOLLIN;
	if (mask & IHandler::HET_WRITE)
		ret |= EPOLLOUT;

	ret |= EPOLLERR;

	return ret;
}

int32 EpollReactor::dispatch_io_event() {
	if (!_event->data.ptr) return 0;

	IHandler *handler = (IHandler *)_event->data.ptr;

	int32 status = 1;
	if (kcommon::bit_test(_event->events, EPOLLIN)) {
		while (status > 0) { status = handler->handle_input(); }
	}

	status = status == 0 ? 1 : -1;
	if (kcommon::bit_test(_event->events, EPOLLOUT)) {
		while (status > 0) { status = handler->handle_output(); }
		// remove write event.
		handler->flag(kcommon::bit_disable(handler->flag(), IHandler::HET_WRITE));
	}
	
	if (kcommon::bit_test(_event->events, EPOLLHUP) || kcommon::bit_test(_event->events, EPOLLERR)) {
		status = -1;
	}

	if (status < 0) {
		unregister_handler(handler->handle());
		handler->handle_close();
	} else {
		modify_handler(handler);
	}

	// reset flag.
	_event->data.ptr = 0;
	_event->events = 0;

	return 1;
}

int32 EpollReactor::dispatch_timer_event() {
	return timer_mgr()->expire_single(ktimeutil::get_current_time());
}

NMS_END // end namespace kevent

