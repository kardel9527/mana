#ifndef __SESSION_H_
#define __SESSION_H_
#include "udt.h"
#include "macros.h"
#include "isession.h"

NMS_BEGIN(kevent)
class NetIoHandler;
NMS_END

class Session : public ISession {
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

	virtual void on_recv(const char *data, uint32 len);

	void on_connect(kevent::NetIoHandler *handler);

	void on_disconnect();

	Session::SessionStatus get_status() { return _state; }

	void update();

private:
	void keep_alive();

private:
	SessionStatus _state;
	kevent::NetIoHandler *_handler;
};

#endif // __SESSION_H_

