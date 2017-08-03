#ifndef __NET_IO_HANDLER_H_
#define __NET_IO_HANDLER_H_
#include "macros.h"
#include "udt.h"
#include "./reactor/ihandler.h"
#include "ringbuffer.h"
#include "inetaddr.h"
#include "lock.h"

NMS_BEGIN(kcore)
class Session;
class CommandHandler;

class NetIoHandler : public kevent::IHandler {
	struct RecvHelper {
		char *_ptr;
		uint32 _size;
		uint32 _len;

		RecvHelper() { reset(); }
		~RecvHelper() { reset(); }

		void reset() { _ptr = 0; _size = 0; _len = 0; }

		void* ptr() { return _len < sizeof(_size) ? (void *)((char *)&_size + _len) : _ptr + _len; }

		uint32 len() { return _len < sizeof(_size) ? (sizeof(_size) - _len) : (_size - _len); }

		bool complete(uint32 len) {
			_len += len;
			if (_len == _size && _size > 0) return true;
			if (_len == sizeof(_size) && !_ptr) {
				uint32 size = _size;//::ntohl(_size);
				if (size == 0) return true;
				if (size >= 65535 * 100) return true;

				_ptr = (char *)::malloc(size);
				*(uint32 *)_ptr = _size;
				_size = size;
				return false;
			}
			return false;
		}
	};


public:
	NetIoHandler() : _fd(-1), _active(false), _snd_timer(-1), _session(0), _cmd_handler(0) {}
	~NetIoHandler();

	void redirect(int32 fd, const kcommon::InetAddr &addr) { _active = fd > 0; _fd = fd; _inet_addr = addr; }

	void session(Session *session) { _session = session; }

	void cmd_handler(CommandHandler *cmd_handler) { _cmd_handler = cmd_handler; }

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

	int32 send_immediate(const char *data, uint32 len);

private:
	int32 _fd;
	bool _active;
	int32 _snd_timer;
	Session *_session;
	CommandHandler *_cmd_handler;
	kcommon::InetAddr _inet_addr;
	kcommon::RingBuffer<byte> _snd_buff;
	RecvHelper _recv_helper;
};

NMS_END // end namespace kcore

#endif // __NET_IO_HANDLER_H_

