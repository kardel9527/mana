#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "reactor.h"
#include "stringutil.h"
#include "netiohandler.h"
#include "session.h"
#include "sessionmgr.h"
#include "connector.h"

NMS_BEGIN(kcommon)

Connector::~Connector() {
	if (_timer_id > 0) {
		reactor()->cancel_timer(_timer_id);
		_timer_id = 0;
	}

	kcommon::AutoLock<LockType> guard(_conn_list);
	while (!_conn_list.empty()) {
		Session *s = _conn_list.pop();
		s->mgr()->destroy(s);
	}

	_interval = 0;
}

int32 Connector::open(uint32 interval) {
	_interval = interval;
	int32 timer_id = reactor()->register_timer(this, 0, interval);
	if (timer_id > 0) {
		_timer_id = timer_id;
		return 0;
	} else {
		return -1;
	}
}

int32 Connector::connect(Session *s) {
	_conn_list.lock();
	_conn_list.push(s);
	_conn_list.unlock();
	return 0;
}

int32 Connector::handle_timeout() {
	kevent::Reactor *poller = reactor();
	kcommon::AutoLock<LockType> guard(_conn_list);
	while (!_conn_list.empty()) {
		Session *s = _conn_list.pop();
		kcommon::NetIoHandler *io_handler = s->io_handler();
		io_handler->reactor(poller);

		int32 ret = io_handler->reconnect();
		if (ret == 0) {
			// connect successfully
			s->mgr()->handle_new_connect(s);
		} else {
			_conn_list.push(s);
		}
	}

	return 0;
}

NMS_END // end namespace kcommon

