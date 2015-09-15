#ifndef __SESSION_H_
#define __SESSION_H_
#include "udt.h"
#include "macros.h"
#include "bytebuffer.h"
#include "ringbuffer.h"

NMS_BEGIN(kcommon)

class SessionMgr;
class NetIoHandler;

class Session {
public:
	enum SessionStatus { 
		SS_NONE = 0, 
		SS_CONNECTED =1, 
		SS_AUTHED = 2, 
	};

	enum SessionType {
		ST_NONE = 0, 
		ST_C2S = 1,
		ST_GW2G = 2,
		ST_W2G = 3,
	};

public:
	Session();
	~Session();

	kcommon::NetIoHandler* io_handler() { return _io_handler; }

	void on_recv(ReadBuffer *buff);

	int32 id();

	void update();

	int32 send(const char *data, int32 len, bool immediately = false);

	void kickoff();

	virtual void handle_packet(ReadBuffer *packet) {}

	virtual void handle_connect() {}

	virtual void handle_disconnect() {}

	Session::SessionStatus status() { return _state; }

	void type(int32 session_type) { _type = (Session::SessionType)session_type; }

	void mgr(kcommon::SessionMgr *mgr) { _mgr = mgr; }

	kcommon::SessionMgr* mgr() { return _mgr; }

private:
	Session(const Session &other) {}
	Session& operator = (const Session &rh) { return *this; }

	void process_packet();

	void keep_alive();

private:
	SessionType _type;
	SessionStatus _state;
	kcommon::NetIoHandler *_io_handler;
	kcommon::SessionMgr *_mgr;
	kcommon::RingBuffer<ReadBuffer *> _msg_queue;
};

NMS_END // end namespace kcommon

#endif // __SESSION_H_

