#ifndef __NET_IO_HANDLER_H_
#define __NET_IO_HANDLER_H_
#include "macros.h"
#include "udt.h"
#include "ihandler.h"
#include "ringbuffer.h"
#include "inetaddr.h"
#include "lock.h"

NMS_BEGIN(kevent)

class NetIoHandler : public IHandler {
public:
	NetIoHandler() : _fd(-1), _active(true) {}
	~NetIoHandler();

	void redirect(int32 fd, const InetAddr &addr) { _fd = fd; _inet_addr = addr; }

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
	bool _active;
	InetAddr _inet_addr;
	RingBuffer _snd_buff;
	kcommon::Mutex _snd_lock;
	RingBuffer _rcv_buff;
	kcommon::Mutex _rcv_lock;
};

NMS_END // end namespace kevent

#endif // __NET_IO_HANDLER_H_

