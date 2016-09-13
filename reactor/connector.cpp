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
#include "connector.h"
#include "netiohandler.h"

NMS_BEGIN(kevent)

int32 Connector::open() {
	int32 ret = connect();
	if (ret < 0) {
		return ret;
	} else if (ret > 0) {
		return 0;
	} else {
		_timer_id = reactor()->register_timer(this, 0, _interval);
		if (_timer_id <= 0) return -1;
	}

	return 0;
}

void Connector::set_addr(const char *addr, uint16 port) {
	_port = port;
	kcommon::stringutil::smemcpy(_addr, addr, sizeof(_addr));
}

int32 Connector::handle_timeout() {
	_sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == -1) return -1;

	fcntl(_sock, F_SETFL, fcntl(_sock, F_GETFL) | O_NONBLOCK);

	int32 ret = connect();
	if (ret < 0) {
		return ret;
	} else if (ret > 0) {
		return 0;
	} else {
		_timer_id = reactor()->register_timer(this, 0, _interval);
		if (_timer_id <= 0) return -1;
	}

	return 0;
}

int32 Connector::connect() {
	sockaddr_in addr;
	::memset(&addr, 0, sizeof(addr));
	socklen_t addr_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_addr);

	int32 ret = ::connect(_sock, (sockaddr *)&addr, addr_len);
	if (ret == 0) {
		// new handler.
		if (_timer_id > 0) {
			reactor()->cancel_timer(_timer_id);
		}

		NetIoHandler *handler = new NetIoHandler();
		handler->set_addr(_addr, _port);
		handler->set_handle(_sock);
		handler->reactor(reactor());

		return reactor()->register_handler(handler, IHandler::HET_Read | IHandler::HET_Write | IHandler::HET_Except);
	} else if (ret == -1) {
		return errno == EINPROGRESS ? 0 : -1;
	}

	return 0;
}

NMS_END // end namespace kevent

