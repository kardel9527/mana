#ifndef __TEST_C2S_SESSION_H_
#define __TEST_C2S_SESSION_H_
#include "session.h"
#include "sessionmgr.h"

class TestC2SSession : public kcore::Session {
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

#endif // __TEST_C2S_SESSION_H_

