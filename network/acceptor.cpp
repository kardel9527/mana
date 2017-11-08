#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include "reactor.h"
#include "netiohandler.h"
#include "netmgrbase.h"
#include "acceptor.h"

NMS_BEGIN(network)

int32 Acceptor::open() {
	int32 fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) return -1;

	// none block
	int32 ret = ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK);
	if (ret) return ret;

	// bind socket
	ret = ::bind(fd, (sockaddr *)_addr.addr(), sizeof(sockaddr));
	if (ret) {
		::close(fd);
		return ret;
	}

	// listen
	ret = ::listen(fd, 128);
	if (ret) {
		::close(fd);
		return ret;
	}

	// register to reactor
	handle(fd);
	ret = register_io_event();
	if (ret) {
		safe_close_handle(this);
		return ret;
	}

	return 0;
}

void Acceptor::close() {
	unregister_io_event();
}

int32 Acceptor::handle_input() {
	kcommon::InetAddr addr;
	socklen_t addr_len = sizeof(sockaddr);

	int32 fd = ::accept(handle(), (sockaddr *)addr.addr(), &addr_len);
	if (fd == -1) {
		return (errno == EAGAIN || errno == EWOULDBLOCK) ? 0 : -1;
	} else {
		// set none block
		::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK);

		// create a client session
		NetIoHandler *handler = reactor()->net_mgr()->get_handler(_type);
		if (!handler) {
			// no enough handler, may notice the net mgr ?
			::close(fd);
			return 1;
		}

		handler->addr(addr);
		handler->handle(fd);
		handler->reactor(reactor());

		// register
		if (handler->register_io_event()) {
			safe_close_handle(handler);
			reactor()->net_mgr()->put_handler(handler);
		} else {
			handler->active(true);
			handler->id(fd);
			reactor()->net_mgr()->handle_new_connect(handler);
		}

		return 1;
	}
}

int32 Acceptor::handle_close() {
	close();
	return 0;
}

NMS_END // end namespace network

