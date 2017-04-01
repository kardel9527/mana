#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "ihandler.h"
#include "timerlist.h"
#include "epollreactor.h"

NMS_BEGIN(kevent)
// EpollReactor::HandlerRepository
////////////////////////////////////////////////////////////////////////////////
EpollReactor::HandlerRepository::~HandlerRepository() {
	_size = 0;
	_capacity = 0;

	// notice the handler
	for (uint32 i = 0; i < _capacity; ++i) {
		HandlerEntry *entry = find(i);
		if (entry) entry->_handler->handle_close();
	}
	sdelete(_tuple);
}

int32 EpollReactor::HandlerRepository::open(uint32 max_handler_sz) {
	// TODO: set system handle limit
	// already opened.
	if (_tuple || _capacity || _size) return -1;

	_capacity = max_handler_sz;
	_tuple = new HandlerEntry[max_handler_sz];
	return 0;
}

int32 EpollReactor::HandlerRepository::add(IHandler *handler, int32 mask) {
	int32 hid = handler->get_handle();
	if (hid <= 0 || hid >= (int32)_capacity) return -1;

	if (find(hid)) {
		// LOG : repeated add
		return -1;
	}

	_tuple[hid]._handler = handler;
	_tuple[hid]._mask = mask;
	_tuple[hid]._suspend = true;
	++_size;

	return 0;
}

int32 EpollReactor::HandlerRepository::remove(int32 hid) {
	EpollReactor::HandlerEntry *tuple = find(hid);
	if (!tuple) {
		return -1;
	}

	tuple->_handler = NULL;
	tuple->_mask = 0;
	tuple->_suspend = false;
	--_size;

	return 0;
}

EpollReactor::HandlerEntry* EpollReactor::HandlerRepository::find(int32 hid) {
	if (hid <= 0 || hid >= (int32)_capacity) return NULL;
	return _tuple[hid]._handler ? &(_tuple[hid]) : NULL;
}

// HandlerRepository
////////////////////////////////////////////////////////////////////////////////
EpollReactor::EpollReactor() : _epoll_fd(-1), _active(false), _timer_queue(0) {}

EpollReactor::~EpollReactor() {
	close();
}

int32 EpollReactor::open(uint32 max_fd_sz, ITimerMgr *timer, SigMgr *sig) {
	int32 ret = _handler.open(max_fd_sz);
	if (ret) return ret;

	if (timer) {
		_timer_queue = timer;
	} else {
		_timer_queue = new TimerList();
	}

	_epoll_fd = epoll_create(max_fd_sz);
	if (_epoll_fd == -1) {
		return -1;
	}

	return 0;
}

void EpollReactor::close() {
	sclose(_epoll_fd);
}

int32 EpollReactor::handle_events(timet interval/* = 0ull*/) {
	if (!is_active()) return 0;

	epoll_event event;
	event.data.fd = -1;
	int32 ret = epoll_wait(_epoll_fd, &event, 1, (int32)interval);
	if (ret == -1 && errno != EINTR) {
		// TODO: error ocured.
		return -1;
	}

	if (event.data.fd > 0) {
		// have an io event.
		HandlerEntry *entry = _handler.find(event.data.fd);
		if (!entry || !entry->_handler) {
			// TODO: logic error.
			return -1;
		}

		IHandler *handler = entry->_handler;
		int32 mask = event.events;
		int32 (IHandler::*callback)() = 0;
		if (mask & EPOLLIN) {
			callback = &IHandler::handle_input;
		} else if (mask & EPOLLOUT) {
			callback = &IHandler::handle_output;
		} else if (mask & EPOLLHUP || mask & EPOLLERR) {
			callback = &IHandler::handle_error;
		}

		int32 status = 0;
		do { status = (handler->*callback)(); } while (status > 0);
		if (status < 0) {
			remove_handler(handler->get_handle());
		} else {
			resume_handler_impl(entry, EPOLL_CTL_MOD);
		}
	} else {
		// no io event happend, dispach a timer event.
		_timer_queue->expire_single();
	}

	return 0;
}

bool EpollReactor::is_active() {
	return _active;
}

void EpollReactor::deactive(bool active/* = false*/) {
	_active = active;
}

int32 EpollReactor::register_handler(IHandler *handler, int32 mask) {
	int32 emask = epoll_mask(mask);
	int32 ret = _handler.add(handler, emask);
	if (ret) return ret;

	epoll_event event;
	event.data.fd = handler->get_handle();
	event.events = emask | EPOLLERR | EPOLLET | EPOLLONESHOT;
	ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);

	return ret;
}

int32 EpollReactor::register_handler(int32 sig_num, IHandler *handler) {
	return -1;
}

int32 EpollReactor::remove_handler(int32 hid) {
	HandlerEntry *entry = _handler.find(hid);
	if (!entry) return -1;

	epoll_event event;
	event.data.fd = entry->_handler->get_handle();
	event.events = entry->_mask;
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);

	entry->_handler->handle_close();
	return _handler.remove(hid);
}

int32 EpollReactor::suspend_handler(int32 hid) {
	HandlerEntry *entry = _handler.find(hid);
	if (!entry || entry->_suspend) return -1;

	epoll_event event;
	event.data.fd = entry->_handler->get_handle();
	event.events = entry->_mask;
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);

	entry->_suspend = true;
	return 0;
}

int32 EpollReactor::resume_handler(int32 hid) {
	HandlerEntry *entry = _handler.find(hid);
	if (!entry) return -1;
	if (entry->_suspend == false) return -1;

	return resume_handler_impl(entry, EPOLL_CTL_ADD);
}

int32 EpollReactor::register_timer(IHandler *handler, timet start_time, timet interval) {
	return _timer_queue->add(start_time, interval, handler);
}

int32 EpollReactor::cancel_timer(int32 tid) {
	_timer_queue->remove(tid);
	return 0;
}

int32 EpollReactor::epoll_mask(int32 mask) {
	int32 ret = 0;
	if (mask & IHandler::HET_Read)
		ret |= EPOLLIN;
	if (mask & IHandler::HET_Write)
		ret |= EPOLLOUT;

	ret |= EPOLLERR;

	return ret;
}

int32 EpollReactor::resume_handler_impl(HandlerEntry *entry, int32 op) {
	epoll_event event;
	event.data.fd = entry->_handler->get_handle();
	event.events = entry->_mask | EPOLLERR | EPOLLET | EPOLLONESHOT;
	epoll_ctl(_epoll_fd, op, event.data.fd, &event);
	entry->_suspend = false;

	return 0;
}

NMS_END // end namespace kevent

