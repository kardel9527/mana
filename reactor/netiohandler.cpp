#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "ireactor.h"
#include "stringutil.h"
#include "netiohandler.h"

NMS_BEGIN(kevent)

void NetIoHandler::set_addr(const char *addr, uint16 port) {
	_port = port;
	kcommon::stringutil::smemcpy(_addr, addr, sizeof(_addr));
}

int32 NetIoHandler::handle_input() {
	char buff[2048] = { 0 };
	int32 n = ::recv(_fd, buff, sizeof(buff), 0);
	if (n == 0) return -1;
	if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;
	
	_rcv_lock.lock();
	_rcv_buff.write(buff, n);
	_rcv_lock.unlock();
	return 1;
}

int32 NetIoHandler::handle_output() {
	kcommon::AutoLock<kcommon::Mutex> guard(_snd_lock);
	while (_snd_buff.avail() > 0) {
		int32 n = ::send(_fd, _snd_buff.rd_ptr(), _snd_buff.avail(), 0);
		if (n == 0) return -1;
		if (n < 0) return (errno != EAGAIN && errno != EWOULDBLOCK) ? -1 : 0;
		_snd_buff.rd_move(n);
	}
	return 0;
}

int32 NetIoHandler::send(const char *data, uint32 len, bool immediately/* = true*/) {
	_snd_lock.lock();
	_snd_buff.write(data, len);
	_snd_lock.unlock();
	return immediately ? handle_output() : 0;
}

int32 NetIoHandler::disconnect() {
	// TODO: how to handle the data left in recv buff and snd buff.
	reactor()->remove_handler(get_handle());
	_active = false;
	return 0;
}

int32 NetIoHandler::reconnect() {
	sockaddr_in addr;
	::memset(&addr, 0, sizeof(addr));
	socklen_t addr_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_addr);

	int32 ret = ::connect(_fd, (sockaddr *)&addr, addr_len);
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

NMS_END // end namespace kevent

