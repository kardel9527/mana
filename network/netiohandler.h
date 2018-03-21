#ifndef __NET_IO_HANDLER_H_
#define __NET_IO_HANDLER_H_
#include "macros.h"
#include "udt.h"
#include "./reactor/ihandler.h"
#include "ringbuffer.h"
#include "inetaddr.h"
#include "lock.h"

NMS_BEGIN(network)
class CommandHandler;

enum NetIoHandlerLogicType {
	NHLT_CLIENT = 0, // connection between client and server.
	NHLT_GTW = 1, // connection from or to gateway.
	NHLT_WS = 2, // connection from or to world server.
	NHLT_GS = 3, // connection from or to game server.
	NHLT_DB = 4, // connection from or to database server.

	NHLT_MAX
};

enum HandlerDisconnectReasion {
	HDR_NONE = 0, // disconnect none
	HDR_KICK = 1, // kicked off
	HDR_ERR = 2, // error occured
	HDR_PEER = 3, // disconnect by peer.
};

class NetIoHandler : public kevent::IHandler {
	const static uint32 _s_max_packet = 1024 * 640; // default max 640k per packet.

	struct RecvHelper {
		byte *_ptr;
		uint32 _size;
		uint32 _len;

		RecvHelper() { reset(); }
		~RecvHelper() { reset(); }

		void reset() { _ptr = 0; _size = 0; _len = 0; }

		void release() { sfree(_ptr); reset(); }

		void* ptr() const { return _len < sizeof(_size) ? (void *)((char *)&_size + _len) : _ptr + _len; }

		uint32 len() const { return _len < sizeof(_size) ? (sizeof(_size) - _len) : (_size - _len); }

		bool bad_packet() const { return _len > 0 && !_ptr; }

		bool complete(uint32 len) {
			_len += len;
			if (_len == _size && _size > 0) return true;
			if (_len == sizeof(_size) && !_ptr) {
				uint32 size = _size;//::ntohl(_size);
				if (size == 0) return true;
				if (size >= _s_max_packet) return true;

				// TODO: use a pool instead.
				_ptr = (byte *)::malloc(size);
				*(uint32 *)_ptr = _size;
				_size = size;
				return false;
			}
			return false;
		}
	};

public:
	NetIoHandler() : _id(-1), _type(NHLT_MAX), _active(false), _send_ordered(false) { }
	NetIoHandler(int32 type) : _id(-1), _type(type), _active(false) { }
	~NetIoHandler();

	void addr(const kcommon::InetAddr &addr) { _inet_addr = addr; }
	const kcommon::InetAddr& addr() const { return _inet_addr; }

	int32 id() const { return _id; }
	void id(int32 id) { _id = id; }

	int32 type() const { return _type; }
	void type(int32 type) { _type = type; }

	virtual int32 handle_input();

	virtual int32 handle_output();

	virtual int32 handle_close();

	int32 send(const char *data, uint32 len, bool immediately = true);

	int32 kickoff();

	int32 reconnect();

	// return packet num handled.
	int32 update_base();

	void active(bool active) { _active = active; }
	bool active() const { return _active; }

	virtual void handle_packet(const char *data) { }

	virtual void handle_connect() { }

	virtual void handle_disconnect() { }

private:
	int32 send_immediate(const char *data, uint32 len);

private:
	int32 _id;
	int32 _type;
	volatile bool _active;
	volatile bool _send_ordered;
	kcommon::InetAddr _inet_addr;
	kcommon::RingBuffer<byte> _snd_buff;
	kcommon::RingBuffer<byte *> _recved_packet;
	RecvHelper _recv_helper;
};

NMS_END // end namespace network

#endif // __NET_IO_HANDLER_H_

