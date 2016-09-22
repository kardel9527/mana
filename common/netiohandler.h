#ifndef __NET_IO_HANDLER_H_
#define __NET_IO_HANDLER_H_
#include "macros.h"
#include "udt.h"
#include "ihandler.h"
#include "ringbuffer.h"
#include "inetaddr.h"
#include "lock.h"

NMS_BEGIN(kcommon)
class Session;

class NetIoHandler : public kevent::IHandler {
public:
	NetIoHandler() : _fd(-1), _active(false), _session(0) {}
	~NetIoHandler();

	void redirect(int32 fd, const kcommon::InetAddr &addr) { _active = fd > 0; _fd = fd; _inet_addr = addr; }

	void session(kcommon::Session *session) { _session = session; }

	virtual int32 get_handle() { return _fd; }

	virtual void set_handle(int32 hid) { _fd = hid; }

	virtual int32 handle_input();

	virtual int32 handle_output();

	virtual int32 handle_close();

	int32 send(const char *data, uint32 len, bool immediately = true);

	int32 disconnect();

	int32 reconnect();

	bool is_active();

private:
	int32 send_impl();

private:
	int32 _fd;
	bool _active;
	Session *_session;
	kcommon::InetAddr _inet_addr;
	RingBuffer<byte> _snd_buff;
	RingBuffer<byte, kcommon::DefaultLock> _rcv_buff;
};

NMS_END // end namespace kcommon

#endif // __NET_IO_HANDLER_H_

