#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "./reactor/reactor.h"
#include "stringutil.h"
#include "netiohandler.h"
#include "session.h"
#include "sessionmgr.h"
#include "connector.h"

NMS_BEGIN(kcore)

Connector::~Connector() {
	if (_timer_id > 0) {
		reactor()->cancel_timer(_timer_id);
		_timer_id = 0;
	}

	while (!_conn_list.empty()) {
		Session *s = _conn_list.pop();
		s->mgr()->destroy(s);
	}

	_interval = 0;
	_cmd_handler = 0;
}

int32 Connector::open(uint32 interval, CommandHandler *cmd_handler) {
	_interval = interval;
	int32 timer_id = reactor()->register_timer(this, 0, interval);
	if (timer_id > 0) {
		_timer_id = timer_id;
		_cmd_handler = cmd_handler;
		return 0;
	} else {
		return -1;
	}
}

int32 Connector::connect(Session *s) {
	_conn_list.push(s);
	return 0;
}

int32 Connector::handle_timeout() {
	kevent::Reactor *poller = reactor();
	uint32 sz = _conn_list.size();
	while (sz -- > 0 && !_conn_list.empty()) {
		Session *s = _conn_list.pop();
		NetIoHandler *io_handler = s->io_handler();
		io_handler->reactor(poller);

		int32 ret = io_handler->reconnect();
		if (ret > 0) {
			// connect successfully
			io_handler->cmd_handler(_cmd_handler);
			s->id(io_handler->get_handle());
			s->mgr()->handle_new_connect(s);
		} else {
			_conn_list.push(s);
		}
	}

	return 0;
}

NMS_END // end namespace kcore

