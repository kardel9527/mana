#ifndef __SESSION_H_
#define __SESSION_H_
#include "udt.h"
#include "macros.h"
#include "fastqueue.h"

NMS_BEGIN(kcore)

class SessionMgr;
class NetIoHandler;
struct PacketHeader;

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

	NetIoHandler* io_handler() { return _io_handler; }

	void on_recv(const char *data);

	int32 id() { return _id; }

	void id(int32 cid) { _id = cid; }

	void update();

	int32 send(const char *data, int32 len, bool immediately = false);

	void kickoff();

	virtual void handle_packet(const char *data) {}

	virtual void handle_connect() {}

	virtual void handle_disconnect() {}

	Session::SessionStatus status() { return _state; }

	void type(int32 session_type) { _type = (Session::SessionType)session_type; }

	Session::SessionType type() { return _type; }

	void mgr(SessionMgr *mgr) { _mgr = mgr; }

	SessionMgr* mgr() { return _mgr; }

private:
	Session(const Session &other) {}
	Session& operator = (const Session &rh) { return *this; }

	void process_packet();

	void keep_alive();

private:
	int32 _id;
	SessionType _type;
	SessionStatus _state;
	NetIoHandler *_io_handler;
	SessionMgr *_mgr;
	kcommon::FastQueue<char *> _msg_queue;
};

NMS_END // end namespace kcore

#endif // __SESSION_H_

