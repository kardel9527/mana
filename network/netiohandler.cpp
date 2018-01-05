#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "reactor.h"
#include "stringutil.h"
#include "netmgrbase.h"
#include "commandhandler.h"
#include "netiohandler.h"

NMS_BEGIN(network)

NetIoHandler::~NetIoHandler() {
	safe_close_handle(this);

	_type = NHLT_MAX;
	_active = false;
}

int32 NetIoHandler::handle_input() {
	if (!active()) return -1;

	assert(_recv_helper.ptr() && _recv_helper.len() && "receive param error.");

	int32 n = ::recv(handle(), _recv_helper.ptr(), _recv_helper.len(), 0);

	// connection was disconnected by peer.
	if (n == 0) return -1;
	// error curred, see if would block
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;

	// write the recved n bytes into the recv buffer.
	if (_recv_helper.complete(n)) {
		// an error occured while parse the length of the packet.
		if (_recv_helper.bad_packet()) return -1;

		_recved_packet.lock();
		_recved_packet.write(_recv_helper._ptr);
		_recved_packet.unlock();

		_recv_helper.reset();
	}

	return 1;
}

int32 NetIoHandler::handle_output() {
	int32 ret = 0;

	_snd_buff.lock();
	while (_snd_buff.avail()) {
		assert(_snd_buff.rptr() && _snd_buff.rsize() && "ring buffer logic error!");
		int32 n = send_immediate(_snd_buff.rptr(), _snd_buff.rsize());
		if (n <= 0) {
			ret = -1;
			break;
		} else {
			_snd_buff.rmove(n);
		}
	}
	_snd_buff.unlock();
	
	return ret;
}

int32 NetIoHandler::handle_close() {
	if (!active()) return 0;
	
	active(false);

	// release the uncomplete packet if.
	_recv_helper.release();

	// notice net mgr.
	reactor()->net_mgr()->handle_disconnect(this);

	// close the handle.
	safe_close_handle(this);

	return 0;
}

int32 NetIoHandler::send(const char *data, uint32 len, bool immediately/* = true*/) {
	if (!active()) return 0;

	bool err = false;
	bool snd_ordered = false;
	_snd_buff.lock();
	if (_snd_buff.avail() == 0) {
		// send directly if no data in send cache.(this may be normal situation)
		_snd_buff.unlock();
		int32 status = send_immediate(data, len);
		if (status >= 0 && (len - status) > 0) {
			// partially sended.
			_snd_buff.lock();
			_snd_buff.write(data + status, len - status);
			_snd_buff.unlock();
			snd_ordered = true;
		}
		err = (status < 0);
	} else {
		// put in send cache, and send to the net through the poller.(this may be slow than directly send.)
		_snd_buff.write(data, len);
		_snd_buff.unlock();
		snd_ordered = true;
	}
	_snd_buff.unlock();

	if (err) {
		reactor()->add_cmd(CommandHandler::CT_DISCONNECT, this);
	} else if (snd_ordered) {
		reactor()->add_cmd(CommandHandler::CT_SEND_ORDERED, this);
	}

	return 0;
}

int32 NetIoHandler::kickoff() {
	if (active()) {
		reactor()->add_cmd(CommandHandler::CT_DISCONNECT, this);
	}

	return 0;
}

int32 NetIoHandler::reconnect() {
	if (active()) return 0;

	if (handle() == invalid_handle) {
		int32 fd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1) return -1;

		// set none block
		::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK);

		handle(fd);
	}

	socklen_t addr_len = sizeof(sockaddr);
	int32 ret = ::connect(handle(), (sockaddr *)_inet_addr.addr(), addr_len);
	if (ret == 0) {
		if (register_io_event()) {
			safe_close_handle(this);
			return -1;
		} else {
			active(true);
			return 0;
		}
	} else if (ret == -1) {
		return -1;
	}

	return 0;
}

void NetIoHandler::update_base() {
	// handle max 64 packets a round.
	char *packet[64] = { 0 };
	_recved_packet.lock();
	int32 ret = _recved_packet.read((char **)packet, sizeof(packet));
	_recved_packet.unlock();

	for (int i = 0; i < ret; ++i) {
		handle_packet((char *)packet[i]);
		sfree(packet[i]);
	}
}

int32 NetIoHandler::send_immediate(const char *data, uint32 len) {
	int32 n = ::send(handle(), data, len, 0);
	// connection was disconnected by peer.
	if (n == 0) return -1;
	// error curred, see if would block
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;

	// ok, send n bytes.
	return n;
}

NMS_END // end namespace network.

