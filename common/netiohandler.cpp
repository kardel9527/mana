#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "reactor.h"
#include "bytebuffer.h"
#include "stringutil.h"
#include "session.h"
#include "sessionmgr.h"
#include "netiohandler.h"

NMS_BEGIN(kcommon)
NetIoHandler::~NetIoHandler() {
	sclose(_fd);
}

int32 NetIoHandler::handle_input() {
	if (!is_active()) return -1;

	// recv 2048 bytes one time, and reactor will call it until nothing to recv.
	char buff[2048] = { 0 };
	int32 n = ::recv(_fd, buff, sizeof(buff), 0);
	
	// connection was disconnected by peer.
	if (n == 0) return -1;
	// error curred, see if would block
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;

	// TODO: parse the packet here?
	// write the recved n bytes into the recv buffer.
	_rcv_buff.write(buff, n);

	// if recv a full packet, put it into the session.
	uint32 packet_size = 0;
	_rcv_buff.peek((char *)&packet_size, sizeof(packet_size));
	while (_rcv_buff.avail() >= (packet_size + sizeof(packet_size))) {
		// TODO: avoid alloc and copy?
		kcommon::ReadBuffer *buffer = new kcommon::ReadBuffer();
		buffer->resize(packet_size);
		_rcv_buff.rd_move(sizeof(packet_size));

		_rcv_buff.read(buffer->wr_ptr(), packet_size);
		buffer->wr_move(packet_size);
		_session->on_recv(buffer);

		packet_size = 0;
		_rcv_buff.peek((char *)&packet_size, sizeof(packet_size));
	}

	return 1;
}

int32 NetIoHandler::handle_output() {
	AutoLock<LockType> guard(_snd_buff);
	return send_impl();
}

int32 NetIoHandler::handle_close() {
	_active = false;
	// send left data in the buff.
	while (send_impl() > 0) ;

	// notice session manager.
	_session->mgr()->handle_disconnect(_fd);

	sclose(_fd);

	return 0;
}

int32 NetIoHandler::send(const char *data, uint32 len, bool immediately/* = true*/) {
	_snd_buff.lock();
	_snd_buff.write(data, len);
	// send immediately
	while (send_impl() > 0) ;
	_snd_buff.unlock();
	//return immediately ? handle_output() : 0;
}

int32 NetIoHandler::disconnect() {
	_active = false;
	::shutdown(_fd, SHUT_RDWR);
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

NMS_END // end namespace kcommon

