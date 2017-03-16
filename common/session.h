#ifndef __SESSION_H_
#define __SESSION_H_
#include "udt.h"
#include "macros.h"

NMS_BEGIN(kevent)
	class NetIoHandler;
NMS_END

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

	void connected();

	void disconnected();

private:
	kevent::NetIoHandler *_handler;
};

#endif // __SESSION_H_

