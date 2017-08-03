#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "./reactor/reactor.h"
#include "stringutil.h"
#include "session.h"
#include "sessionmgr.h"
#include "commandhandler.h"
#include "netiohandler.h"

NMS_BEGIN(kcore)
NetIoHandler::~NetIoHandler() {
	sclose(_fd);
	_active = false;
	_session = 0;
	_cmd_handler = 0;
}

int32 NetIoHandler::handle_input() {
	if (!is_active()) return -1;

	assert(_recv_helper.ptr() && _recv_helper.len());

	int32 n = ::recv(_fd, _recv_helper.ptr(), _recv_helper.len(), 0);

	// connection was disconnected by peer.
	if (n == 0) return -1;
	// error curred, see if would block
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;

	// write the recved n bytes into the recv buffer.
	if (_recv_helper.complete(n)) {
		// an error occured while parse the length of the packet.
		if (!_recv_helper._ptr) return -1;

		_session->on_recv(_recv_helper._ptr);

		_recv_helper.reset();
	}

	return 1;
}

int32 NetIoHandler::handle_output() {
	kcommon::AutoLock<LockType> guard(_snd_buff);
	return send_impl();
}

int32 NetIoHandler::handle_close() {
	_active = false;

	// notice session manager.
	_session->mgr()->handle_disconnect(_fd);
	sclose(_fd);

	return 0;
}

int32 NetIoHandler::send(const char *data, uint32 len, bool immediately/* = true*/) {
	bool err = false;
	bool snd_ordered = false;
	_snd_buff.lock();
	if (_snd_buff.avail() == 0) {
		// send directly if no data in send cache.(this may be normal situation)
		int32 status = send_immediate(data, len);
		if (status >= 0 && (len - status) > 0) {
			// ok, not send all.
			_snd_buff.write(data + status, len - status);
			snd_ordered = true;
		}
		err = (status < 0);
	} else {
		// put in send cache, and send to the net through the poller.(this may be slow than directly send.)
		_snd_buff.write(data, len);
		snd_ordered = true;
	}
	_snd_buff.unlock();

	if (err) {
		_cmd_handler->add(CommandHandler::Command(CommandHandler::CT_DISCONNECT, get_handle()));
	} else if (snd_ordered) {
		_cmd_handler->add(CommandHandler::Command(CommandHandler::CT_SEND_ORDERED, get_handle()));
	}

	return 0;
}

int32 NetIoHandler::disconnect() {
	_active = false;
	_cmd_handler->add(CommandHandler::Command(CommandHandler::CT_DISCONNECT, get_handle()));
	return 0;
}

int32 NetIoHandler::reconnect() {
	if (is_active()) return 0;

	if (_fd == - 1) {
		_fd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (_fd == -1) return -1;

		// set none block
		fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
	}

	socklen_t addr_len = sizeof(sockaddr);
	int32 ret = ::connect(_fd, (sockaddr *)_inet_addr.addr(), addr_len);
	if (ret == 0) {
		_active = true;
		int32 ret = reactor()->register_handler(this, IHandler::HET_Read /*| IHandler::HET_Write*/ | IHandler::HET_Except);
		return ret == 0 ? 1 : -1;
	} else if (ret == -1) {
		return errno == EINPROGRESS ? 0 : -1;
	}

	return 0;
}

bool NetIoHandler::is_active() {
	return _active;
}

int32 NetIoHandler::send_impl() {
	if (_snd_buff.avail() == 0) return 0;

	assert(_snd_buff.rptr() && _snd_buff.rsize());

	int32 n = ::send(_fd, _snd_buff.rptr(), _snd_buff.rsize(), 0);
	// connection was disconnected by peer.
	if (n == 0) return -1;
	// error curred, see if would block
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;

	// ok, send n bytes.
	_snd_buff.rmove(n);

	return 1;
}

int32 NetIoHandler::send_immediate(const char *data, uint32 len) {
	int32 n = ::send(_fd, data, len, 0);
	// connection was disconnected by peer.
	if (n == 0) return -1;
	// error curred, see if would block
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;

	// ok, send n bytes.
	return n;
}

NMS_END // end namespace kcommon

