#ifndef __I_SESSION_MGR_H
#define __I_SESSION_MGR_H
#include "macros.h"
#include "udt.h"

NMS_BEGIN(kevent)
class NetIoHandler;
NMS_END

class ISessionMgr {
public:
	virtual ~ISessionMgr() {}

	virtual void handle_new_connect(kevent::NetIoHandler *handler) {}

	virtual void handle_disconnect(kevent::NetIoHandler *handler) {}
};

#endif // __I_SESSION_MGR_H

