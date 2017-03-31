#ifndef __TEST_C2S_SESSION_H_
#define __TEST_C2S_SESSION_H_
#include "session.h"

class TestC2SSession : public kcommon::Session {
public:
	virtual void handle_packet(ReadBuffer *packet);

	virtual void handle_connect();

	virtual void handle_disconnect();
};

#endif // __TEST_C2S_SESSION_H_

