#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "ireactor.h"
#include "stringutil.h"
#include "netiohandler.h"

NMS_BEGIN(kevent)
NetIoHandler::~NetIoHandler() {
	if (_fd > 0) {
		::close(_fd);
		_fd = -1;
	}
}

int32 NetIoHandler::handle_input() {
	// recv 2048 bytes one time, and reactor will call it until nothing to recv.
	char buff[2048] = { 0 };
	int32 n = ::recv(_fd, buff, sizeof(buff), 0);
	
	// connection was disconnected by peer.
	if (n == 0) return -1;
	// error curred, see if would block
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;

	// TODO: parse the packet here?
	// write the recved n bytes into the recv buffer.
	_rcv_buff.lock();
	_rcv_buff.write(buff, n);
	_rcv_buff.unlock();

	return 1;
}

int32 NetIoHandler::handle_output() {
	AutoLock guard(_snd_buff);
	return send_impl();
}

int32 NetIoHandler::send(const char *data, uint32 len, bool immediately/* = true*/) {
	_snd_buff.lock();
	_snd_buff.write(data, len);
	_snd_buff.unlock();
	return immediately ? handle_output() : 0;
}

int32 NetIoHandler::disconnect() {
	//reactor()->remove_handler(get_handle());

	// give up data in the rcv buff.
	_rcv_buffer.reset();
	// try send all content in the send buff.
	while (send_impl() > 0) ;

	::close(_fd);
	_fd = -1;
	_active = false;

	return 0;
}

int32 NetIoHandler::reconnect() {
	if (_fd > 0) return 0;

	_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1) return -1;

	// set none block
	fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);

	socklen_t addr_len = sizeof(sockaddr);
	int32 ret = ::connect(_fd, (sockaddr *)_inet_addr.addr(), addr_len);
	if (ret == 0) {
		_active = true;
		return reactor()->register_handler(this, IHandler::HET_Read | IHandler::HET_Write | IHandler::HET_Except);
	} else if (ret == -1) {
		return errno == EINPROGRESS ? 0 : -1;
	}
	
	return 0;
}

bool NetIoHandler::is_active() {
	return _active;
}

int32 NetIoHandler::send_impl() {
	// try send 2 kbytes one time.
	char buff[1024 * 2] = { 0 };
	int32 len = _snd_buff.peek(buff, sizeof(buff));
	if (len <= 0) return 0;

	int32 n = ::send(_fd, buff, len, 0);
	// connection was disconnected by peer.
	if (n == 0) return -1;
	// error curred, see if would block
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;

	// ok, send n bytes.
	_snd_buff.rd_move(n);

	return 1;
}

NMS_END // end namespace kevent

