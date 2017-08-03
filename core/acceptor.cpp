#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "./reactor/reactor.h"
#include "netiohandler.h"
#include "session.h"
#include "sessionmgr.h"
#include "acceptor.h"

NMS_BEGIN(kcore)

Acceptor::~Acceptor() {
	sclose(_listen_fd);
	_cmd_handler = 0;
	_session_mgr = 0;
}

int32 Acceptor::open(const kcommon::InetAddr &addr, CommandHandler *cmd_handler) {
	_listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd == -1) return -1;

	// none block
	int32 ret = ::fcntl(_listen_fd, F_SETFL, fcntl(_listen_fd, F_GETFL) | O_NONBLOCK);
	if (ret) return ret;

	// bind socket
	ret = ::bind(_listen_fd, (sockaddr *)addr.addr(), sizeof(sockaddr));
	if (ret) return ret;

	// listen
	ret = ::listen(_listen_fd, 128);
	if (ret) return ret;

	// register to reactor
	ret = reactor()->register_handler(this, IHandler::HET_Read | IHandler::HET_Except);
	if (ret) return ret;

	_cmd_handler = cmd_handler;

	return 0;
}

int32 Acceptor::handle_input() {
	kcommon::InetAddr addr;
	socklen_t addr_len = sizeof(sockaddr);

	int32 fd = ::accept(_listen_fd, (sockaddr *)addr.addr(), &addr_len);
	if (fd == -1) {
		return (errno == EAGAIN || errno == EWOULDBLOCK) ? 0 : -1;
	} else {
		// set none block
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

		// create a client session
		Session *s = _session_mgr->create(1);
		s->mgr(_session_mgr);
		NetIoHandler *handler = s->io_handler();
		handler->redirect(fd, addr);
		handler->reactor(reactor());
		handler->cmd_handler(_cmd_handler);
		s->id(handler->get_handle());

		// register
		if (reactor()->register_handler(handler, IHandler::HET_Read /*| IHandler::HET_Write */| IHandler::HET_Except) != 0) {
			_session_mgr->destroy(s);
		} else {
			_session_mgr->handle_new_connect(s);
		}
		return 1;
	}
}

int32 Acceptor::handle_close() {
	sclose(_listen_fd);
	return 0;
}

NMS_END // end namespace kcore

