#ifndef __TEST_S2C_SESSION_H_
#define __TEST_S2C_SESSION_H_
#include "session.h"
#include "sessionmgr.h"

class TestS2CSession : public kcore::Session {
public:
	virtual void handle_packet(const char *data);

	virtual void handle_connect();

	virtual void handle_disconnect();
};

class SessionMgrTest : public kcore::SessionMgr {
public:
	virtual kcore::Session* create(int32 type);

	virtual void destroy(kcore::Session *s);
};

#endif // __TEST_S2C_SESSION_H_

