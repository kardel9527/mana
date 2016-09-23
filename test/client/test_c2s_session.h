#ifndef __TEST_C2S_SESSION_H_
#define __TEST_C2S_SESSION_H_
#include "session.h"
#include "sessionmgr.h"

class TestC2SSession : public kcommon::Session {
public:
	virtual void handle_packet(kcommon::ReadBuffer *packet);

	virtual void handle_connect();

	virtual void handle_disconnect();
};

class SessionMgrTest : public kcommon::SessionMgr {
public:
	virtual kcommon::Session* create(int32 type);

	virtual void destroy(kcommon::Session *s);
};

#endif // __TEST_C2S_SESSION_H_

