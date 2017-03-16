#include <sys/type.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "ireactor.h"
#include "netiohandler.h"
#include "acceptor.h"

NMS_BEGIN(kevent)

Acceptor::~Acceptor() {
	if (_sock > 0) {
		::close(_sock);
		_sock = -1;
	}
	_reactor = NULL;
}

int32 Acceptor::open(const char *addr, uint16 port) {
	_sock = ::socket(AF_INET, SOCK_STEAM, 0);
	if (_sock == -1) return -1;

	// none block
	int32 ret = ::fcntl(_sock, F_SETFL, fcntl(_sock, F_GETFL) | O_NONBLOCK);
	if (ret) return ret;

	// bind socket
	sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(addr);
	ret = ::bind(_sock, (sockaddr *)&saddr, sizeof(saddr));
	if (ret) return ret;

	// listen
	ret = ::listen(_sock, 128);
	if (ret) return ret;

	// register to reactor
	return reactor()->regist_handle(this, IHandler::HET_Read | IHandler::HET_Except);
}

int32 Acceptor::handle_input() {
	sockaddr_in addr;
	mmeset(&addr, 0, sizeof(addr));
	int32 addr_len = sizeof(addr);

	int32 fd = ::accept(_sock, &addr, &addr_len);
	if (fd == -1) {
		return (errno == EAGAIN || errno == EWOULDBLOCK) ? 0 : -1;
	} else {
		// set none block
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

		// get a handler and register into reactor.
		NetIoHandler *handler = new NetIoHandler();
		handler->set_handle(fd);
		handler->set_addr(inet_ntoa(addr.sin_addr.s_addr), addr.sin_port);
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

