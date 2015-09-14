#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "inetaddr.h"
#include "ireactor.h"
#include "netiohandler.h"
#include "acceptor.h"

NMS_BEGIN(kevent)

Acceptor::~Acceptor() {
	if (_sock > 0) {
		::close(_sock);
		_sock = -1;
	}
}

int32 Acceptor::open(const char *addr, uint16 port) {
	_sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == -1) return -1;

	// none block
	int32 ret = ::fcntl(_sock, F_SETFL, fcntl(_sock, F_GETFL) | O_NONBLOCK);
	if (ret) return ret;

	// bind socket
	InetAddr inet_addr(addr, port);
	ret = ::bind(_sock, (sockaddr *)inet_addr.addr(), sizeof(sockaddr));
	if (ret) return ret;

	// listen
	ret = ::listen(_sock, 128);
	if (ret) return ret;

	// register to reactor
	return reactor()->register_handler(this, IHandler::HET_Read | IHandler::HET_Except);
}

int32 Acceptor::handle_input() {
	InetAddr addr;
	socklen_t addr_len = sizeof(sockaddr);

	int32 fd = ::accept(_sock, (sockaddr *)addr.addr(), &addr_len);
	if (fd == -1) {
		return (errno == EAGAIN || errno == EWOULDBLOCK) ? 0 : -1;
	} else {
		// set none block
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

		// get a handler and register into reactor.
		NetIoHandler *handler = new NetIoHandler();
		handler->redirect(fd, addr);
		handler->reactor(reactor());

		// TODO: when register failed?
		return reactor()->register_handler(handler, IHandler::HET_Read | IHandler::HET_Write | IHandler::HET_Except);
	}
}

int32 Acceptor::handle_close() {
	if (_sock > 0) {
		::close(_sock);
		_sock = -1;
	}
	return 0;
}

NMS_END // end namespace kevent

