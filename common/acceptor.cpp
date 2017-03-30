#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "ireactor.h"
#include "netiohandler.h"
#include "session.h"
#include "sessionmgr.h"
#include "acceptor.h"

NMS_BEGIN(kcommon)

Acceptor::~Acceptor() {
	sclose(_sock);
}

int32 Acceptor::open(const kcommon::InetAddr &addr) {
	_sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == -1) return -1;

	// none block
	int32 ret = ::fcntl(_sock, F_SETFL, fcntl(_sock, F_GETFL) | O_NONBLOCK);
	if (ret) return ret;

	// bind socket
	ret = ::bind(_sock, (sockaddr *)addr.addr(), sizeof(sockaddr));
	if (ret) return ret;

	// listen
	ret = ::listen(_sock, 128);
	if (ret) return ret;

	// register to reactor
	return reactor()->register_handler(this, IHandler::HET_Read | IHandler::HET_Except);
}

int32 Acceptor::handle_input() {
	kcommon::InetAddr addr;
	socklen_t addr_len = sizeof(sockaddr);

	int32 fd = ::accept(_sock, (sockaddr *)addr.addr(), &addr_len);
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

		// register
		if (reactor()->register_handler(handler, IHandler::HET_Read | IHandler::HET_Write | IHandler::HET_Except) != 0) {
			_session_mgr->destroy(s);
		} else {
			_session_mgr->handle_new_connect(s);
		}
	}
}

int32 Acceptor::handle_close() {
	sclose(_sock);
	return 0;
}

NMS_END // end namespace kcommon

