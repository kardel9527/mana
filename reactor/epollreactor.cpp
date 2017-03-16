#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "ihandler.h"
#include "imerlist.h"
#include "epollreactor.h"

NMS_BEGIN(kevent)
// EPollReactor::HandlerRepository
////////////////////////////////////////////////////////////////////////////////
EPollReactor::HandlerRepository::~HandlerRepository() {
	_size = 0;
	_max_size = 0;
	SAFE_DELETE(_tuple);
}
int32 EPollReactor::HandlerRepository::open(uint32 max_handler_sz) {
	// TODO: set system handle limit
	// already opened.
	if (_tuple || _max_size || _size) return -1;

	_max_size = max_handler_sz;
	_tuple = new HandlerEntry[max_handler_sz];
	return 0;
}

int32 EPollReactor::HandlerRepository::add(IHandler *hanlder, int32 mask) {
	int32 hid = handler->get_hid();
	if (hid <= 0 || hid >= _max_size) return -1;

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

int32 EPollReactor::HandlerRepository::remove(int32 hid) {
	EPollReactor::HandlerEntry *tuple = find(hid);
	if (!tuple) {
		return -1;
	}

	tuple->_handler = NULL;
	tuple->_mask = 0;
	tuple->_suspend = false;
	--_size;

	return 0;
}

EPollReactor::HandlerEntry* EPollReactor::HandlerRepository::find(int32 hid) {
	return (hid <= 0 || hid >= _max_size) ? NULL : &(_tuple[hid]);
}

// HandlerRepository
////////////////////////////////////////////////////////////////////////////////
EPollReactor::EPollReactor() : _epoll_fd(-1), _active(false), _timer_queue(0) {}

EPollReactor::~EPollReactor() {
	close();
}

int32 EPollReactor::open(uint32 max_fd_sz, kcommon::ITimerMgr *timer, kcommon::SigMgr *sig) {
	int32 ret = _handler.open(max_fd_sz);
	if (ret) return ret;

	if (timer) {
		_timer_queue = timer;
	} else {
		_timer_queue = new kcommon::TimerList();
	}

	_epoll_fd = epoll_create(max_fd_sz);
	if (_epoll_fd == -1) {
		return -1;
	}

	return 0;
}

void EPollReactor::close() {
	if (_epoll_fd != -1) {
		::close(_epoll_fd);
		_epoll_fd = -1;
	}
}

int32 EPollReactor::handle_events(timeval interval = 0ull) {
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
			remove_handler(handler->get_hid());
		} else {
			resume_handler(handler->get_hid());
		}
	} else {
		// no io event happend, dispach a timer event.
		_timer_queue->expire_single();
	}

	return 0;
}

bool EPollReactor::is_active() {
	return _active;
}

void EPollReactor::deactive(bool active/* = false*/) {
	_active = active;
}

int32 EPollReactor::register_handler(IHandler *handler, int32 mask) {
	int32 epoll_mask = transform_mask(mask);
	int32 ret = _handler.add(handler, epoll_mask);
	if (ret) return ret;

	epoll_event event;
	event.data.fd = handler->get_hid();
	event.events = epoll_mask | EPOLLERR | EPOLLET | EPOLLONESHOT;
	epoll_ctl(_epoll_fd, event.data.fd, EPOLL_CTL_ADD, &event);

	return 0;
}

int32 EPollReactor::register_handler(int32 sig_num, IHandler *handler) {
	return -1;
}

int32 EPollReactor::remove_handler(int32 hid) {
	HandlerEntry *entry = _handler.find(hid);
	if (!entry) return -1;

	epoll_event event;
	event.data.fd = entry->_handler->get_hid();
	event.events = entry->_mask;
	epoll_ctl(_epoll_fd, event.data.fd, EPOLL_CTL_DEL, &event);

	entry->_handler->handle_close();
	return _handler.remove(hid);
}

int32 EPollReactor::suspend_handler(int32 hid) {
	HandlerEntry *entry = _handler.find(hid);
	if (!entry || entry->_suspend) return -1;

	epoll_event event;
	event.data.fd = entry->_handler->get_hid();
	event.events = entry->_mask;
	epoll_ctl(_epoll_fd, event.data.fd, EPOLL_CTL_DEL, &event);

	entry->_suspend = true;
	return 0;
}

int32 EPollReactor::resume_handler(int32 hid) {
	HandlerEntry *entry = _handler.find(hid);
	if (!entry) return -1;
	if (entry->_suspend == false) return -1;

	epoll_event event;
	event.data.fd = entry->_handler->get_hid();
	event.events = entry->_mask | EPOLLERR | EPOLLET | EPOLLONESHOT;
	epoll_ctl(_epoll_fd, event.data.fd, EPOLL_CTL_ADD, &event);

	entry->_suspend = false;
	return 0;
}

int32 EPollReactor::register_timer(IHandler *handler, timeval start_time, timeval interval) {
	return _timer_queue->add(start_time, interval, handler);
}

int32 EPollReactor::cancel_timer(int32 tid) {
	_timer_queue->remove(tid);
	return 0;
}

int32 EpollReactor::epoll_mask(int32 mask) {
	int32 ret = 0;
	switch (mask) {
	case IHandler::HET_Read:
		ret = EPOLLIN;
		break;
	case IHandler::HET_Write:
		ret = EPOLLOUT;
		break;
	case IHandler:HET_Except:
		ret = EPOLLERR;
		break;
	default:
		break;
	}
	
	return ret;
}

NMS_END // end namespace kevent
