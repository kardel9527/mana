#ifndef __NET_IO_HANDLER_H_
#define __NET_IO_HANDLER_H_
#include "macros.h"
#include "udt.h"
#include "ihandler.h"
#include "buffer.h"

NMS_BEGIN(kevent)

class NetIoHandler : public IHandler {
public:
	NetIoHandler() : _fd(-1), _port(0), _recv_buff(2048), _send_buff(2048) {}
	~NetIoHandler();

	void set_addr(const char *addr, uint16 port);

	virtual int32 get_handle() { return _fd; }

	virtual void set_handle(int32 hid) { _fd = hid; }

	virtual int32 handle_input();

	virtual int32 handle_output();

	int32 send(const char *data, uint32 len, bool immediately = true);

	int32 disconnect();

	int32 reconnect();

	bool is_active();

private:
	int32 _fd;
	uint16 _port;
	const char *_addr[64];
	Mutex _recv_lock;
	NetBuffer _recv_buff;
	Mutex _send_lock;
	NetBuffer _send_buff;
};

NMS_END // end namespace kevent

#endif // __NET_IO_HANDLER_H_

