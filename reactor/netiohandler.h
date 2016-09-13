#ifndef __NET_IO_HANDLER_H_
#define __NET_IO_HANDLER_H_
#include "macros.h"
#include "udt.h"
#include "ihandler.h"

NMS_BEGIN(kevent)

class NetIoHandler : public IHandler {
public:
	NetIoHandler() : _fd(-1), _port(0) {}
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
	char _addr[64];
};

NMS_END // end namespace kevent

#endif // __NET_IO_HANDLER_H_

