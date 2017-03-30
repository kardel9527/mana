#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "ireactor.h"
#include "stringutil.h"
#include "netiohandler.h"
#include "session.h"
#include "connector.h"

NMS_BEGIN(kcommon)

Connector::~Connector() {
	if (_timer_id > 0) {
		reactor()->cancel_timer(_timer_id);
		_timer_id = 0;
	}

	_interval = 0;
	_conn_list.clear();
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
	_conn_list.write(s);
	_conn_list.unlock();
	
	return 0;
}

int32 Connector::handle_timeout() {
	Reactor *reactor = reactor();
	kcommon::AutoLock guard(_conn_list);

	uint32 size = _conn_list.avail();
	for (uint32 it = 0; i < size; ++i) {
		Session *s = 0;
		_conn_list.read(s);

		kcommon::NetIoHandler *io_handler = s->io_handler();
		io_handler->reactor(reactor);

		int32 ret = io_handler->reconnect();
		if (ret > 0) {
			// connect successfully
			
		} else {
			_conn_list.write(s);
		}
	}

	return 0;
}

NMS_END // end namespace kcommon

