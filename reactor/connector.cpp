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
#include "connector.h"

NMS_BEGIN(kevent)
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

int32 Connector::connect(const InetAddr &addr) {
	int32 sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) return -1;

	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
	
	ConnectInfo conn(sock, addr);
	int32 ret = connect(conn);
	if (ret == -1) return -1;
	if (ret == 1) return 0;

	_conn_list.push_back(conn);
	return 0;
}

int32 Connector::handle_timeout() {
	for (ToBeConnectList::iterator it = _conn_list.begin(); it != _conn_list.end(); ) {
		int32 ret = connect(*it);
		if (ret == 1) {
			it = _conn_list.erase(it);
		} else {
			it ++;
		}
	}

	return 0;
}

int32 Connector::connect(const ConnectInfo &conn) {
	socklen_t addr_len = sizeof(sockaddr);
	int32 ret = ::connect(conn._fd, (const sockaddr *)conn._inet_addr.addr(), addr_len);
	if (ret == 0) {
		NetIoHandler *handler = new NetIoHandler();
		handler->redirect(conn._fd, conn._inet_addr);
		handler->reactor(reactor());
		reactor()->register_handler(handler, IHandler::HET_Read | IHandler::HET_Write | IHandler::HET_Except);
		return 1;
	} else if (ret == -1) {
		return errno == EINPROGRESS ? 0 : -1;
	}

	return 0;
}

NMS_END // end namespace kevent

